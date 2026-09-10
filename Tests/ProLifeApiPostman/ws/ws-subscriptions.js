// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
//
// WebSocket subscription suite for the ProLife server.
//
// Newman does not execute Postman WebSocket requests, so this runs the WS side
// separately, against the same server the newman suite uses and with the same
// environment file, writing JUnit XML for the same TeamCity report step.
//
// What this covers that the Puma suite cannot: a ProLife client subscribes to
// the ProLife server, which serves some collections itself and forwards the rest
// to Puma or Lisa. Both halves of that routing decision are exercised here - the
// Puma suite only sees requests that already arrived.
//
// Protocol (see ImtCore CWebSocketServletComp / Qml/imtguigql/SubscriptionManager.qml):
//   -> {"type":"connection_init"}                                   <- {"type":"connection_ack"}
//   -> {"id":..,"type":"start","headers":{..},"payload":{"data":Q}} <- (silence on success)
//                                                                   <- {"type":"data","id":..,"payload":{..}}
//   -> {"id":..,"type":"stop"}                                      <- {"type":"complete","id":..}
//   errors arrive as {"id":..,"type":"error","payload":[{message,extensions:{type}}]}
//   the server also pushes unsolicited {"type":"ka"} keep-alives
//
// Every assertion here fails on a mismatch rather than on absence alone: refusals
// are checked against the reason the server actually gives, notifications against
// their documentId and operation, and every inbound frame against the accounting
// in SubscriptionSession.audit() - an error, a data frame or a complete for an id
// the suite did not expect fails the run even when no single case asked for it.

'use strict';

const fs = require('fs');
const path = require('path');
const http = require('http');
const https = require('https');
const crypto = require('crypto');
const WebSocket = require('ws');

// ---------------------------------------------------------------- configuration

function parseArgs(argv) {
	const out = {};
	for (let i = 2; i < argv.length; i++) {
		const a = argv[i];
		if (a.startsWith('--')) {
			const key = a.slice(2);
			const next = argv[i + 1];
			if (next && !next.startsWith('--')) { out[key] = next; i++; }
			else { out[key] = true; }
		}
	}
	return out;
}

const args = parseArgs(process.argv);

const envPath = args.env || path.join(__dirname, '..', 'ProLifeApi-Dev.postman_environment.json');
const envValues = {};
if (fs.existsSync(envPath)) {
	const parsed = JSON.parse(fs.readFileSync(envPath, 'utf8'));
	for (const entry of (parsed.values || [])) {
		envValues[entry.key] = entry.value;
	}
}

// The collection templates every request as "{{baseUrl}}/graphql", so baseUrl
// stops one segment short of the endpoint - accept either form here.
const baseUrl = args.http || process.env.WS_TEST_HTTP_URL || envValues.baseUrl || 'http://localhost:17778/ProLife';
const httpUrl = baseUrl.endsWith('/graphql') ? baseUrl : baseUrl.replace(/[/]+$/, '') + '/graphql';
const wsUrl = args.ws || process.env.WS_TEST_WS_URL || deriveWsUrl(httpUrl);
const suLogin = args['su-login'] || envValues.suLogin || 'su';
const suPassword = args['su-password'] || envValues.suPassword || '1';
const productId = args['product-id'] || envValues.productId || 'ProLife';
const junitPath = args.junit || path.join(__dirname, '..', 'junit-report-ws.xml');
const frameTimeoutMs = Number(args.timeout || 15000);
// How long a refusal has to arrive before a registration counts as accepted.
const registrationGraceMs = Number(args['grace'] || 2500);
// How long a notification already in flight may still land after its "complete".
const drainGraceMs = 750;

// The WS listener is a separate port from HTTP (ProLifeServerTest.acc: HttpPort 17778,
// WebSocketPort 18778). Derive it by convention, override with --ws when it differs.
function deriveWsUrl(httpEndpoint) {
	const u = new URL(httpEndpoint);
	const httpPort = Number(u.port || (u.protocol === 'https:' ? 443 : 80));
	const wsPort = process.env.WS_TEST_WS_PORT || (httpPort + 1000);
	const scheme = u.protocol === 'https:' ? 'wss:' : 'ws:';
	return `${scheme}//${u.hostname}:${wsPort}`;
}

// ---------------------------------------------------------------- http graphql

// Values reach a query through JSON.stringify rather than bare interpolation: a
// quote in a document name the server handed back would otherwise produce a
// syntactically broken query and a failure that reads like a protocol fault.
function gqlString(value) {
	return JSON.stringify(String(value));
}

function gql(query, token) {
	return new Promise((resolve, reject) => {
		const u = new URL(httpUrl);
		const body = JSON.stringify({ query });
		const headers = {
			'Content-Type': 'application/json',
			'Content-Length': Buffer.byteLength(body)
		};
		if (token) { headers['x-authentication-token'] = token; }
		if (productId) { headers['productId'] = productId; }

		const lib = u.protocol === 'https:' ? https : http;
		const req = lib.request({
			hostname: u.hostname,
			port: u.port,
			path: u.pathname + u.search,
			method: 'POST',
			headers
		}, (res) => {
			let data = '';
			res.on('data', (chunk) => { data += chunk; });
			res.on('end', () => {
				try { resolve(JSON.parse(data)); }
				catch (e) { reject(new Error(`Non-JSON response (${res.statusCode}): ${data.slice(0, 300)}`)); }
			});
		});
		req.on('error', reject);
		req.write(body);
		req.end();
	});
}

// Fails on a GraphQL "errors" array too - a mutation that did not happen must not
// be read as a missing notification three assertions later.
async function mutate(label, query, token) {
	const res = await gql(query, token);
	if (res && res.errors) {
		throw new Error(`${label} returned GraphQL errors: ${JSON.stringify(res.errors).slice(0, 400)}`);
	}
	const payload = res && res.data && res.data[label];
	if (!payload) {
		throw new Error(`${label} returned no data: ${JSON.stringify(res).slice(0, 400)}`);
	}
	return payload;
}

async function login(loginName, password) {
	const res = await gql(
		`query Authorization { Authorization(input: { login: ${gqlString(loginName)}, password: ${gqlString(password)},` +
		` productId: ${gqlString(productId)} })` +
		' { token userId username permissions systemId } }');
	const auth = res && res.data && res.data.Authorization;
	if (!auth || !auth.token) {
		throw new Error(`Authorization failed for '${loginName}': ${JSON.stringify(res).slice(0, 300)}`);
	}
	return auth;
}

// ---------------------------------------------------------------- ws client

// Types the client (Qml/imtguigql/SubscriptionClient.qml) and CWebSocketServletComp
// between them can produce. Anything else on the wire is a protocol violation.
const KNOWN_FRAME_TYPES = new Set(['connection_ack', 'data', 'error', 'complete', 'ka', 'pong', 'start_ack']);

class SubscriptionSession {
	constructor(label, url, token) {
		this.label = label;
		this.url = url;
		this.token = token;
		this.frames = [];
		this.waiters = [];
		this.socket = null;

		// Frame accounting. Every data/error/complete frame has to be attributable to
		// something the suite asked for; whatever is not is collected in violations
		// and fails the run in the final case.
		this.violations = [];
		this.activeIds = new Map();      // id -> command id its data frames must carry
		this.retiredIds = new Map();     // id -> { commandId, at } once its complete arrived
		this.expectErrorIds = new Set();
		this.expectCompleteIds = new Set();
	}

	connect() {
		return new Promise((resolve, reject) => {
			const socket = new WebSocket(this.url);
			this.socket = socket;

			const failTimer = setTimeout(() => reject(new Error(`WS connect timeout: ${this.url}`)), frameTimeoutMs);

			socket.on('open', () => { clearTimeout(failTimer); resolve(); });
			socket.on('error', (err) => { clearTimeout(failTimer); reject(err); });
			socket.on('message', (raw) => this.onFrame(raw.toString()));
		});
	}

	violation(text) {
		this.violations.push(`[${this.label}] ${text}`);
	}

	onFrame(raw) {
		let frame;
		try { frame = JSON.parse(raw); }
		catch (e) {
			this.violation(`unparsable frame: ${raw.slice(0, 200)}`);
			frame = { type: 'unparsable', raw };
		}

		this.frames.push(frame);
		this.audit(frame);

		for (let i = this.waiters.length - 1; i >= 0; i--) {
			const waiter = this.waiters[i];
			if (waiter.predicate(frame)) {
				this.waiters.splice(i, 1);
				clearTimeout(waiter.timer);
				waiter.resolve(frame);
			}
		}
	}

	// Shape and attribution checks applied to every inbound frame.
	audit(frame) {
		if (frame.type === 'unparsable') { return; }

		if (!KNOWN_FRAME_TYPES.has(frame.type)) {
			this.violation(`unknown frame type "${frame.type}": ${JSON.stringify(frame).slice(0, 200)}`);
			return;
		}

		if (frame.type === 'connection_ack') {
			const timeout = frame.payload && frame.payload.connectionTimeoutMs;
			if (typeof timeout !== 'number' || timeout <= 0) {
				this.violation(`connection_ack without a usable connectionTimeoutMs: ${JSON.stringify(frame).slice(0, 200)}`);
			}
			return;
		}

		if (frame.type === 'ka' || frame.type === 'pong') { return; }

		if (typeof frame.id !== 'string') {
			this.violation(`"${frame.type}" frame carries no string id: ${JSON.stringify(frame).slice(0, 200)}`);
			return;
		}

		if (frame.type === 'error') {
			this.auditErrorShape(frame);
			if (!this.expectErrorIds.has(frame.id)) {
				this.violation(`unsolicited error for id "${frame.id}": ${JSON.stringify(frame.payload).slice(0, 250)}`);
			}
			return;
		}

		if (frame.type === 'complete') {
			if (!this.activeIds.has(frame.id) && !this.retiredIds.has(frame.id) && !this.expectCompleteIds.has(frame.id)) {
				this.violation(`complete for an id the suite never registered: "${frame.id}"`);
			}
			return;
		}

		const expectedCommand = this.activeIds.has(frame.id)
			? this.activeIds.get(frame.id)
			: this.retiredCommandWithinGrace(frame.id);

		if (expectedCommand === undefined) {
			this.violation(`data frame for id "${frame.id}", which holds no live subscription: ` +
				JSON.stringify(frame.payload).slice(0, 250));
			return;
		}

		const data = frame.payload && frame.payload.data;
		if (!data || typeof data !== 'object') {
			this.violation(`data frame for "${frame.id}" has no payload.data object: ${JSON.stringify(frame).slice(0, 250)}`);
			return;
		}
		if (!Object.prototype.hasOwnProperty.call(data, expectedCommand)) {
			this.violation(`data frame for "${frame.id}" carries ${JSON.stringify(Object.keys(data))} ` +
				`instead of the subscribed command "${expectedCommand}"`);
		}
	}

	auditErrorShape(frame) {
		const payload = frame.payload;
		if (!Array.isArray(payload) || payload.length === 0) {
			this.violation(`error frame payload is not a non-empty array: ${JSON.stringify(frame).slice(0, 250)}`);
			return;
		}
		for (const entry of payload) {
			if (!entry || typeof entry.message !== 'string' || entry.message.length === 0) {
				this.violation(`error entry without a message: ${JSON.stringify(entry).slice(0, 250)}`);
			}
			if (!entry || !entry.extensions || typeof entry.extensions.type !== 'string') {
				this.violation(`error entry without extensions.type: ${JSON.stringify(entry).slice(0, 250)}`);
			}
		}
	}

	retiredCommandWithinGrace(id) {
		const retired = this.retiredIds.get(id);
		if (retired === undefined) { return undefined; }
		if (Date.now() - retired.at > drainGraceMs) { return undefined; }
		return retired.commandId;
	}

	// Index into frames, so a wait can be scoped to what arrives after this point.
	// Without it a predicate matching on type alone is satisfied by a frame from an
	// earlier case and the wait proves nothing.
	mark() {
		return this.frames.length;
	}

	waitFor(predicate, timeoutMs, fromIndex) {
		const start = fromIndex === undefined ? 0 : fromIndex;
		const existing = this.frames.slice(start).find(predicate);
		if (existing) { return Promise.resolve(existing); }

		return new Promise((resolve, reject) => {
			const waiter = { predicate, resolve };
			waiter.timer = setTimeout(() => {
				const index = this.waiters.indexOf(waiter);
				if (index >= 0) { this.waiters.splice(index, 1); }
				reject(new Error(`Timed out after ${timeoutMs}ms waiting for a matching frame. Seen since mark: ` +
					JSON.stringify(this.frames.slice(start).map(f => ({ type: f.type, id: f.id }))).slice(0, 500)));
			}, timeoutMs);
			this.waiters.push(waiter);
		});
	}

	send(obj) {
		this.socket.send(JSON.stringify(obj));
	}

	async init() {
		const mark = this.mark();
		this.send({ type: 'connection_init' });
		return this.waitFor(f => f.type === 'connection_ack', frameTimeoutMs, mark);
	}

	// Frames are built exactly as Qml/imtguigql/SubscriptionManager.qml builds them
	// for a Qml/imtguigql/SubscriptionClient.qml. Two details are easy to get wrong
	// and both are load-bearing: the payload carries the query as a JSON *string*
	// ({"query": "subscription ..."}) rather than raw GraphQL, and the access token
	// and productId travel in the frame's own headers, not the socket's. Any other
	// shape tests a protocol the client does not speak - and a malformed payload is
	// refused by CGqlRequest::ParseQuery before the case under test is ever reached,
	// which turns a negative case into a pass for the wrong reason.
	headers() {
		const headers = {};
		if (this.token) { headers['x-authentication-token'] = this.token; }
		if (productId) { headers['productId'] = productId; }
		return headers;
	}

	startFrame(id, query) {
		return { id, headers: this.headers(), type: 'start', payload: { data: JSON.stringify({ query }) } };
	}

	stopFrame(id) {
		return { id, headers: this.headers(), type: 'stop', payload: {} };
	}

	// A successful "start" is answered with silence: nothing in ImtCore ever emits
	// start_ack (CWebSocketRequest only parses one), and CWebSocketServletComp
	// returns an empty response on success. So a refusal is the only registration
	// outcome that produces a frame, and the absence of one is the acceptance.
	settle(id, waitMs) {
		const isOutcome = (f) => f.id === id && (f.type === 'error' || f.type === 'start_ack');
		const existing = this.frames.find(isOutcome);
		if (existing) { return Promise.resolve(existing.type === 'error' ? existing : null); }

		return new Promise((resolve) => {
			const waiter = { predicate: isOutcome, resolve: (frame) => resolve(frame.type === 'error' ? frame : null) };
			waiter.timer = setTimeout(() => {
				const index = this.waiters.indexOf(waiter);
				if (index >= 0) { this.waiters.splice(index, 1); }
				resolve(null);
			}, waitMs === undefined ? registrationGraceMs : waitMs);
			this.waiters.push(waiter);
		});
	}

	// Registers without waiting, so a batch can share one grace window instead of
	// paying it once per subscription.
	beginSubscribe(commandId, query) {
		const id = crypto.randomUUID();
		this.activeIds.set(id, commandId);
		this.expectErrorIds.add(id);
		this.send(this.startFrame(id, query));
		return id;
	}

	// Turns an id whose grace window has passed into an accepted subscription, or
	// throws with the refusal the server gave.
	confirmAccepted(id) {
		const refusal = this.frames.find(f => f.id === id && f.type === 'error');
		if (refusal) {
			this.activeIds.delete(id);
			throw new Error(`Subscription refused: ${JSON.stringify(refusal.payload).slice(0, 400)}`);
		}
		this.expectErrorIds.delete(id);
		return id;
	}

	async subscribe(commandId, query) {
		const id = this.beginSubscribe(commandId, query);
		await this.settle(id);
		return this.confirmAccepted(id);
	}

	// Registers and requires the server to refuse. Returns the error frame so the
	// caller can assert on the reason; returns null when the server accepted it.
	async subscribeExpectingRefusal(commandId, query) {
		const id = crypto.randomUUID();
		this.expectErrorIds.add(id);
		this.send(this.startFrame(id, query));

		const refusal = await this.settle(id);
		if (!refusal) {
			// Accepted after all - register it properly so the cleanup stop and any
			// notification it now produces are accounted for rather than reported twice.
			this.activeIds.set(id, commandId);
			await this.stop(id);
			return null;
		}

		return refusal;
	}

	async stop(id) {
		const mark = this.mark();
		const commandId = this.activeIds.get(id);
		this.expectCompleteIds.add(id);
		this.send(this.stopFrame(id));

		const frame = await this.waitFor(f => f.id === id && (f.type === 'complete' || f.type === 'error'),
			frameTimeoutMs, mark);

		if (this.activeIds.has(id)) {
			this.activeIds.delete(id);
			this.retiredIds.set(id, { commandId, at: Date.now() });
		}
		return frame;
	}

	dataFrames(id) {
		return this.frames.filter(f => f.type === 'data' && f.id === id);
	}

	waitForData(id, matcher) {
		return this.waitFor(f => f.type === 'data' && f.id === id && (!matcher || matcher(f)), frameTimeoutMs);
	}

	close() {
		if (this.socket) { this.socket.close(); }
	}
}

// A data frame wraps the notification as {"data": {"<CommandId>": {...}}} - the
// same shape SubscriptionClient.deliverReady() unwraps before handing it to the
// view. Reading frame.payload directly finds nothing, which fails slowly and
// looks exactly like "the notification never arrived".
function notificationOf(frame, commandId) {
	const data = frame && frame.payload && frame.payload.data;
	if (!data) { return null; }

	return commandId ? data[commandId] : data[Object.keys(data)[0]];
}

// ---------------------------------------------------------------- assertions

function assert(condition, message) {
	if (!condition) { throw new Error(message); }
}

// A refusal only counts when the server refused for the reason under test. Without
// this, a case that accidentally sends a malformed payload or a broken token still
// gets its error frame and passes while proving nothing.
function assertRefusalReason(frame, expectations) {
	assert(frame !== null && frame !== undefined, expectations.absent);

	const message = frame.payload && frame.payload[0] && frame.payload[0].message;
	assert(typeof message === 'string' && message.length > 0,
		`refusal carries no message: ${JSON.stringify(frame).slice(0, 300)}`);

	assert(!/Error when parsing/.test(message),
		'the server refused this because the frame itself was malformed, not because of the case under ' +
		`test - the case would prove nothing as written: "${message}"`);

	for (const forbidden of (expectations.mustNotMatch || [])) {
		assert(!forbidden.test(message), `refused for the wrong reason (matched ${forbidden}): "${message}"`);
	}

	assert(expectations.mustMatch.test(message),
		`refusal message does not match ${expectations.mustMatch}: "${message}"`);

	return message;
}

// What the routing cases require: the publisher's IsRequestSupported rejected the
// request, so no servlet claimed it (CWebSocketServletComp::RegisterSubscription).
function assertRoutingRefusal(frame, commandId, absent) {
	return assertRefusalReason(frame, {
		absent,
		mustMatch: new RegExp(`No servlet was found for the given command: '${commandId}`),
		mustNotMatch: [/^Unauthorized:/, /^Forbidden:/]
	});
}

function documentNotification(frame, commandId, documentId) {
	const notification = notificationOf(frame, commandId);
	return !!notification && notification.documentId === documentId;
}

// ---------------------------------------------------------------- test harness

const results = [];

async function test(name, fn) {
	const started = Date.now();
	try {
		await fn();
		results.push({ name, ok: true, ms: Date.now() - started });
		console.log(`  PASS  ${name}`);
	} catch (err) {
		results.push({ name, ok: false, ms: Date.now() - started, error: err && err.message ? err.message : String(err) });
		console.log(`  FAIL  ${name}\n        ${err && err.message ? err.message : err}`);
	}
	writeJUnit(junitPath);
}

// Cases downstream of a failed one must not run their assertions against undefined
// state - that reports a second, misleading defect. They fail explicitly instead.
function requires(value, what) {
	if (value === null || value === undefined || value === '') {
		throw new Error(`precondition not met: ${what} - an earlier case did not produce it, so this case was not verified`);
	}
	return value;
}

function writeJUnit(filePath) {
	const failures = results.filter(r => !r.ok).length;
	const totalMs = results.reduce((sum, r) => sum + r.ms, 0);
	const escape = (s) => String(s).replace(/[<>&"']/g, c => (
		{ '<': '&lt;', '>': '&gt;', '&': '&amp;', '"': '&quot;', "'": '&apos;' }[c]));

	const cases = results.map(r => {
		const head = `    <testcase classname="ProLifeApi.WebSocket" name="${escape(r.name)}" time="${(r.ms / 1000).toFixed(3)}"`;
		return r.ok
			? head + '/>'
			: head + `>\n      <failure message="${escape(r.error)}"/>\n    </testcase>`;
	}).join('\n');

	const xml =
		'<?xml version="1.0" encoding="UTF-8"?>\n' +
		`<testsuites name="ProLife API WebSocket subscriptions" tests="${results.length}" failures="${failures}" time="${(totalMs / 1000).toFixed(3)}">\n` +
		`  <testsuite name="ProLifeApi.WebSocket" tests="${results.length}" failures="${failures}" time="${(totalMs / 1000).toFixed(3)}">\n` +
		cases + '\n  </testsuite>\n</testsuites>\n';

	fs.writeFileSync(filePath, xml, 'utf8');
}

// ---------------------------------------------------------------- the suite

// Every subscription the QML client registers, in the exact shape it sends -
// empty input, "notification { id }" selection - taken from a live trace of a
// ProLife client start. Anything the client subscribes to has to be accepted by
// the server it connects to, whether that server answers itself or forwards to
// Puma or Lisa.
const clientSubscriptions = [
	// Connection probes (ConnectionStatusSubscriberController on each engine).
	'PumaWsConnection',
	'LisaWsConnection',
	// Tenant/authorisation notifications, forwarded to Puma.
	'OnMembershipNotification',
	'OnTenantsCollectionChanged',
	'OnCrossTenantMessageNotification',
	'OnConnectionCodesNotification',
	'OnRolesCollectionChanged',
	'OnGroupsCollectionChanged',
	'OnUsersForInvitationCollectionChanged',
	'OnTenantRelationshipsCollectionChanged',
	'OnCrossOrgGrantsCollectionChanged',
	// Desk, forwarded.
	'OnTicketMessageReceived',
	'OnTicketAssigneeChanged',
	'OnTicketsCollectionChanged',
	// Licensing, forwarded to Lisa.
	'OnProductsCollectionChanged',
	'OnLicensesCollectionChanged',
	// ProLife's own collections.
	'OnOrdersCollectionChanged',
	'OnAccountsCollectionChanged',
	'OnDevicesCollectionChanged',
	'OnSoftwareProductsCollectionChanged',
	'OnUserActionsCollectionChanged'
];

// The two probes answer immediately with the state of the upstream link, so they
// are the only entries above that also assert on a delivered payload.
const connectionProbes = ['PumaWsConnection', 'LisaWsConnection'];

// Served by Puma and forwarded by the ProLife server's document-service bridge.
const remoteCollections = ['Users', 'Groups', 'Roles', 'Tenants', 'TenantRelationships', 'CrossOrgGrants'];
// The only collection ProLife serves through a document service of its own:
// ProLifeQmlVoce.arp/ProLifeServerBase.acc registers exactly one
// CollectionDocumentServicePublisher, with CollectionId "Devices". A forwarder
// selecting on the command alone would claim this one too and send it to Puma,
// where it does not exist - so accepting it here is what tells the two halves apart.
const localCollections = ['Devices'];

// Served by ProLife, but through their own collection controller rather than a
// document service, so a document-service subscription for them has nothing to
// bind to and has to be refused. Listed explicitly rather than left out: this is
// what the server component registry says today, and adding a publisher for one of
// them has to move it up into localCollections instead of quietly starting to pass.
const documentServicelessCollections = ['Orders'];
// Desk collections, reached through their own engine.
const deskCollections = ['Tickets'];

const clientQuery = (commandId) =>
	`subscription ${commandId} { ${commandId}(input: {}) { notification { id } } }`;

const documentManagerQuery = (collectionId) =>
	'subscription OnDocumentManagerChanged { OnDocumentManagerChanged(input: { __typename: "CollectionId",' +
	` collectionId: ${gqlString(collectionId)} })` +
	' { documentOperation documentId documentName objectId isDirty isLoading } }';

const undoRedoQuery = (collectionId) =>
	'subscription OnUndoRedoChanged { OnUndoRedoChanged(input: { __typename: "CollectionId",' +
	` collectionId: ${gqlString(collectionId)} })` +
	' { documentId isDirty availableUndoSteps availableRedoSteps } }';

// Creates a draft in a collection and requires the notification for that exact
// document to come back on the given subscription. A UUID, not a readable label:
// these Id columns are uuid, so anything else is rejected on insert and surfaces
// as a bare "Failed to save document".
async function createDocumentAndExpectNotification(session, token, subscriptionId, collectionId, typeId) {
	const info = await mutate('CreateNewDocument',
		`mutation CreateNewDocument { CreateNewDocument(input: { collectionId: ${gqlString(collectionId)},` +
		` typeId: ${gqlString(typeId)}, proposedSourceDocumentId: ${gqlString(crypto.randomUUID())} })` +
		' { documentId documentName objectId } }', token);

	assert(info.documentId, `CreateNewDocument(${collectionId}) returned no documentId: ${JSON.stringify(info).slice(0, 300)}`);

	const frame = await session.waitForData(subscriptionId, f => {
		const notification = notificationOf(f, 'OnDocumentManagerChanged');
		return !!notification &&
			notification.documentId === info.documentId &&
			notification.documentOperation === 'NewDocumentCreated';
	});

	const notification = notificationOf(frame, 'OnDocumentManagerChanged');
	assert(notification.documentId === info.documentId,
		`notification is for document "${notification.documentId}", not "${info.documentId}"`);
	assert(notification.documentOperation === 'NewDocumentCreated',
		`expected documentOperation "NewDocumentCreated", got "${notification.documentOperation}"`);

	return info.documentId;
}

async function closeDocument(token, collectionId, documentId) {
	if (!documentId) { return; }
	await gql(`mutation CloseDocument { CloseDocument(input: { collectionId: ${gqlString(collectionId)},` +
		` id: ${gqlString(documentId)} }) { status message } }`, token).catch(() => {});
}

async function main() {
	console.log(`HTTP : ${httpUrl}`);
	console.log(`WS   : ${wsUrl}`);

	const su = await login(suLogin, suPassword);
	console.log(`Authenticated as '${suLogin}' (userId=${su.userId})\n`);

	const session = new SubscriptionSession('main', wsUrl, su.token);
	await session.connect();

	await test('connection_init is acknowledged', async () => {
		const frame = await session.init();
		assert(frame.type === 'connection_ack', `expected connection_ack, got ${JSON.stringify(frame).slice(0, 200)}`);
		assert(typeof frame.payload.connectionTimeoutMs === 'number' && frame.payload.connectionTimeoutMs > 0,
			`connection_ack must carry a positive connectionTimeoutMs, got ${JSON.stringify(frame.payload)}`);
	});

	// --- one case per subscription the client actually registers
	//
	// A rejected registration is answered with "No servlet was found for the given
	// command", which is what the whole screen goes quiet on. Registering each of
	// them the way the client does is the cheapest way to catch a command list or
	// a forwarding rule that drifted away from the client.
	//
	// Registered as one batch sharing a single grace window: a successful start is
	// answered with silence, so waiting per subscription would cost 2.5s each and
	// dominate the run.

	const clientSubIds = new Map();
	for (const commandId of clientSubscriptions) {
		clientSubIds.set(commandId, session.beginSubscribe(commandId, clientQuery(commandId)));
	}
	await new Promise(r => setTimeout(r, registrationGraceMs));

	// Before the acceptance loop stops them: the probes are the only entries that
	// deliver on their own, and what they deliver is the premise of every forwarded
	// case below.
	for (const commandId of connectionProbes) {
		await test(`'${commandId}' reports the upstream link as connected`, async () => {
			const id = session.confirmAccepted(clientSubIds.get(commandId));
			const frame = await session.waitForData(id);

			const notification = notificationOf(frame, commandId);
			assert(!!notification, `probe payload carries no "${commandId}": ${JSON.stringify(frame.payload).slice(0, 250)}`);
			assert(typeof notification.status === 'string' && notification.status.length > 0,
				`probe payload carries no status string: ${JSON.stringify(notification).slice(0, 250)}`);
			assert(notification.status === 'Connected',
				`the upstream link reports "${notification.status}" instead of "Connected" - every forwarded ` +
				'subscription below depends on this link, so they would be testing nothing');
		});
	}

	for (const commandId of clientSubscriptions) {
		await test(`the client subscription '${commandId}' is accepted`, async () => {
			const id = session.confirmAccepted(clientSubIds.get(commandId));
			const frame = await session.stop(id);
			assert(frame.type === 'complete',
				`stopping an accepted subscription answered ${JSON.stringify(frame).slice(0, 250)}`);
			assert(frame.id === id, `complete carries id "${frame.id}" instead of "${id}"`);
		});
	}

	// --- both halves of the routing decision
	//
	// The remote collections live in Puma: the ProLife server has to recognise the
	// command, forward it upstream *with its input arguments*, and bind the result.
	// The local ones are served here, and a forwarder selecting on the command alone
	// would claim them too and send them to Puma, where they do not exist. Only
	// passing both halves tells a correct forwarder from a greedy one.

	const routed = [];
	for (const collectionId of remoteCollections) {
		routed.push({
			name: `OnDocumentManagerChanged is forwarded for the remote collection '${collectionId}'`,
			commandId: 'OnDocumentManagerChanged',
			query: documentManagerQuery(collectionId)
		});
	}
	routed.push({
		name: "OnUndoRedoChanged is forwarded for the remote collection 'Groups'",
		commandId: 'OnUndoRedoChanged',
		query: undoRedoQuery('Groups')
	});
	for (const collectionId of localCollections) {
		routed.push({
			name: `OnDocumentManagerChanged stays local for the collection '${collectionId}'`,
			commandId: 'OnDocumentManagerChanged',
			query: documentManagerQuery(collectionId)
		});
	}
	for (const collectionId of deskCollections) {
		routed.push({
			name: `OnDocumentManagerChanged is accepted for the desk collection '${collectionId}'`,
			commandId: 'OnDocumentManagerChanged',
			query: documentManagerQuery(collectionId)
		});
	}

	for (const entry of routed) {
		entry.id = session.beginSubscribe(entry.commandId, entry.query);
	}
	await new Promise(r => setTimeout(r, registrationGraceMs));

	for (const entry of routed) {
		await test(entry.name, async () => {
			const id = session.confirmAccepted(entry.id);
			const frame = await session.stop(id);
			assert(frame.type === 'complete',
				`stopping an accepted subscription answered ${JSON.stringify(frame).slice(0, 250)}`);
		});
	}

	// --- delivery, end to end

	let localDocumentId = null;

	await test('a locally served document reaches its subscriber', async () => {
		const subscriptionId = await session.subscribe('OnDocumentManagerChanged', documentManagerQuery('Devices'));
		localDocumentId = await createDocumentAndExpectNotification(session, su.token, subscriptionId, 'Devices', 'Device');

		await closeDocument(su.token, 'Devices', localDocumentId);
		const frame = await session.stop(subscriptionId);
		assert(frame.type === 'complete', `stop answered ${JSON.stringify(frame).slice(0, 250)}`);
	});

	await test('a document created on Puma reaches the subscriber through the chain', async () => {
		const subscriptionId = await session.subscribe('OnDocumentManagerChanged', documentManagerQuery('Tenants'));

		// The whole point of the suite: the notification is produced on Puma and has
		// to travel back through the bridge to this socket, carrying the id of the
		// document that was actually created.
		const documentId = await createDocumentAndExpectNotification(session, su.token, subscriptionId, 'Tenants', 'Tenant');

		await closeDocument(su.token, 'Tenants', documentId);
		const frame = await session.stop(subscriptionId);
		assert(frame.type === 'complete', `stop answered ${JSON.stringify(frame).slice(0, 250)}`);
	});

	await test('stopping a forwarded subscription ends delivery through the bridge', async () => {
		// "complete" is only half the contract, and on the forwarding path the other
		// half is the one that breaks: the bridge holds an upstream subscription of
		// its own, and a stop that unbinds locally without unbinding upstream keeps
		// a closed view's notifications arriving.
		const subscriptionId = await session.subscribe('OnDocumentManagerChanged', documentManagerQuery('Tenants'));

		const frame = await session.stop(subscriptionId);
		assert(frame.type === 'complete', `stop answered ${JSON.stringify(frame).slice(0, 250)}`);
		const stoppedAt = session.dataFrames(subscriptionId).length;

		const info = await mutate('CreateNewDocument',
			'mutation CreateNewDocument { CreateNewDocument(input: { collectionId: "Tenants", typeId: "Tenant",' +
			` proposedSourceDocumentId: ${gqlString(crypto.randomUUID())} }) { documentId } }`, su.token);
		assert(info.documentId, `CreateNewDocument(Tenants) returned no documentId: ${JSON.stringify(info).slice(0, 250)}`);

		// A live subscription on the same collection proves the notification really
		// was produced and forwarded, so the silence on the stopped one is a real
		// unregistration and not an upstream that simply had nothing to say.
		const witnessId = await session.subscribe('OnDocumentManagerChanged', documentManagerQuery('Tenants'));
		const witnessMark = session.mark();
		const second = await mutate('CreateNewDocument',
			'mutation CreateNewDocument { CreateNewDocument(input: { collectionId: "Tenants", typeId: "Tenant",' +
			` proposedSourceDocumentId: ${gqlString(crypto.randomUUID())} }) { documentId } }`, su.token);
		await session.waitFor(f => f.type === 'data' && f.id === witnessId &&
			documentNotification(f, 'OnDocumentManagerChanged', second.documentId), frameTimeoutMs, witnessMark);

		const leaked = session.dataFrames(subscriptionId).slice(stoppedAt);
		assert(leaked.length === 0,
			`${leaked.length} notification(s) were still forwarded to a stopped subscription: ` +
			JSON.stringify(leaked.map(f => notificationOf(f, 'OnDocumentManagerChanged'))).slice(0, 300));

		await closeDocument(su.token, 'Tenants', info.documentId);
		await closeDocument(su.token, 'Tenants', second.documentId);
		await session.stop(witnessId);
	});

	// --- protocol hygiene on the forwarding server

	await test('stop of an unknown subscription is idempotent and carries its id', async () => {
		const unknownId = crypto.randomUUID();
		const mark = session.mark();
		session.expectCompleteIds.add(unknownId);
		session.expectErrorIds.add(unknownId);
		session.send(session.stopFrame(unknownId));
		const frame = await session.waitFor(f => (f.type === 'error' || f.type === 'complete') && f.id === unknownId,
			frameTimeoutMs, mark);

		// Unregistering what the server no longer holds is the caller's intent, so it
		// answers "complete". Whatever it answers has to carry the id: the client
		// attributes frames to subscriptions by id and cannot act on one without.
		assert(frame.id === unknownId, `frame carries id="${frame.id}" instead of "${unknownId}"`);
		assert(frame.type === 'complete',
			`expected an idempotent "complete", got ${JSON.stringify(frame).slice(0, 300)}`);
	});

	await test('double stop does not desynchronise the session', async () => {
		const probeId = await session.subscribe('OnDocumentManagerChanged', documentManagerQuery('Devices'));

		const first = await session.stop(probeId);
		assert(first.type === 'complete', `first stop answered ${JSON.stringify(first).slice(0, 250)}`);

		// Scoped to this id and to what arrives after this point. An unscoped wait is
		// satisfied instantly by any earlier "complete" and asserts nothing.
		const mark = session.mark();
		session.send(session.stopFrame(probeId));
		const second = await session.waitFor(f => f.id === probeId && (f.type === 'complete' || f.type === 'error'),
			frameTimeoutMs, mark);
		assert(second.type === 'complete',
			`a repeated stop answered ${JSON.stringify(second).slice(0, 250)} instead of an idempotent "complete"`);

		// The session must still serve new subscriptions afterwards, including
		// forwarded ones - a desynchronised bridge shows up here and nowhere else.
		const afterId = await session.subscribe('OnDocumentManagerChanged', documentManagerQuery('Tenants'));
		const closing = await session.stop(afterId);
		assert(closing.type === 'complete',
			`the session no longer serves subscriptions after a double stop: ${JSON.stringify(closing).slice(0, 250)}`);
	});

	for (const collectionId of documentServicelessCollections) {
		await test(`OnDocumentManagerChanged is refused for '${collectionId}', which has no document service`, async () => {
			const frame = await session.subscribeExpectingRefusal('OnDocumentManagerChanged',
				documentManagerQuery(collectionId));

			assertRoutingRefusal(frame, 'OnDocumentManagerChanged',
				`a document-service subscription was accepted for '${collectionId}', which no ` +
				'CollectionDocumentServicePublisher serves - either a publisher was added and this list ' +
				'is stale, or something is binding a subscription that can never deliver');
		});
	}

	await test('a subscription for a collection no one serves is refused', async () => {
		const frame = await session.subscribeExpectingRefusal('OnDocumentManagerChanged',
			documentManagerQuery('NoSuchCollection'));

		assertRoutingRefusal(frame, 'OnDocumentManagerChanged',
			'a collection served neither locally nor upstream was accepted - the forwarder is claiming everything');
	});

	await test('a document-service subscription without input is refused', async () => {
		// The forwarder selects on the input arguments upstream. One that passes the
		// command on without them binds nothing and delivers nothing, while looking
		// healthy from here unless the server is strict about the arguments.
		const frame = await session.subscribeExpectingRefusal('OnDocumentManagerChanged',
			'subscription OnDocumentManagerChanged { OnDocumentManagerChanged { documentOperation documentId } }');

		assertRoutingRefusal(frame, 'OnDocumentManagerChanged',
			'a document-service subscription carrying no input was accepted - a forwarder that drops ' +
			'the arguments would look healthy while delivering nothing');
	});

	await test('start with an empty subscription id is refused', async () => {
		// Everything on both sides is keyed on this id: the publisher map, data
		// frames, errors and stop. Accepting an empty one makes every subscription
		// that has it collide on a single key and receive each other's messages.
		//
		// The payload is the well-formed one every other case uses - raw GraphQL here
		// is refused by CGqlRequest::ParseQuery before the id is ever looked at, and
		// the case would pass without the server having checked anything about it.
		session.expectErrorIds.add('');
		session.expectCompleteIds.add('');
		session.send(session.startFrame('', documentManagerQuery('Devices')));

		const refusal = await session.settle('');
		if (!refusal) {
			session.send(session.stopFrame(''));
		}

		assertRefusalReason(refusal, {
			absent: 'the server bound a subscription under an empty id',
			mustMatch: /.+/,
			mustNotMatch: [/^Unauthorized:/, /^Forbidden:/]
		});
	});

	const rogue = new SubscriptionSession('rogue', wsUrl, 'not-a-valid-token');

	await test('a subscription registered with an invalid token is refused as an auth failure', async () => {
		await rogue.connect();
		await rogue.init();

		const frame = await rogue.subscribeExpectingRefusal('OnDocumentManagerChanged',
			documentManagerQuery('Devices'));

		// The class of refusal matters: a bogus credential has to come back as an
		// authentication/authorization failure. Anything else - "no servlet found",
		// an internal error - means the token was never the thing that stopped it.
		assertRefusalReason(frame, {
			absent: 'the server accepted a subscription presenting an invalid access token',
			mustMatch: /^(Unauthorized|Forbidden): /,
			mustNotMatch: [/No servlet was found/]
		});
	});

	// Everything SubscriptionSession.audit() collected along the way: frames of an
	// unknown type or shape, errors nobody asked for, notifications delivered to an
	// id that holds no subscription, payloads carrying another command's data. No
	// case above looks for these, which is the point - this is what catches a
	// mismatch no assertion was written for, including one introduced by the bridge.
	await test('no unaccounted frames or protocol violations on any session', async () => {
		const violations = session.violations.concat(rogue.violations);
		assert(violations.length === 0,
			`${violations.length} protocol violation(s):\n        - ` + violations.join('\n        - '));
	});

	rogue.close();
	session.close();
}

main()
	.catch((err) => {
		results.push({ name: 'suite bootstrap', ok: false, ms: 0, error: err && err.message ? err.message : String(err) });
		console.error(`\nSuite aborted: ${err && err.message ? err.message : err}`);
	})
	.finally(() => {
		writeJUnit(junitPath);
		const failures = results.filter(r => !r.ok).length;
		console.log(`\n${results.length - failures}/${results.length} passed. JUnit: ${junitPath}`);
		process.exit(failures > 0 ? 1 : 0);
	});
