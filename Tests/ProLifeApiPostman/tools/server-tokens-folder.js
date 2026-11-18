// Builds the folder "14 Server tokens" of ProLifeApi.postman_collection.json: run "node tools/server-tokens-folder.js"
// after editing the cases here. The real servers of ProLife and Puma hold the tokens Run-CiTests.ps1 issued;
// Refusals pin the handler's reason; live validation is performed by the collection runner.
const fs = require('fs');
const path = require('path');

const collectionPath = path.join(__dirname, '..', 'ProLifeApi.postman_collection.json');
const urls = { ProLife: '{{baseUrl}}/graphql', Puma: '{{pumaUrl}}/graphql' };

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

// ProLife and its connections
st.push(item('ST: ProLife reaches Puma with a valid token', 'ProLife', appInfo, {
	test: accepted('GetApplicationInfo', [
		'const byName = {}; (payload.dependencies || []).forEach(function (d) { byName[d.applicationName] = d; });',
		"pm.test('Puma: connected, the token is valid', function () { pm.expect(byName.Puma, JSON.stringify(payload.dependencies)).to.include({ status: 'CONNECTED', credentialStatus: 'VALID' }); pm.expect(byName.Puma.version).to.not.be.empty; });",
		"pm.test('ProLife no longer depends on Lisa', function () { pm.expect(byName.Lisa, JSON.stringify(payload.dependencies)).to.eql(undefined); });",
		"pm.environment.set('stProLifeVersion', payload.version);"
	])
}));
st.push(item('ST: the catalog of Puma shows ProLife online with the version it published', 'Puma', 'query ConnectedProductsList { ConnectedProductsList(input: { viewParams: { offset: 0, count: 100 } }) { items { productId status productVersion tokenCreatedAt lastUsedAt } } }', {
	auth: '{{stPumaSu}}',
	test: accepted('ConnectedProductsList', [
		'const byId = {}; (payload.items || []).forEach(function (i) { byId[i.productId] = i; });',
		"pm.test('ProLife is online and published its version', function () { pm.expect(byId.ProLife, JSON.stringify(payload.items)).to.include({ status: 'Online', productVersion: pm.environment.get('stProLifeVersion') }); pm.expect(byId.ProLife.lastUsedAt).to.match(/^\\d{2}\\.\\d{2}\\.\\d{4} \\d{2}:\\d{2}:\\d{2}$/); });",
		"pm.test('Puma is the server itself', function () { pm.expect(byId.Puma.status).to.eql('This server'); });"
	])
}));
st.push(item('ST: Puma lists the token ProLife works with', 'Puma', 'query GetServerTokenList { GetServerTokenList(input: { clientProductId: "ProLife" }) { tokens { name clientProductId productId scopes revoked lastUsedAt } } }', {
	auth: '{{stPumaSu}}',
	test: accepted('GetServerTokenList', [
		"const token = (payload.tokens || []).find(function (t) { return t.name === 'ProLife for Puma'; });",
		"pm.test('in use and valid', function () {",
		"  pm.expect(token, JSON.stringify(payload.tokens)).to.include({ clientProductId: 'ProLife', productId: 'Puma', revoked: false });",
		"  pm.expect(token.lastUsedAt).to.not.be.empty;",
		"});",
		"// The runner issues it with every permission Puma offers ProLife: the token holds exactly that catalog",
		"pm.sendRequest({",
		"  url: pm.environment.get('pumaUrl') + '/graphql',",
		"  method: 'POST',",
		"  header: { 'Content-Type': 'application/json', productid: 'Puma', 'x-authentication-token': pm.environment.get('stPumaSu') },",
		"  body: { mode: 'raw', raw: JSON.stringify({ query: 'query GetServerTokenReceiver { GetServerTokenReceiver(input: { clientProductId: \"ProLife\" }) { groups { entries { permissionId } } } }' }) }",
		"}, function (error, response) {",
		"  pm.test('holds the catalog of Puma', function () {",
		"    pm.expect(error, String(error)).to.eql(null);",
		"    const receiver = response.json().data && response.json().data.GetServerTokenReceiver;",
		"    const catalog = [];",
		"    ((receiver && receiver.groups) || []).forEach(function (g) { (g.entries || []).forEach(function (e) { catalog.push(e.permissionId); }); });",
		"    pm.expect(catalog, JSON.stringify(response.json())).to.not.be.empty;",
		"    pm.expect(token.scopes).to.have.members(catalog);",
		"  });",
		"});"
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
st.push(item('ST: Puma refuses the sign-in to ProLife that skips its server', 'Puma', signIn('ProLife'), { product: 'ProLife', status: 403, test: refused("/ProLife is not connected to Puma: its server has no valid server token/") }));

st.push(item('ST: Puma issues a token of ProLife for the cross check', 'Puma', issue('ProLife', 'st probe', '[]'), {
	auth: '{{stPumaSu}}',
	test: accepted('CreateServerToken', ["pm.environment.set('stPumaProbeId', payload.id);", "pm.environment.set('stPumaProbe', payload.token);"])
}));
st.push(item('ST: Puma issues a dedicated token without Users/View', 'Puma', issue('ProLife', 'st users denied', '[]'), {
	auth: '{{stPumaSu}}',
	test: accepted('CreateServerToken', [
		"pm.test('issued without Users/View', function () { pm.expect(payload.success).to.eql(true); pm.expect(payload.id).to.be.a('string').and.not.empty; pm.expect(payload.token).to.be.a('string').and.not.empty; pm.expect(payload.scopes).to.be.an('array').that.does.not.include('/Administration/Users/View'); });",
		"pm.environment.set('stPumaNarrowId', payload.id);",
		"pm.environment.set('stPumaNarrow', payload.token);",
		"pm.environment.set('stPumaNarrowScopes', JSON.stringify(payload.scopes));"
	])
}));
st.push(item('ST: Puma issues a paired token with Users/View', 'Puma', issue('ProLife', 'st users allowed', '["/Administration/Users/View"]'), {
	auth: '{{stPumaSu}}',
	test: accepted('CreateServerToken', [
		"pm.test('only Users/View is added to the narrow token scopes', function () { pm.expect(payload.success).to.eql(true); pm.expect(payload.id).to.be.a('string').and.not.empty; pm.expect(payload.token).to.be.a('string').and.not.empty; pm.expect(payload.scopes).to.have.members(JSON.parse(pm.environment.get('stPumaNarrowScopes')).concat(['/Administration/Users/View'])); });",
		"pm.environment.set('stPumaUsersId', payload.id);",
		"pm.environment.set('stPumaUsers', payload.token);"
	])
}));
const users = 'query UsersList { UsersList(input: { viewParams: { offset: 0, count: 1 } }) { items { id } } }';
st.push(item('ST: Puma refuses UsersList outside the dedicated token', 'Puma', users, {
	auth: '{{stPumaNarrow}}', product: null,
	test: refused("/Invalid permissions for the user 'ProLife'/")
}));
st.push(item('ST: Puma answers the same UsersList with Users/View', 'Puma', users, {
	auth: '{{stPumaUsers}}', product: null,
	test: accepted('UsersList', ["pm.test('the same ProLife server principal can read users', function () { pm.expect(payload.items).to.be.an('array').with.length(1); });"])
}));
// Revoke only scenario tokens, in reverse creation order; never change the working connections.
for (const [server, key] of [['Puma', 'stPumaUsers'], ['Puma', 'stPumaNarrow'], ['Puma', 'stPumaProbe']]){
	st.push(item(`ST cleanup: ${server} revokes ${key}`, server, revoke(`{{${key}Id}}`), {
		auth: `{{st${server}Su}}`,
		test: accepted('RevokeServerToken', ["pm.test('revoked', function () { pm.expect(payload.success).to.eql(true); });"])
	}));
}
st.push(item('ST: Puma refuses the revoked Users/View token at once', 'Puma', users, {
	auth: '{{stPumaUsers}}', product: null, status: 403, test: refused('/Invalid personal access token/')
}));
for (const [server, keys] of [['Puma', ['stPumaUsers', 'stPumaNarrow', 'stPumaProbe']]]){
	st.push(item(`ST cleanup: ${server} confirms every scenario token is revoked`, server, 'query GetServerTokenList { GetServerTokenList(input: { clientProductId: "ProLife" }) { tokens { id revoked } } }', {
		auth: `{{st${server}Su}}`,
		test: accepted('GetServerTokenList', [
			`const keys = ${JSON.stringify(keys)};`,
			"pm.test('all scenario tokens are revoked', function () { keys.forEach(function (key) { const token = (payload.tokens || []).find(function (t) { return t.id === pm.environment.get(key + 'Id'); }); pm.expect(token, key).to.be.an('object'); pm.expect(token.revoked, key).to.eql(true); }); });"
		])
	}));
}
st.push(item('ST: ProLife still reaches Puma', 'ProLife', appInfo, {
	test: accepted('GetApplicationInfo', [
		'const byName = {}; (payload.dependencies || []).forEach(function (d) { byName[d.applicationName] = d; });',
		"pm.test('the tokens of ProLife were not touched', function () { pm.expect(byName.Puma.credentialStatus).to.eql('VALID'); });"
	])
}));

// ---------------------------------------------------------------- apply

const collection = JSON.parse(fs.readFileSync(collectionPath, 'utf8'));
const folder = {
	name: '14 Server tokens - Connections Between the Product Servers',
	description: 'ProLife works with Puma through a token Puma issued for it. Run-CiTests.ps1 issues it before ProLife starts.',
	item: st
};
const index = collection.item.findIndex((f) => f.name === folder.name);
if (index >= 0){
	collection.item[index] = folder;
}
else{
	collection.item.splice(collection.item.findIndex((f) => f.name.startsWith('99')), 0, folder);
}

fs.writeFileSync(collectionPath, JSON.stringify(collection, null, 2).replace(/\n/g, '\r\n'));
console.log(`14: ${st.length} requests`);
