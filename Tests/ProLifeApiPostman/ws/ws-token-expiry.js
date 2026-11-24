// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
// ProLife -> Puma: the access token of a ProLife session expires while its subscriptions, local and forwarded to Puma, are live.
// --mode token: ProLife holds a server token of Puma; --mode notoken: it has none and checks the sessions itself.
// Puma runs with short sessions (Run-CiTests.ps1 puts them into its settings): --jwt-lifetime and --session-lifetime name them.
// The harness is the one of the Puma suites (--puma-dir); a case restarts Puma (--puma-exe, --puma).
const crypto = require('crypto');
const fs = require('fs');
const http = require('http');
const path = require('path');
const { spawn } = require('child_process');

function argument(name, fallback) {
	const index = process.argv.indexOf(`--${name}`);
	return index > 0 && index + 1 < process.argv.length ? process.argv[index + 1] : fallback;
}

const pumaDir = argument('puma-dir', process.env.PUMADIR || path.join(__dirname, '..', '..', '..', '..', 'Puma'));
const lib = require(path.join(pumaDir, 'Tests', 'TenantApiPostman', 'ws', 'ws-lib.js'));
const { SubscriptionSession, gqlString, notificationOf, assert, jwtClaim, delay, test, runSuite } = lib;

const suPassword = lib.suPassword;
const jwtLifetime = Number(argument('jwt-lifetime', 20));
const sessionLifetime = Number(argument('session-lifetime', 75));
const pumaExe = argument('puma-exe', path.join(pumaDir, 'Bin', 'Release_Qt6_VC17_x64', 'PumaServerPgTest.exe'));
const pumaUrl = new URL(argument('puma', 'http://localhost:17788/Puma').replace(/[/]+$/, '') + '/graphql');
const pumaLog = argument('puma-log', '');
const started = Date.now();
const nowSecs = () => Math.floor(Date.now() / 1000);
const log = (text) => console.log(`  [t+${((Date.now() - started) / 1000).toFixed(1)}s] ${text}`);
const protectedQuery = 'query GetExternalIdentities { GetExternalIdentities { items { providerId } } }';
const managerQuery = (collectionId) =>
	'subscription OnDocumentManagerChanged { OnDocumentManagerChanged(input: { __typename: "CollectionId",' +
	` collectionId: ${gqlString(collectionId)} }) { documentOperation documentId documentName objectId isDirty isLoading } }`;
// Tenants live on Puma and come through the bridge of ProLife; Devices are served by ProLife itself
const RELAY = { collectionId: 'Tenants', typeId: 'Tenant' };
const LOCAL = { collectionId: 'Devices', typeId: 'Device' };

function post(query, token, port, pathname) {
	return new Promise((resolve, reject) => {
		const u = new URL(lib.httpUrl);
		const body = JSON.stringify({ query });
		const headers = { 'Content-Type': 'application/json', 'Content-Length': Buffer.byteLength(body), productId: lib.productId };
		if (token) { headers['x-authentication-token'] = token; }
		const req = http.request({ hostname: u.hostname, port: port || u.port, path: pathname || u.pathname, method: 'POST', headers }, (res) => {
			let data = '';
			res.on('data', (chunk) => { data += chunk; });
			res.on('end', () => {
				let json = null;
				try { json = data ? JSON.parse(data) : null; } catch (e) { json = null; }
				resolve({ status: res.statusCode, json, raw: data });
			});
		});
		req.on('error', reject);
		req.write(body);
		req.end();
	});
}

async function signIn() {
	const res = await post(`query Authorization { Authorization(input: { login: ${gqlString(lib.suLogin)}, password: ${gqlString(suPassword)}, productId: ${gqlString(lib.productId)} }) { userId token refreshToken } }`);
	const auth = res.json && res.json.data && res.json.data.Authorization;
	assert(auth && auth.token && auth.refreshToken, `sign-in failed: ${res.raw.slice(0, 300)}`);
	return { token: auth.token, refreshToken: auth.refreshToken };
}

async function refresh(refreshToken) {
	const res = await post(`mutation RefreshToken { RefreshToken(input: { refreshToken: ${gqlString(refreshToken)} }) { ok userSession { userId accessToken refreshToken } } }`);
	const payload = res.json && res.json.data && res.json.data.RefreshToken;
	const ok = !!(payload && payload.ok && payload.userSession && payload.userSession.accessToken);
	return { ok, raw: res.raw, token: ok ? payload.userSession.accessToken : null, refreshToken: ok ? payload.userSession.refreshToken : null };
}

async function requestStatus(token) {
	const res = await post(protectedQuery, token);
	if (res.status === 200 && res.json && res.json.errors) { return `200+errors ${JSON.stringify(res.json.errors).slice(0, 160)}`; }
	return res.status;
}

// A new document, made by a fresh session of its own; its notification has to reach the subscription
async function expectDocument(session, subId, kind) {
	const actor = await signIn();
	const mark = session.mark();
	const res = await post(`mutation CreateNewDocument { CreateNewDocument(input: { collectionId: ${gqlString(kind.collectionId)},` +
		` typeId: ${gqlString(kind.typeId)}, proposedSourceDocumentId: ${gqlString(crypto.randomUUID())} }) { documentId } }`, actor.token);
	const info = res.json && res.json.data && res.json.data.CreateNewDocument;
	assert(info && info.documentId, `CreateNewDocument(${kind.collectionId}) failed: ${res.raw.slice(0, 300)}`);
	await session.waitForData(subId, (f) => {
		const n = notificationOf(f, 'OnDocumentManagerChanged');
		return !!n && n.documentId === info.documentId;
	}, mark).catch((e) => { throw new Error(`no ${kind.collectionId} notification: ${e.message}`); });
	await post(`mutation CloseDocument { CloseDocument(input: { collectionId: ${gqlString(kind.collectionId)}, id: ${gqlString(info.documentId)} }) { status } }`, actor.token).catch(() => {});
}

function refusalCode(frame) {
	return frame && Array.isArray(frame.payload) && frame.payload[0] && frame.payload[0].extensions ? frame.payload[0].extensions.code : undefined;
}

async function openSession(label, token) {
	const session = new SubscriptionSession(label, lib.wsUrl, token);
	await session.connect();
	await session.init();
	return session;
}

async function startExpectingRefusal(session, token, kind) {
	session.token = token;
	const id = crypto.randomUUID();
	session.expectErrorIds.add(id);
	session.send(session.startFrame(id, managerQuery(kind.collectionId)));
	return session.settle(id);
}

async function waitUntil(secs) {
	const ms = secs * 1000 - Date.now();
	if (ms > 0) { log(`waiting ${(ms / 1000).toFixed(1)} s`); await delay(ms); }
}

function sessionEndOf(token) {
	return jwtClaim(token, 'exp') - jwtLifetime + sessionLifetime;
}

async function restartPuma() {
	await new Promise((resolve) => { const k = spawn('taskkill', ['/F', '/IM', path.basename(pumaExe)]); k.on('exit', resolve); });
	await delay(2000);
	const output = pumaLog ? fs.openSync(pumaLog, 'a') : 'ignore';
	const child = spawn(pumaExe, [], { cwd: path.dirname(pumaExe), detached: true, stdio: ['ignore', output, output] });
	child.unref();
	for (let i = 0; i < 120; i++) {
		const res = await post('query GetApplicationInfo { GetApplicationInfo { applicationId } }', null, Number(pumaUrl.port), pumaUrl.pathname).catch(() => null);
		if (res && res.status) { return; }
		await delay(500);
	}
	throw new Error('Puma did not come back');
}

// "token": ProLife holds a server token of Puma, Puma answers the authorization of its sessions.
// "notoken": no server token; ProLife checks a session itself and reads with the token of the user.
const mode = argument('mode', 'token');
const isTokenMode = mode === 'token';
// The control check of the test servers is 30 s
const checkWaitMs = 45000;

// Delivery after a reconnect of ProLife to Puma comes back within seconds: a document made before it is lost
async function expectDeliveryRestored(session, subId, kind) {
	const deadline = Date.now() + 30000;
	let lastError = null;
	while (Date.now() < deadline) {
		try { await expectDocument(session, subId, kind); return; } catch (e) { lastError = e; }
	}
	throw lastError;
}

async function expectEnded(session, ids, mark, timeoutMs) {
	return Promise.all(ids.map(id => session.waitFor(f => f.type === 'error' && f.id === id, timeoutMs, mark).catch(() => null)));
}

async function resubscribe(session, ids, token) {
	for (const id of ids) { await session.stop(id).catch(() => null); }
	session.token = token;
	return {
		relay: await session.subscribe('OnDocumentManagerChanged', managerQuery(RELAY.collectionId)),
		local: await session.subscribe('OnDocumentManagerChanged', managerQuery(LOCAL.collectionId))
	};
}

runSuite({ name: `WS token expiry ProLife (${mode})`, className: `WsTokenExpiryProLife_${mode}`, junitFile: `junit-report-ws-token-expiry-${mode}.xml` }, async () => {
	const sessions = [];
	let client = null;
	let main = null;
	let subs = null;
	console.log(`mode: ${mode}`);

	await test('a ProLife session token lives as long as configured at Puma', async () => {
		client = await signIn();
		const life = jwtClaim(client.token, 'exp') - nowSecs();
		assert(life >= jwtLifetime - 2 && life <= jwtLifetime, `the token lives ${life} s`);
		assert(await requestStatus(client.token) === 200, 'a fresh token opens a request');
	});

	await test('forwarded and local subscriptions deliver with a fresh token', async () => {
		main = await openSession('main', client.token);
		sessions.push(main);
		subs = await resubscribe(main, [], client.token);
		await expectDocument(main, subs.relay, RELAY);
		await expectDocument(main, subs.local, LOCAL);
	});

	await test('an expired token is refused at ProLife with 401', async () => {
		await waitUntil(jwtClaim(client.token, 'exp') + 1);
		const status = await requestStatus(client.token);
		assert(status === 401, `expected 401, got ${status}`);
	});

	if (isTokenMode) {
		await test('both established subscriptions outlive the token while the session lives', async () => {
			await expectDocument(main, subs.relay, RELAY);
			await expectDocument(main, subs.local, LOCAL);
		});
	}
	else {
		await test('without a server token nothing confirms an expired token: both subscriptions end as Unauthorized', async () => {
			main.expectError(subs.relay);
			main.expectError(subs.local);
			// The next check comes with a change of the access or after MaxCheckInterval (300 s on ProLife): a change brings it now
			const actor = await signIn();
			await post(`mutation CreateNewDocument { CreateNewDocument(input: { collectionId: "Devices", typeId: "Device", proposedSourceDocumentId: ${gqlString(crypto.randomUUID())} }) { documentId } }`, actor.token);
			const ends = await expectEnded(main, [subs.relay, subs.local], 0, checkWaitMs);
			log(`ended: forwarded ${refusalCode(ends[0])}, local ${refusalCode(ends[1])}`);
			assert(ends[0] && refusalCode(ends[0]) === 'Unauthorized', `forwarded: ${JSON.stringify(ends[0])}`);
			assert(ends[1] && refusalCode(ends[1]) === 'Unauthorized', `local: ${JSON.stringify(ends[1])}`);
		});
	}

	await test('a new forwarded subscription with the expired token is refused as Unauthorized', async () => {
		const frame = await startExpectingRefusal(main, client.token, RELAY);
		assert(frame && refusalCode(frame) === 'Unauthorized', `got ${JSON.stringify(frame)}`);
	});

	await test('a new local subscription with the expired token is refused as Unauthorized', async () => {
		const frame = await startExpectingRefusal(main, client.token, LOCAL);
		assert(frame && refusalCode(frame) === 'Unauthorized', `got ${JSON.stringify(frame)}`);
	});

	let current = null;
	await test('a refresh keeps the session; stop and start move delivery to the new token', async () => {
		current = await refresh(client.refreshToken);
		assert(current.ok, `refresh failed: ${current.raw.slice(0, 300)}`);
		assert(jwtClaim(current.token, 'sessionId') === jwtClaim(client.token, 'sessionId'), 'a new session was opened');
		assert(await requestStatus(current.token) === 200, 'the refreshed token does not open a request');
		subs = await resubscribe(main, [subs.relay, subs.local], current.token);
		await expectDocument(main, subs.relay, RELAY);
		await expectDocument(main, subs.local, LOCAL);
	});

	await test('a Puma restart with an expired token: the forwarded subscription is ended as Unauthorized', async () => {
		await waitUntil(jwtClaim(current.token, 'exp') + 1);
		main.expectError(subs.relay);
		main.expectError(subs.local);
		const mark = isTokenMode ? main.mark() : 0;
		await restartPuma();
		log('Puma restarted');
		const [ended] = await expectEnded(main, [subs.relay], mark, 60000);
		assert(ended, 'the forwarded subscription was neither restored nor ended within 60 s');
		log(`forwarded subscription ended: code ${refusalCode(ended)}: ${JSON.stringify(ended.payload).slice(0, 160)}`);
		assert(refusalCode(ended) === 'Unauthorized', `code ${refusalCode(ended)}`);
	});

	if (isTokenMode) {
		// Puma may refuse the token for a moment right after its start: ProLife checks it again in seconds, not in minutes
		await test('after the restart the server token of ProLife is valid again within seconds', async () => {
			const appInfo = 'query GetApplicationInfo { GetApplicationInfo(input: {}) { dependencies { applicationName status credentialStatus } } }';
			const reader = await signIn();
			const deadline = Date.now() + 20000;
			let puma = null;
			while (Date.now() < deadline) {
				const res = await post(appInfo, reader.token).catch(() => null);
				const info = res && res.json && res.json.data && res.json.data.GetApplicationInfo;
				puma = info && Array.isArray(info.dependencies) ? info.dependencies.find(d => d.applicationName === 'Puma') : null;
				if (puma && puma.status === 'CONNECTED' && puma.credentialStatus === 'VALID') { break; }
				await delay(500);
			}
			log(`Puma as ProLife sees it: ${JSON.stringify(puma)}`);
			assert(puma && puma.status === 'CONNECTED' && puma.credentialStatus === 'VALID', `not valid within 20 s: ${JSON.stringify(puma)}`);
		});
	}

	await test('after the restart a refresh and a new start restore delivery', async () => {
		current = await refresh(current.refreshToken);
		assert(current.ok, `refresh failed: ${current.raw.slice(0, 300)}`);
		subs = await resubscribe(main, [subs.relay, subs.local], current.token);
		await expectDeliveryRestored(main, subs.relay, RELAY);
		await expectDocument(main, subs.local, LOCAL);
	});

	await test('a sign-out at ProLife closes the session at Puma and ends its subscriptions', async () => {
		const leaving = await signIn();
		const session = await openSession('leaving', leaving.token);
		sessions.push(session);
		const own = await resubscribe(session, [], leaving.token);
		session.expectError(own.relay);
		session.expectError(own.local);
		const mark = session.mark();
		const out = await post(`mutation Logout { Logout(input: { accessToken: ${gqlString(leaving.token)} }) { ok } }`, leaving.token);
		assert(out.json && out.json.data && out.json.data.Logout && out.json.data.Logout.ok, `logout failed: ${out.raw.slice(0, 200)}`);
		const loggedOut = Date.now();
		const first = await requestStatus(leaving.token);
		const second = await requestStatus(leaving.token);
		log(`requests with the closed token: ${first}, then ${second}`);
		assert(second === 401, `the token of a closed session still opens a request: ${second}`);
		assert(!(await refresh(leaving.refreshToken)).ok, 'the refresh token of a closed session works');
		const ends = await expectEnded(session, [own.relay, own.local], mark, 70000);
		log(`after the sign-out: forwarded ${ends[0] ? refusalCode(ends[0]) : 'NOT ended'}, local ${ends[1] ? refusalCode(ends[1]) : 'NOT ended'}, ${((Date.now() - loggedOut) / 1000).toFixed(1)} s`);
		assert(ends[0] && ends[1], 'a subscription of a closed session is still served after 70 s');
		session.close();
	});

	await test('the end of the session ends the subscriptions as Unauthorized and the refresh token with it', async () => {
		main.expectError(subs.relay);
		main.expectError(subs.local);
		const end = sessionEndOf(current.token);
		// With a server token they live to the end of the session; without one, to the next check after the token expired
		const ends = await expectEnded(main, [subs.relay, subs.local], 0, (end - nowSecs() + 45) * 1000);
		log(`ended: forwarded ${ends[0] ? refusalCode(ends[0]) : 'NOT ended'}, local ${ends[1] ? refusalCode(ends[1]) : 'NOT ended'}, ${(Date.now() / 1000 - end).toFixed(1)} s after the end of the session`);
		assert(ends[0] && refusalCode(ends[0]) === 'Unauthorized', `forwarded: ${JSON.stringify(ends[0])}`);
		// Without a server token the end of the session is not known here: the local one ends at the next check, up to MaxCheckInterval later
		assert(!isTokenMode || (ends[1] && refusalCode(ends[1]) === 'Unauthorized'), `local: ${JSON.stringify(ends[1])}`);
		await waitUntil(end + 1);
		assert(!(await refresh(current.refreshToken)).ok, 'the refresh token of an ended session works');
	});

	await test('no frame left unaccounted', async () => {
		const violations = sessions.flatMap(s => s.violations);
		assert(violations.length === 0, violations.join('\n'));
	});

	for (const s of sessions) { s.close(); }
});
