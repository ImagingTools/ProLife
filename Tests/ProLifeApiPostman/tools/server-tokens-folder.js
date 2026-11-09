// Builds the folder "14 Server tokens" of ProLifeApi.postman_collection.json: run "node tools/server-tokens-folder.js"
// after editing the cases here. The real servers of ProLife, Puma and Lisa hold the tokens Run-CiTests.ps1 issued;
// every expectation was measured on them, refusals pin their reason.
const fs = require('fs');
const path = require('path');

const collectionPath = path.join(__dirname, '..', 'ProLifeApi.postman_collection.json');
const urls = { ProLife: '{{baseUrl}}/graphql', Puma: '{{pumaUrl}}/graphql', Lisa: '{{lisaUrl}}/graphql' };

function item(name, server, query, options = {}){
	const headers = [];
	if (options.auth){
		headers.push({ key: 'x-authentication-token', value: options.auth });
	}
	if (options.server){
		headers.push({ key: 'x-server-token', value: options.server });
	}

	const prerequest = [];
	if (options.product === null){
		// A request of another product: the collection names ProLife on every request otherwise
		prerequest.push("pm.request.headers.remove('productid');");
	}
	else{
		prerequest.push(`pm.request.headers.upsert({ key: 'productid', value: '${options.product || server}' });`);
	}
	if (options.status){
		prerequest.push(`pm.variables.set('expectedHttpStatus', ${options.status});`);
	}

	return {
		name,
		request: {
			// Only the headers of the case: the collection signs every request in as the user of ProLife otherwise
			auth: { type: 'noauth' },
			method: 'POST',
			header: headers,
			body: { mode: 'graphql', graphql: { query, variables: '' } },
			url: urls[server]
		},
		event: [
			{ listen: 'prerequest', script: { type: 'text/javascript', exec: prerequest } },
			{ listen: 'test', script: { type: 'text/javascript', exec: options.test } }
		]
	};
}

function refused(pattern){
	return [
		'const json = pm.response.json();',
		"const messages = (json.errors || []).map(function (e) { return e.message || ''; }).join(' | ');",
		`pm.test('refused: ${pattern.replace(/\\/g, '').replace(/'/g, '')}', function () { pm.expect(messages, JSON.stringify(json)).to.match(${pattern}); });`,
		"pm.test('no data', function () { const data = json.data || {}; Object.keys(data).forEach(function (k) { pm.expect(data[k], k).to.eql(null); }); });"
	];
}

function accepted(command, checks){
	return [
		'const json = pm.response.json();',
		`const payload = json.data && json.data.${command};`,
		`pm.test('${command} answered', function () { pm.expect(json.errors, JSON.stringify(json.errors)).to.eql(undefined); pm.expect(payload, JSON.stringify(json)).to.be.an('object'); });`,
		...(checks || [])
	];
}

const signIn = (product) => `query Authorization { Authorization(input: { login: "{{suLogin}}", password: "{{suPassword}}", productId: "${product}" }) { token userId } }`;
const appInfo = 'query GetApplicationInfo { GetApplicationInfo(input: {}) { applicationId version dependencies { applicationName status credentialStatus version dependencies { applicationName status credentialStatus } } } }';
const issue = (product, name, scopes) => `mutation CreateServerToken { CreateServerToken(input: { clientProductId: "${product}", name: "${name}", scopes: ${scopes} }) { id token success scopes } }`;
const revoke = (id) => `mutation RevokeServerToken { RevokeServerToken(input: { id: "${id}" }) { success } }`;
const permissionIds = 'const ids = []; (payload.groups || []).forEach(function (g) { (g.entries || []).forEach(function (e) { ids.push(e.permissionId); }); });';

const st = [];

st.push(item('ST seed: su of ProLife', 'ProLife', signIn('ProLife'), {
	test: accepted('Authorization', ["pm.environment.set('stProLifeSu', payload.token);"])
}));
st.push(item('ST seed: su of Puma', 'Puma', signIn('Puma'), {
	test: accepted('Authorization', ["pm.environment.set('stPumaSu', payload.token);"])
}));
st.push(item('ST seed: su of Lisa', 'Lisa', signIn('Lisa'), {
	test: accepted('Authorization', ["pm.environment.set('stLisaSu', payload.token);"])
}));

// ProLife and its connections
st.push(item('ST: ProLife reaches Puma and Lisa with valid tokens', 'ProLife', appInfo, {
	test: accepted('GetApplicationInfo', [
		'const byName = {}; (payload.dependencies || []).forEach(function (d) { byName[d.applicationName] = d; });',
		"pm.test('Puma: connected, the token is valid', function () { pm.expect(byName.Puma, JSON.stringify(payload.dependencies)).to.include({ status: 'CONNECTED', credentialStatus: 'VALID' }); pm.expect(byName.Puma.version).to.not.be.empty; });",
		"pm.test('Lisa: connected, the token is valid', function () { pm.expect(byName.Lisa, JSON.stringify(payload.dependencies)).to.include({ status: 'CONNECTED', credentialStatus: 'VALID' }); });",
		"pm.test('Lisa reaches Puma with its own valid token', function () { const puma = (byName.Lisa.dependencies || []).find(function (d) { return d.applicationName === 'Puma'; }); pm.expect(puma, JSON.stringify(byName.Lisa)).to.include({ status: 'CONNECTED', credentialStatus: 'VALID' }); });",
		"pm.environment.set('stProLifeVersion', payload.version);"
	])
}));
st.push(item('ST: the catalog of Puma shows ProLife online with the version it published', 'Puma', 'query ConnectedProductsList { ConnectedProductsList(input: { viewParams: { offset: 0, count: 100 } }) { items { productId status productVersion tokenCreatedAt lastUsedAt } } }', {
	auth: '{{stPumaSu}}',
	test: accepted('ConnectedProductsList', [
		'const byId = {}; (payload.items || []).forEach(function (i) { byId[i.productId] = i; });',
		"pm.test('ProLife is online and published its version', function () { pm.expect(byId.ProLife, JSON.stringify(payload.items)).to.include({ status: 'Online', productVersion: pm.environment.get('stProLifeVersion') }); pm.expect(byId.ProLife.lastUsedAt).to.match(/^\\d{2}\\.\\d{2}\\.\\d{4} \\d{2}:\\d{2}:\\d{2}$/); });",
		"pm.test('Lisa is online', function () { pm.expect(byId.Lisa, JSON.stringify(payload.items)).to.include({ status: 'Online' }); });",
		"pm.test('Puma is the server itself', function () { pm.expect(byId.Puma.status).to.eql('This server'); });"
	])
}));
st.push(item('ST: Puma lists the token ProLife works with', 'Puma', 'query GetServerTokenList { GetServerTokenList(input: { clientProductId: "ProLife" }) { tokens { name clientProductId productId scopes revoked lastUsedAt } } }', {
	auth: '{{stPumaSu}}',
	test: accepted('GetServerTokenList', [
		"const token = (payload.tokens || []).find(function (t) { return t.name === 'ProLife for Puma'; });",
		"pm.test('in use and valid', function () { pm.expect(token, JSON.stringify(payload.tokens)).to.include({ clientProductId: 'ProLife', productId: 'Puma', revoked: false }); pm.expect(token.lastUsedAt).to.not.be.empty; pm.expect(token.scopes).to.include.members(['/ProductServer/SignIn', '/ProductServer/PublishPermissions', '/ProductServer/Roles/Edit']); });"
	])
}));
st.push(item('ST: Puma holds the permissions ProLife published', 'Puma', 'query GetProductPermissions { GetProductPermissions(input: { productId: "ProLife" }) { groups { entries { permissionId } } } }', {
	auth: '{{stPumaSu}}',
	test: accepted('GetProductPermissions', [permissionIds, "pm.test('the tree of ProLife', function () { pm.expect(ids).to.include('/SensorManagement/ViewPage'); });"])
}));
st.push(item('ST: ProLife offers its roles the permissions Puma gives', 'ProLife', 'query GetProductPermissions { GetProductPermissions(input: { productId: "ProLife" }) { groups { entries { permissionId } } } }', {
	auth: '{{stProLifeSu}}',
	test: accepted('GetProductPermissions', [
		permissionIds,
		"pm.test('its own permissions', function () { pm.expect(ids).to.include('/SensorManagement/ViewPage'); });",
		"pm.test('the administration of Puma, fetched with the token', function () { pm.expect(ids.filter(function (id) { return id.indexOf('/Administration/') === 0; })).to.not.be.empty; });"
	])
}));
st.push(item('ST: a user signs in to ProLife through its server', 'ProLife', signIn('ProLife'), {
	test: accepted('Authorization', ["pm.test('signed in', function () { pm.expect(payload.token).to.not.be.empty; pm.expect(payload.userId).to.not.be.empty; });"])
}));
st.push(item('ST: Puma refuses the sign-in to ProLife that skips its server', 'Puma', signIn('ProLife'), { product: 'ProLife', status: 403, test: refused("/The product 'ProLife' is not connected: its server sends no valid token/") }));

// Lisa as a receiver of its own
st.push(item('ST: Lisa offers ProLife its own permissions', 'Lisa', 'query GetServerTokenReceiver { GetServerTokenReceiver(input: { clientProductId: "ProLife" }) { productId requiredPermissions groups { entries { permissionId } } } }', {
	auth: '{{stLisaSu}}',
	test: accepted('GetServerTokenReceiver', [
		permissionIds,
		"pm.test('Lisa receives', function () { pm.expect(payload.productId).to.eql('Lisa'); pm.expect(payload.requiredPermissions || []).to.eql([]); });",
		"pm.test('the permissions of Lisa only', function () { pm.expect(ids).to.include('/FeatureManagement/View'); pm.expect(ids.filter(function (id) { return id.indexOf('/ProductServer/') === 0; })).to.eql([]); });"
	])
}));
st.push(item('ST: Lisa serves only its clients', 'Lisa', 'query GetServerTokenReceiver { GetServerTokenReceiver(input: { clientProductId: "Puma" }) { productId } }', { auth: '{{stLisaSu}}', test: refused("/The product 'Puma' is not a client of this server/") }));
// Lisa keeps its clients in its own catalog, as Puma does: a configurator (Agentino) adds them
const addLisaClient = (product) => `mutation AddServerTokenClient { AddServerTokenClient(input: { clientProductId: "${product}", name: "${product}" }) { success added } }`;
st.push(item('ST: Lisa knows ProLife as its client', 'Lisa', addLisaClient('ProLife'), {
	auth: '{{stLisaSu}}',
	test: accepted('AddServerTokenClient', ["pm.test('a client already', function () { pm.expect(payload.success).to.eql(true); pm.expect(payload.added).to.eql(false); });"])
}));
st.push(item('ST: Lisa adds a new client', 'Lisa', addLisaClient('StLisaClient'), {
	auth: '{{stLisaSu}}',
	test: accepted('AddServerTokenClient', ["pm.test('added now', function () { pm.expect(payload.success).to.eql(true); pm.expect(payload.added).to.eql(true); });"])
}));
st.push(item('ST: the new client of Lisa is offered the permissions of Lisa', 'Lisa', 'query GetServerTokenReceiver { GetServerTokenReceiver(input: { clientProductId: "StLisaClient" }) { productId groups { entries { permissionId } } } }', {
	auth: '{{stLisaSu}}',
	test: accepted('GetServerTokenReceiver', ["pm.test('Lisa receives', function () { pm.expect(payload.productId).to.eql('Lisa'); });"])
}));
st.push(item('ST: Lisa is not its own client', 'Lisa', addLisaClient('Lisa'), { auth: '{{stLisaSu}}', test: refused('/A server token is issued to the server of another product/') }));
st.push(item('ST: Lisa still refuses the product nobody added', 'Lisa', 'query GetServerTokenReceiver { GetServerTokenReceiver(input: { clientProductId: "Puma" }) { productId } }', { auth: '{{stLisaSu}}', test: refused("/The product 'Puma' is not a client of this server/") }));
st.push(item('ST: Lisa lists the token ProLife works with', 'Lisa', 'query GetServerTokenList { GetServerTokenList(input: { clientProductId: "ProLife" }) { tokens { name clientProductId productId revoked lastUsedAt } } }', {
	auth: '{{stLisaSu}}',
	test: accepted('GetServerTokenList', [
		"const token = (payload.tokens || []).find(function (t) { return t.name === 'ProLife for Lisa'; });",
		"pm.test('in use and valid', function () { pm.expect(token, JSON.stringify(payload.tokens)).to.include({ clientProductId: 'ProLife', productId: 'Lisa', revoked: false }); pm.expect(token.lastUsedAt).to.not.be.empty; });"
	])
}));
st.push(item('ST: Lisa does not give the permissions of Puma', 'Lisa', issue('ProLife', 'st foreign permission', '["/ProductServer/Users/View"]'), { auth: '{{stLisaSu}}', test: refused("/The permission '\\/ProductServer\\/Users\\/View' cannot be given to a server token/") }));
st.push(item('ST: Lisa issues a token without permissions', 'Lisa', issue('ProLife', 'st probe', '[]'), {
	auth: '{{stLisaSu}}',
	test: accepted('CreateServerToken', ["pm.test('nothing is required by Lisa', function () { pm.expect(payload.success).to.eql(true); pm.expect(payload.scopes).to.eql([]); });", "pm.environment.set('stLisaPlainId', payload.id);", "pm.environment.set('stLisaPlain', payload.token);"])
}));
st.push(item('ST: Lisa issues a token for the features', 'Lisa', issue('ProLife', 'st features', '["/FeatureManagement/Edit/Add"]'), {
	auth: '{{stLisaSu}}',
	test: accepted('CreateServerToken', ["pm.test('adding brings viewing', function () { pm.expect(payload.scopes).to.have.members(['/FeatureManagement/View', '/FeatureManagement/Edit/Add']); });", "pm.environment.set('stLisaFeaturesId', payload.id);", "pm.environment.set('stLisaFeatures', payload.token);"])
}));
st.push(item('ST: Puma issues a token of ProLife for the cross check', 'Puma', issue('ProLife', 'st probe', '[]'), {
	auth: '{{stPumaSu}}',
	test: accepted('CreateServerToken', ["pm.environment.set('stPumaProbeId', payload.id);", "pm.environment.set('stPumaProbe', payload.token);"])
}));
const features = 'query FeaturesList { FeaturesList(input: { viewParams: { offset: 0, count: 1 } }) { items { id } } }';
st.push(item('ST: Lisa answers the server of ProLife within its token', 'Lisa', features, {
	auth: '{{stLisaFeatures}}', product: null,
	test: accepted('FeaturesList', ["pm.test('the features are read', function () { pm.expect(payload.items.length).to.eql(1); });"])
}));
st.push(item('ST: Lisa refuses a command outside the token', 'Lisa', features, { auth: '{{stLisaPlain}}', product: null, status: 403, test: refused("/The server of 'ProLife' may not send 'FeaturesList': its token lacks '\\/FeatureManagement\\/View'/") }));
st.push(item('ST: Lisa closes the token management to the servers', 'Lisa', 'query GetServerTokenList { GetServerTokenList(input: { clientProductId: "ProLife" }) { tokens { id } } }', { auth: '{{stLisaFeatures}}', product: null, status: 403, test: refused("/The request 'GetServerTokenList' is closed to the servers of products/") }));
st.push(item('ST: the server of ProLife does not add clients at Lisa', 'Lisa', addLisaClient('StBySrv'), { auth: '{{stLisaFeatures}}', product: null, status: 403, test: refused("/The request 'AddServerTokenClient' is closed to the servers of products/") }));
st.push(item('ST: Lisa takes its token under the header of ProLife', 'Lisa', 'query GetApplicationInfo { GetApplicationInfo(input: {}) { applicationId } }', {
	server: '{{stLisaPlain}}', product: 'ProLife',
	test: accepted('GetApplicationInfo', ["pm.test('Lisa answers', function () { pm.expect(payload.applicationId).to.eql('Lisa'); });"])
}));
st.push(item('ST: Lisa refuses a token Puma issued', 'Lisa', 'query GetApplicationInfo { GetApplicationInfo(input: {}) { applicationId } }', { server: '{{stPumaProbe}}', product: null, status: 403, test: refused('/Invalid personal access token/') }));
st.push(item('ST: Puma refuses a token Lisa issued', 'Puma', 'query GetApplicationInfo { GetApplicationInfo(input: {}) { applicationId } }', { server: '{{stLisaPlain}}', product: null, status: 403, test: refused('/Invalid personal access token/') }));
st.push(item('ST: Lisa refuses ProLife without the token of its server', 'Lisa', 'query GetApplicationInfo { GetApplicationInfo(input: {}) { applicationId } }', { product: 'ProLife', status: 403, test: refused("/The product 'ProLife' is not connected: its server sends no valid token/") }));
st.push(item('ST: Lisa revokes its tokens', 'Lisa', revoke('{{stLisaPlainId}}'), {
	auth: '{{stLisaSu}}',
	test: accepted('RevokeServerToken', ["pm.test('revoked', function () { pm.expect(payload.success).to.eql(true); });"])
}));
st.push(item('ST: Lisa refuses the revoked token at once', 'Lisa', 'query GetApplicationInfo { GetApplicationInfo(input: {}) { applicationId } }', { server: '{{stLisaPlain}}', product: null, status: 403, test: refused('/Invalid personal access token/') }));
st.push(item('ST cleanup: Lisa revokes the token for the features', 'Lisa', revoke('{{stLisaFeaturesId}}'), {
	auth: '{{stLisaSu}}',
	test: accepted('RevokeServerToken', ["pm.test('revoked', function () { pm.expect(payload.success).to.eql(true); });"])
}));
st.push(item('ST cleanup: Puma revokes the token of the cross check', 'Puma', revoke('{{stPumaProbeId}}'), {
	auth: '{{stPumaSu}}',
	test: accepted('RevokeServerToken', ["pm.test('revoked', function () { pm.expect(payload.success).to.eql(true); });"])
}));
st.push(item('ST: ProLife still reaches Puma and Lisa', 'ProLife', appInfo, {
	test: accepted('GetApplicationInfo', [
		'const byName = {}; (payload.dependencies || []).forEach(function (d) { byName[d.applicationName] = d; });',
		"pm.test('the tokens of ProLife were not touched', function () { pm.expect(byName.Puma.credentialStatus).to.eql('VALID'); pm.expect(byName.Lisa.credentialStatus).to.eql('VALID'); });"
	])
}));

// ---------------------------------------------------------------- apply

const collection = JSON.parse(fs.readFileSync(collectionPath, 'utf8'));
const folder = {
	name: '14 Server tokens - Connections Between the Product Servers',
	description: 'ProLife works with Puma and Lisa through tokens each of them issued for itself. Run-CiTests.ps1 issues them before ProLife starts.',
	item: st
};
const index = collection.item.findIndex((f) => f.name === folder.name);
if (index >= 0){
	collection.item[index] = folder;
}
else{
	collection.item.splice(collection.item.findIndex((f) => f.name.startsWith('99')), 0, folder);
}

// A request may expect a refusal status
const collectionPre = collection.event.find((e) => e.listen === 'prerequest').script.exec;
if (!collectionPre.some((line) => line.includes('expectedHttpStatus'))){
	collectionPre.unshift('// A request may expect a refusal status by setting this variable in its own pre-request script', "pm.variables.set('expectedHttpStatus', 200);", '');
}
const collectionTest = collection.event.find((e) => e.listen === 'test').script.exec;
const statusIndex = collectionTest.indexOf("pm.test('HTTP status is 200', function () {");
if (statusIndex >= 0){
	collectionTest.splice(statusIndex, 3,
		"const __expectedHttpStatus = Number(pm.variables.get('expectedHttpStatus') || 200);",
		"pm.test('HTTP status is ' + __expectedHttpStatus, function () {",
		'  pm.response.to.have.status(__expectedHttpStatus);',
		'});');
}

fs.writeFileSync(collectionPath, JSON.stringify(collection, null, 2).replace(/\n/g, '\r\n'));
console.log(`14: ${st.length} requests`);
