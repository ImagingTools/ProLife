// Builds the folder "16 Contract Permissions - Four Organizations Exchange Permissions":
// run "node tools/contract-permissions-folder.js" after editing it.
//
// A, B, C and D sign one contract. A is linked with B, C and D, B with C. A gives B the account view, B gives A
// the account view and change, C gives A the account view, B gives C the account view; D gives and receives
// nothing. Every receiver gives what it received to a group of its own. Then a member of each group works in
// the organizations that gave: what it holds there, the accounts it reads and the account it may change are
// checked in its own session; so are the members outside the groups, the time before the conclusion, a taken
// back allocation and a party that leaves.
const fs = require('fs');
const path = require('path');

const collectionPath = path.join(__dirname, '..', 'ProLifeApi.postman_collection.json');
const FOLDER = '16 Contract Permissions - Four Organizations Exchange Permissions';
const ORGS = ['A', 'B', 'C', 'D'];
const VIEW = ['/AccountManagement/ViewPage', '/AccountManagement/View'];
const CHANGE = ['/AccountManagement/Edit/Change/CustomerId', '/AccountManagement/Edit/Change/Description', '/AccountManagement/Edit/Change/Name',
	'/AccountManagement/Edit/Change/Email', '/AccountManagement/Edit/Change/CompanyAddress', '/AccountManagement/Edit/Change/Groups'];
const PASSWORD = 'ShareApi_2026!';

const PRELUDE = [
	'(function () {',
	'  const json = pm.response.json();',
	'  const data = json.data || {};',
	'  const errors = json.errors || [];',
	'  function payload(name) { return data[name]; }',
	'  function refused(name) { const p = data[name]; return errors.length > 0 || !p || !!p.errorMessage || p.success === false || p.ok === false; }',
	'  function ok(name) { const p = data[name]; return errors.length === 0 && !!p && !p.errorMessage && p.success !== false && p.ok !== false; }',
	'  const env = (key) => pm.environment.get(key);',
	"  const letters = { [env('npTA')]: 'A', [env('npTB')]: 'B', [env('npTC')]: 'C', [env('npTD')]: 'D' };",
	"  const accounts = { [env('npAccA')]: 'A', [env('npAccB')]: 'B', [env('npAccC')]: 'C', [env('npAccD')]: 'D' };"
];

const js = (value) => JSON.stringify(value);

function item(name, query, token, tests, prerequest){
	const event = [];
	if (prerequest){
		event.push({ listen: 'prerequest', script: { type: 'text/javascript', exec: prerequest } });
	}
	event.push({ listen: 'test', script: { type: 'text/javascript', exec: PRELUDE.concat(tests.map((t) => '  ' + t), ['})();']) } });

	const request = {
		method: 'POST',
		header: [],
		body: { mode: 'graphql', graphql: { query, variables: '' } },
		url: '{{baseUrl}}/graphql',
		auth: token
			? { type: 'apikey', apikey: [{ key: 'key', value: 'x-authentication-token', type: 'string' }, { key: 'value', value: '{{' + token + '}}', type: 'string' }, { key: 'in', value: 'header', type: 'string' }] }
			: { type: 'noauth' }
	};
	return { name: 'NP: ' + name, event, request, response: [] };
}

const accepted = (command, label) => `pm.test(${js(label)}, () => pm.expect(ok('${command}'), JSON.stringify(json)).to.eql(true));`;
const refusedFor = (command, reason) => [
	`const reason = errors.map((e) => e.message || '').concat(payload('${command}') && payload('${command}').errorMessage ? [payload('${command}').errorMessage] : []).join(' | ');`,
	`pm.test(${js('Refused: ' + reason)}, () => { pm.expect(refused('${command}'), JSON.stringify(json)).to.eql(true); pm.expect(reason).to.match(new RegExp(${js(reason)})); });`
];

// ---------------------------------------------------------------- the organizations

function organization(L){
	return [
		item(`UserAdd owner of ${L}`,
			`mutation UserAdd { UserAdd(input: { typeId: "User", productId: "{{productId}}", id: "{{npO${L}Id}}", item: { id: "{{npO${L}Id}}", productId: "{{productId}}", name: "Contract Owner ${L}", username: "np_o${L.toLowerCase()}_{{npRun}}", password: "${PASSWORD}", email: "np_o${L.toLowerCase()}_{{npRun}}@prolifeapi.test", groups: [], roles: [], permissions: [], systemInfos: [] } }) { id } }`,
			'npSu', [accepted('UserAdd', 'Owner created')], [`pm.environment.set('npO${L}Id', pm.variables.replaceIn('{{$guid}}'));`]),
		item(`CreateTenant ${L}`,
			`mutation CreateTenant { CreateTenant(input: { name: "Contract Org ${L} {{npRun}}", ownerId: "{{npO${L}Id}}" }) { tenantId errorMessage } }`,
			'npSu', [accepted('CreateTenant', 'Organization created'), `pm.environment.set('npT${L}', payload('CreateTenant').tenantId);`]),
		item(`The creator gives ${L} the whole product`,
			`mutation UpdateTenant { UpdateTenant(input: { tenantId: "{{npT${L}}}", name: "Contract Org ${L} {{npRun}}", tenantPermissions: {{npScope}} }) { success errorMessage } }`,
			'npSu', [accepted('UpdateTenant', 'The scope is the whole product')]),
		item(`Auth owner of ${L}`,
			`query Authorization { Authorization(input: { login: "np_o${L.toLowerCase()}_{{npRun}}", password: "${PASSWORD}", productId: "{{productId}}" }) { token userId } }`,
			null, [`pm.test('Authorized', () => pm.expect(payload('Authorization') && payload('Authorization').token, JSON.stringify(json)).to.be.a('string').and.not.empty);`,
				`pm.environment.set('npLogin', payload('Authorization').token);`]),
		item(`SelectTenant ${L} as its owner`,
			`mutation SelectTenant { SelectTenant(input: { tenantId: "{{npT${L}}}" }) { ok errorMessage userSession { accessToken } } }`,
			'npLogin', [accepted('SelectTenant', 'The owner works in its organization'), `pm.environment.set('npO${L}Token', payload('SelectTenant').userSession.accessToken);`]),
		item(`AccountAdd account of ${L}`,
			`mutation AccountAdd { AccountAdd(input: { typeId: "Account", id: "{{npAcc${L}}}", item: { id: "{{npAcc${L}}}", name: "Contract account ${L} {{npRun}}", customerId: "NP-${L}-{{npRun}}", description: "", email: "np${L.toLowerCase()}@prolifeapi.test", companyName: "", country: "", city: "", postalCode: "", street: "", groups: [] } }) { id } }`,
			`npO${L}Token`, [accepted('AccountAdd', 'Account created')], [`pm.environment.set('npAcc${L}', pm.variables.replaceIn('{{$guid}}'));`])
	];
}

function connect(from, to){
	return [
		item(`Connection code of ${to} for ${from}`,
			`query GetConnectionCode { GetConnectionCode(input: { tenantId: "{{npT${to}}}" }) { connectionCode { connectionCode } errorMessage } }`,
			`npO${to}Token`, [accepted('GetConnectionCode', 'Code read'), `pm.environment.set('npCode', payload('GetConnectionCode').connectionCode.connectionCode);`]),
		item(`${from} asks ${to} for a connection`,
			`mutation CreateConnectionRequest { CreateConnectionRequest(input: { sourceTenantId: "{{npT${from}}}", connectionCode: "{{npCode}}", message: "Contract" }) { requestId errorMessage } }`,
			`npO${from}Token`, [accepted('CreateConnectionRequest', 'Asked'), `pm.environment.set('npRequest', payload('CreateConnectionRequest').requestId);`]),
		item(`${to} approves ${from}`,
			`mutation ApproveConnectionRequest { ApproveConnectionRequest(input: { requestId: "{{npRequest}}", tenantId: "{{npT${to}}}" }) { connectionId errorMessage } }`,
			`npO${to}Token`, [accepted('ApproveConnectionRequest', 'Connected')])
	];
}

// ---------------------------------------------------------------- the contract

const COMMAND = '{ success contractId revision errorMessage }';
const link = (a, b, ra, rb) => `{ tenantAId: "{{npT${a}}}", tenantBId: "{{npT${b}}}", roleAId: "${ra}", roleBId: "${rb}" }`;
const draft = `{ name: "Four organizations {{npRun}}", typeId: "Services", parties: [{ tenantId: "{{npTA}}", roleId: "Customer" }, { tenantId: "{{npTB}}", roleId: "ServiceProvider" },`
	+ ` { tenantId: "{{npTC}}", roleId: "ServiceProvider" }, { tenantId: "{{npTD}}", roleId: "ServiceProvider" }],`
	+ ` links: [${link('A', 'B', 'Customer', 'ServiceProvider')}, ${link('A', 'C', 'Customer', 'ServiceProvider')}, ${link('A', 'D', 'Customer', 'ServiceProvider')}, ${link('B', 'C', 'ServiceProvider', 'ServiceProvider')}] }`;

const give = (from, to, permissions) =>
	`mutation GiveContractPermissions { GiveContractPermissions(input: { tenantId: "{{npT${from}}}", contractId: "{{npContract}}", granteeTenantId: "{{npT${to}}}", productId: "{{productId}}", permissionIds: ${js(permissions)} }) ${COMMAND} }`;

function sign(L){
	return item(`${L} signs`,
		`mutation SignContract { SignContract(input: { tenantId: "{{npT${L}}}", contractId: "{{npContract}}", revision: {{npRev}} }) ${COMMAND} }`,
		`npO${L}Token`, [accepted('SignContract', 'Signed')]);
}

// The allocations a party received, by the letter of the giver: npAlloc<giver><receiver>
function receivedAllocations(L){
	return item(`${L} finds the permissions it received`,
		`query GetContract { GetContract(input: { tenantId: "{{npT${L}}}", contractId: "{{npContract}}" }) { contract { allocations { allocationId grantorTenantId granteeTenantId } } errorMessage } }`,
		`npO${L}Token`, [
			"const allocations = ((payload('GetContract') || {}).contract || {}).allocations || [];",
			`allocations.forEach((a) => pm.environment.set('npAlloc' + letters[a.grantorTenantId] + letters[a.granteeTenantId], a.allocationId));`,
			`pm.test('Read', () => pm.expect(ok('GetContract'), JSON.stringify(json)).to.eql(true));`]);
}

function group(L, allocations){
	return [
		item(`Owner of ${L} makes a group of delegates`,
			`mutation GroupAdd { GroupAdd(input: { typeId: "Group", productId: "{{productId}}", id: "{{npG${L}}}", item: { id: "{{npG${L}}}", productId: "{{productId}}", name: "Contract delegates ${L} {{npRun}}", description: "", roles: [], users: [], parentGroups: [] } }) { id } }`,
			`npO${L}Token`, [accepted('GroupAdd', 'Group created')], [`pm.environment.set('npG${L}', pm.variables.replaceIn('{{$guid}}'));`]),
		...allocations.map((key) => item(`${L} gives what ${key[0]} gave it to the group`,
			`mutation SetContractAllocationGroups { SetContractAllocationGroups(input: { tenantId: "{{npT${L}}}", contractId: "{{npContract}}", allocationId: "{{npAlloc${key}}}", groupIds: ["{{npG${L}}}"] }) ${COMMAND} }`,
			`npO${L}Token`, [accepted('SetContractAllocationGroups', 'The group receives the permissions')])),
		item(`UserAdd delegate of ${L} in the group`,
			`mutation UserAdd { UserAdd(input: { typeId: "User", productId: "{{productId}}", id: "{{npD${L}Id}}", item: { id: "{{npD${L}Id}}", productId: "{{productId}}", name: "Contract Delegate ${L}", username: "np_d${L.toLowerCase()}_{{npRun}}", password: "${PASSWORD}", email: "np_d${L.toLowerCase()}_{{npRun}}@prolifeapi.test", groups: [{id: "{{npG${L}}}", direct: true}], roles: [], permissions: [], systemInfos: [] } }) { id } }`,
			'npSu', [accepted('UserAdd', 'Delegate created')], [`pm.environment.set('npD${L}Id', pm.variables.replaceIn('{{$guid}}'));`]),
		item(`The delegate is a member of ${L}`,
			`mutation AddMembership { AddMembership(input: { userId: "{{npD${L}Id}}", tenantId: "{{npT${L}}}" }) { membershipId errorMessage } }`,
			'npSu', [accepted('AddMembership', 'Member')])
	];
}

// ---------------------------------------------------------------- the sessions of the delegates

// A fresh session: SelectTenant ends the session of the token it is called with
function login(user, tokenVar){
	return item(`Auth ${user}`,
		`query Authorization { Authorization(input: { login: "np_${user}_{{npRun}}", password: "${PASSWORD}", productId: "{{productId}}" }) { token userId } }`,
		null, [`pm.test('Authorized', () => pm.expect(payload('Authorization') && payload('Authorization').token, JSON.stringify(json)).to.be.a('string').and.not.empty);`,
			`pm.environment.set('${tokenVar}', payload('Authorization').token);`]);
}

function enter(user, L, sessionVar){
	return [
		login(user, 'npLogin'),
		item(`${user} works in ${L}`,
			`mutation SelectTenant { SelectTenant(input: { tenantId: "{{npT${L}}}" }) { ok errorMessage userSession { accessToken } } }`,
			'npLogin', [accepted('SelectTenant', 'In the organization that gave'), `pm.environment.set('${sessionVar}', payload('SelectTenant').userSession.accessToken);`])
	];
}

function cannotEnter(user, L, label){
	return [
		login(user, 'npLogin'),
		item(`${user} cannot work in ${L}${label ? ' ' + label : ''}`,
			`mutation SelectTenant { SelectTenant(input: { tenantId: "{{npT${L}}}" }) { ok errorMessage userSession { accessToken } } }`,
			'npLogin', refusedFor('SelectTenant', 'not an active member'))
	];
}

function organizationsOf(user, L, expected){
	return item(`The organizations of ${user}`,
		`query GetUserOrganizations { GetUserOrganizations(input: { id: "{{npD${L}Id}}", productId: "{{productId}}" }) { organizations { id isDelegated } } }`,
		'npLogin', [
			"const listed = (((payload('GetUserOrganizations') || {}).organizations) || []).map((o) => (letters[o.id] || o.id) + (o.isDelegated ? '*' : '')).sort();",
			`pm.test(${js('Its own and the delegating ones: ' + expected.join(', '))}, () => pm.expect(listed, JSON.stringify(json)).to.eql(${js(expected.slice().sort())}));`]);
}

function permissionsOf(label, sessionVar, expected){
	return item(`${label}: the account permissions held`,
		`query GetPermissions { GetPermissions(input: { accessToken: "{{${sessionVar}}}" }) { permissions } }`,
		sessionVar, [
			"const held = (((payload('GetPermissions') || {}).permissions) || []).filter((p) => p.startsWith('/AccountManagement')).sort();",
			`pm.test(${js(expected.length ? 'Exactly what was given' : 'Nothing any more')}, () => pm.expect(held, JSON.stringify(json)).to.eql(${js(expected.slice().sort())}));`]);
}

function accountsOf(label, sessionVar, expected){
	return item(`${label}: the accounts read`,
		'query AccountsList { AccountsList(input: { viewParams: { count: 500, offset: 0 } }) { items { id } } }',
		sessionVar, [
			"const read = (((payload('AccountsList') || {}).items) || []).map((i) => accounts[i.id]).filter((L) => !!L).sort();",
			...(expected.length
				? [`pm.test(${js('The accounts of ' + expected.join(', ') + ' only')}, () => { pm.expect(errors, JSON.stringify(json)).to.have.length(0); pm.expect(read).to.eql(${js(expected)}); });`]
				: ["pm.test('No account', () => pm.expect(read, JSON.stringify(json)).to.eql([]));"])]);
}

function update(label, sessionVar, L, isAllowed){
	return item(`${label}: changes the account of ${L}`,
		`mutation AccountUpdate { AccountUpdate(input: { typeId: "Account", id: "{{npAcc${L}}}", item: { id: "{{npAcc${L}}}", name: "Changed by a delegate {{npRun}}", customerId: "NP-${L}-{{npRun}}", description: "", email: "np${L.toLowerCase()}@prolifeapi.test", companyName: "", country: "", city: "", postalCode: "", street: "", groups: [] } }) { id } }`,
		sessionVar, isAllowed ? [accepted('AccountUpdate', 'Changed')] : refusedFor('AccountUpdate', 'Invalid permissions'));
}

// ---------------------------------------------------------------- the folder

const items = [
	item('Auth SU and start the run',
		'query Authorization { Authorization(input: { login: "{{suLogin}}", password: "{{suPassword}}", productId: "{{productId}}" }) { token userId } }',
		null, [`pm.test('SU authorized', () => pm.expect(payload('Authorization') && payload('Authorization').token, JSON.stringify(json)).to.be.a('string').and.not.empty);`,
			"pm.environment.set('npSu', payload('Authorization').token);"],
		["pm.environment.set('npRun', Date.now().toString(36));"]),
	item('The product permissions for the scopes of the organizations',
		'query GetProductPermissions { GetProductPermissions(input: { productId: "{{productId}}" }) { groups { entries { permissionId } } errorMessage } }',
		'npSu', [
			"const ids = []; (((payload('GetProductPermissions') || {}).groups) || []).forEach((g) => (g.entries || []).forEach((e) => ids.push(e.permissionId)));",
			`pm.test('The product declares the account permissions given here', () => ${js(VIEW.concat(CHANGE))}.forEach((id) => pm.expect(ids, JSON.stringify(json)).to.include(id)));`,
			"pm.environment.set('npScope', JSON.stringify(ids));"]),
	...ORGS.flatMap(organization),
	...connect('B', 'A'), ...connect('C', 'A'), ...connect('D', 'A'), ...connect('C', 'B'),

	item('A drafts the contract of the four',
		`mutation CreateContract { CreateContract(input: { tenantId: "{{npTA}}", draft: ${draft} }) ${COMMAND} }`,
		'npOAToken', [accepted('CreateContract', 'Drafted'), "pm.environment.set('npContract', payload('CreateContract').contractId);", "pm.environment.set('npRev', payload('CreateContract').revision);"]),
	item('A sends it',
		`mutation SubmitContract { SubmitContract(input: { tenantId: "{{npTA}}", contractId: "{{npContract}}", revision: {{npRev}} }) ${COMMAND} }`,
		'npOAToken', [accepted('SubmitContract', 'Sent')]),

	// What the parties give each other
	item('A gives B the account view', give('A', 'B', VIEW), 'npOAToken', [accepted('GiveContractPermissions', 'Given')]),
	item('B gives A the account view and change', give('B', 'A', VIEW.concat(CHANGE)), 'npOBToken', [accepted('GiveContractPermissions', 'Given')]),
	item('C gives A the account view', give('C', 'A', VIEW), 'npOCToken', [accepted('GiveContractPermissions', 'Given')]),
	item('B gives C the account view', give('B', 'C', VIEW), 'npOBToken', [accepted('GiveContractPermissions', 'Given')]),
	item('D cannot give B anything: they are not linked', give('D', 'B', VIEW), 'npODToken', refusedFor('GiveContractPermissions', 'linked with')),
	item('A cannot give what it does not hold', give('A', 'B', ['/No/Such/Permission']), 'npOAToken', refusedFor('GiveContractPermissions', 'not available to your organization')),
	item('A cannot give in the name of B', give('B', 'A', VIEW), 'npOAToken', refusedFor('GiveContractPermissions', 'Access denied')),

	// Every receiver distributes what it received
	...['A', 'B', 'C', 'D'].map(receivedAllocations),
	item('A cannot choose the groups of B', `mutation SetContractAllocationGroups { SetContractAllocationGroups(input: { tenantId: "{{npTA}}", contractId: "{{npContract}}", allocationId: "{{npAllocAB}}", groupIds: [] }) ${COMMAND} }`,
		'npOAToken', refusedFor('SetContractAllocationGroups', 'only the permissions it receives')),
	...group('A', ['BA', 'CA']), ...group('B', ['AB']), ...group('C', ['BC']), ...group('D', []),
	item('UserAdd member of B outside the group',
		'mutation UserAdd { UserAdd(input: { typeId: "User", productId: "{{productId}}", id: "{{npXBId}}", item: { id: "{{npXBId}}", productId: "{{productId}}", name: "Contract Member B", username: "np_xb_{{npRun}}", password: "' + PASSWORD + '", email: "np_xb_{{npRun}}@prolifeapi.test", groups: [], roles: [], permissions: [], systemInfos: [] } }) { id } }',
		'npSu', [accepted('UserAdd', 'Member created')], ["pm.environment.set('npXBId', pm.variables.replaceIn('{{$guid}}'));"]),
	item('The member outside the group is a member of B',
		'mutation AddMembership { AddMembership(input: { userId: "{{npXBId}}", tenantId: "{{npTB}}" }) { membershipId errorMessage } }',
		'npSu', [accepted('AddMembership', 'Member')]),

	// Given but not in force before everybody signed
	...cannotEnter('db', 'A', 'before the contract is concluded'),
	...['B', 'C', 'D', 'A'].map(sign),
	item('The contract is in force',
		'query GetContract { GetContract(input: { tenantId: "{{npTA}}", contractId: "{{npContract}}" }) { contract { state } errorMessage } }',
		'npOAToken', ["pm.test('Active', () => pm.expect(((payload('GetContract') || {}).contract || {}).state, JSON.stringify(json)).to.eql('Active'));"]),

	// The delegate of B works in A with the account view
	login('db', 'npLogin'),
	organizationsOf('db', 'B', ['B', 'A*']),
	...enter('db', 'A', 'npDBinA'),
	permissionsOf('db in A', 'npDBinA', VIEW),
	accountsOf('db in A', 'npDBinA', ['A']),
	update('db in A', 'npDBinA', 'A', false),

	// The delegate of A works in B with the view and the change, and in C with the view
	login('da', 'npLogin'),
	organizationsOf('da', 'A', ['A', 'B*', 'C*']),
	...enter('da', 'B', 'npDAinB'),
	permissionsOf('da in B', 'npDAinB', VIEW.concat(CHANGE)),
	accountsOf('da in B', 'npDAinB', ['B']),
	update('da in B', 'npDAinB', 'B', true),
	item('The owner of B reads the change of the delegate of A',
		'query AccountItem { AccountItem(input: { typeId: "Account", id: "{{npAccB}}" }) { id name } }',
		'npOBToken', ["pm.test('Changed', () => pm.expect((payload('AccountItem') || {}).name, JSON.stringify(json)).to.eql('Changed by a delegate ' + env('npRun')));"]),
	...enter('da', 'C', 'npDAinC'),
	permissionsOf('da in C', 'npDAinC', VIEW),
	accountsOf('da in C', 'npDAinC', ['C']),
	update('da in C', 'npDAinC', 'C', false),

	// The delegate of C works in B; nobody gave C anything in A
	login('dc', 'npLogin'),
	organizationsOf('dc', 'C', ['B*', 'C']),
	...enter('dc', 'B', 'npDCinB'),
	permissionsOf('dc in B', 'npDCinB', VIEW),
	accountsOf('dc in B', 'npDCinB', ['B']),
	...cannotEnter('dc', 'A'),

	// D gave and received nothing; a member of B outside the group received nothing
	login('dd', 'npLogin'),
	organizationsOf('dd', 'D', ['D']),
	...cannotEnter('dd', 'A'),
	...cannotEnter('dd', 'B'),
	...cannotEnter('xb', 'A', 'outside the group'),

	// A takes back what it gave B: the open session of the delegate of B loses it at once, also right after a read
	accountsOf('db in A right before the take back', 'npDBinA', ['A']),
	item('A takes back what it gave B',
		`mutation WithdrawContractPermissions { WithdrawContractPermissions(input: { tenantId: "{{npTA}}", contractId: "{{npContract}}", allocationId: "{{npAllocAB}}" }) ${COMMAND} }`,
		'npOAToken', [accepted('WithdrawContractPermissions', 'Taken back')]),
	permissionsOf('db in A after the take back', 'npDBinA', []),
	accountsOf('db in A after the take back', 'npDBinA', []),
	...cannotEnter('db', 'A', 'after the take back'),
	permissionsOf('da in B is not touched', 'npDAinB', VIEW.concat(CHANGE)),

	// C leaves at once: what it gave and what it received end, the contract of A, B and D goes on
	accountsOf('da in C right before C leaves', 'npDAinC', ['C']),
	item('C leaves the contract at once',
		'mutation GiveContractNotice { GiveContractNotice(input: { tenantId: "{{npTC}}", contractId: "{{npContract}}", reason: "The test ends the participation", isImmediate: true }) ' + COMMAND + ' }',
		'npOCToken', [accepted('GiveContractNotice', 'C left')]),
	permissionsOf('da in C after C left', 'npDAinC', []),
	accountsOf('da in C after C left', 'npDAinC', []),
	...cannotEnter('da', 'C', 'after C left'),
	permissionsOf('dc in B after C left', 'npDCinB', []),
	...cannotEnter('dc', 'B', 'after C left'),
	permissionsOf('da in B after C left', 'npDAinB', VIEW.concat(CHANGE)),
	accountsOf('da in B after C left', 'npDAinB', ['B']),
	item('The contract of A, B and D is still in force',
		'query GetContract { GetContract(input: { tenantId: "{{npTA}}", contractId: "{{npContract}}" }) { contract { state } errorMessage } }',
		'npOAToken', ["pm.test('Active', () => pm.expect(((payload('GetContract') || {}).contract || {}).state, JSON.stringify(json)).to.eql('Active'));"])
];

const raw = fs.readFileSync(collectionPath, 'utf8');
const collection = JSON.parse(raw);
const folder = {
	name: FOLDER,
	description: 'Four organizations exchange permissions by one contract; the members of the receiving groups work with exactly what was given, in their own sessions, and lose it when it is taken back or a party leaves. Generated by tools/contract-permissions-folder.js.',
	item: items
};
const index = collection.item.findIndex((i) => i.name === FOLDER);
if (index >= 0){
	collection.item[index] = folder;
}
else{
	const cleanupIndex = collection.item.findIndex((i) => i.name.startsWith('99 '));
	collection.item.splice(cleanupIndex >= 0 ? cleanupIndex : collection.item.length, 0, folder);
}

const eol = raw.includes('\r\n') ? '\r\n' : '\n';
const indent = raw.match(/^\{\r?\n([ \t]+)/)[1];
fs.writeFileSync(collectionPath, (JSON.stringify(collection, null, indent === '\t' ? '\t' : indent.length) + (raw.endsWith('\n') ? '\n' : '')).replace(/\r?\n/g, eol));
console.log(FOLDER + ': ' + items.length + ' requests');
