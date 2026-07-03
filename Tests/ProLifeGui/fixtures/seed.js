// Self-contained test-user seeding via the ProLife GraphQL endpoint.
//
// global-setup calls seedUsers() once, authenticated as the superuser, to create the roles + users
// declared in fixtures/users.js. The RoleAdd / UserAdd payload shapes and the ';'-delimited
// permissions string are the same ones validated in
//   Tests/ProLifeApiPostman/ProLifeApi.postman_collection.json  (folder "08 Multi-role Scenario").
//
// Idempotent: "already exists" style failures are tolerated so re-runs against a warm DB are fine.

const { seededUsers } = require('./users');

const GQL_PATH = '/ProLife/graphql';

function uuid() {
  return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, (c) => {
    const r = (Math.random() * 16) | 0;
    return (c === 'x' ? r : (r & 0x3) | 0x8).toString(16);
  });
}

/**
 * Minimal GraphQL POST using the browser context's fetch (so it shares cookies/origin with the app).
 * @param {import('@playwright/test').APIRequestContext | import('@playwright/test').BrowserContext} ctx
 */
async function gql(request, baseURL, token, query) {
  const res = await request.post(baseURL + GQL_PATH, {
    headers: {
      'content-type': 'application/json',
      'x-authentication-token': token || '',
    },
    data: JSON.stringify({ query }),
  });
  const json = await res.json().catch(() => ({}));
  return json;
}

/**
 * Authenticate the superuser via GraphQL and return a token.
 * @returns {Promise<string>}
 */
async function authorizeSu(request, baseURL, login, password, productId = 'ProLife') {
  const q = `query Authorization { Authorization(input: { login: "${login}", password: "${password}", productId: "${productId}" }) { token userId } }`;
  const json = await gql(request, baseURL, '', q);
  const auth = json && json.data && json.data.Authorization;
  if (!auth || !auth.token) {
    throw new Error('seed: superuser authorization failed: ' + JSON.stringify(json));
  }
  return auth.token;
}

/**
 * Create the roles + users for every seeded test user. Returns nothing; throws only on unexpected
 * (non "already exists") failures.
 * @param {import('@playwright/test').APIRequestContext} request
 * @param {string} baseURL
 * @param {string} suToken
 */
async function seedUsers(request, baseURL, suToken, productId = 'ProLife') {
  for (const user of seededUsers()) {
    const roleUuid = uuid();
    const permissions = user.permissions.join(';');

    const roleAdd = `mutation RoleAdd { RoleAdd(input: { typeId: "Role", productId: "${productId}", id: "${roleUuid}", item: { id: "${roleUuid}", name: "${user.roleName}", description: "ProLifeGui seeded role", roleId: "${user.roleId}", productId: "${productId}", parentRoles: "", permissions: "${permissions}", isDefault: false, isGuest: false } }) { addedNotification { id } } }`;
    const roleRes = await gql(request, baseURL, suToken, roleAdd);
    const roleId = pickId(roleRes, 'RoleAdd') || roleUuid;
    assertTolerable(roleRes, `RoleAdd(${user.roleName})`);

    const userUuid = uuid();
    const userAdd = `mutation UserAdd { UserAdd(input: { typeId: "User", productId: "${productId}", id: "${userUuid}", item: { id: "${userUuid}", productId: "${productId}", name: "${user.title}", username: "${user.login}", password: "${user.password}", email: "${user.login}@prolifegui.test", groups: [], roles: ["${roleId}"], permissions: "", systemInfos: [] } }) { addedNotification { id } } }`;
    const userRes = await gql(request, baseURL, suToken, userAdd);
    assertTolerable(userRes, `UserAdd(${user.login})`);
  }
}

function pickId(json, field) {
  try {
    return json.data[field].addedNotification.id;
  } catch (_) {
    return null;
  }
}

// Treat "already exists" as success (idempotent re-seed); throw on anything else unexpected.
function assertTolerable(json, label) {
  const errs = (json && json.errors) || [];
  const msg = JSON.stringify(json);
  if (errs.length && !/already exists|duplicate|exists/i.test(msg)) {
    throw new Error(`seed: ${label} failed unexpectedly: ${msg}`);
  }
}

module.exports = { seedUsers, authorizeSu, gql, uuid };
