// Single source of truth for the GUI test users.
//
// Consumed by:
//   - playwright.config.js  -> one Playwright *project* per user (each with its own storageState)
//   - global-setup.js       -> creates the roles/users via GraphQL, then UI-logs-in each one
//   - fixtures/test.js       -> exposes the current user (resolved from the project name) to tests
//
// Permission codes are the exact FeatureId strings from
//   Impl/ProLifeServer/ProLifeFeatures.xml
// and match the page gating declared in
//   Partitura/ProLifeQmlVoce.arp/Pages.acc  (each page's PagePermissions list).
// This mirrors the role/permission model already validated in
//   Tests/ProLifeApiPostman/ProLifeApi.postman_collection.json  (folder "08 Multi-role Scenario").
//
// permissions is stored server-side as a ';'-delimited string; here we keep an array and join on seed.

const PASSWORD = 'ProLifeGui_2026!';

/**
 * @typedef {Object} TestUser
 * @property {string} key            Stable id -> Playwright project name + .auth/<key>.json
 * @property {string} title          Human label
 * @property {string} login          Username used at the login screen
 * @property {string} password       Password used at the login screen
 * @property {boolean} seed          If true, global-setup creates this user via GraphQL
 * @property {string} [roleName]     Role name created for this user (seeded users only)
 * @property {string} [roleId]       Stable role code (seeded users only)
 * @property {string[]} permissions  Granted permission codes ('*' => superuser, everything)
 */

/** @type {TestUser[]} */
const USERS = [
  {
    key: 'su',
    title: 'Superuser',
    login: 'su',
    password: '1', // pre-existing superuser from the DB backup / CreateSuperuser
    seed: false,
    permissions: ['*'],
  },
  {
    key: 'fullAccess',
    title: 'Full Access',
    login: 'prolifegui_full',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Full Access',
    roleId: 'ProLifeGuiFullAccess',
    permissions: [
      'ViewWorkspace',
      'ViewAccounts',
      'ViewSensors',
      'ViewOrders',
      'ViewLicenses',
      'ViewUsers',
      'ViewRoles',
      'ViewGroups',
      'AddSensor',
      'AddOrder',
      'AddAccount',
    ],
  },
  {
    key: 'accountsViewer',
    title: 'Accounts Viewer',
    login: 'prolifegui_accviewer',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Accounts Viewer',
    roleId: 'ProLifeGuiAccountsViewer',
    permissions: ['ViewAccounts'],
  },
  {
    key: 'noAccess',
    title: 'No Access',
    login: 'prolifegui_noaccess',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui No Access',
    roleId: 'ProLifeGuiNoAccess',
    permissions: [],
  },
];

// The guest (unauthenticated) pseudo-user. Not seeded, no storageState.
const GUEST = { key: 'guest', title: 'Guest', login: null, password: null, seed: false, permissions: [] };

const byKey = (key) => USERS.find((u) => u.key === key) || (key === GUEST.key ? GUEST : undefined);

/**
 * Does this user hold a given permission code? '*' users hold everything.
 * @param {TestUser} user
 * @param {string} permission
 */
const can = (user, permission) => !!user && (user.permissions.includes('*') || user.permissions.includes(permission));

module.exports = {
  USERS,
  GUEST,
  PASSWORD,
  byKey,
  can,
  authFile: (key) => `.auth/${key}.json`,
  seededUsers: () => USERS.filter((u) => u.seed),
};
