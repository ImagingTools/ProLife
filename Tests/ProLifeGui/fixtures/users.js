// Single source of truth for the GUI test users.
//
// Consumed by:
//   - playwright.config.js  -> one Playwright *project* per user (each with its own storageState)
//   - global-setup.js       -> creates the roles/users via GraphQL, then UI-logs-in each one
//   - fixtures/test.js       -> exposes the current user (resolved from the project name) to tests
//
// Permission codes are the exact FeatureId strings from
//   Impl/ProLifeServer/ProLifeFeatures.xml
// and match the page/command/field gating declared in
//   Partitura/ProLifeQmlVoce.arp/Pages.acc + *Page.acc      (per-page PagePermissions / CommandPermissions)
//   Partitura/ImtGraphQlVoce.arp/PagesController.acc         (Workspace / Search / Administration)
// A few command-permission ids used below (SplitLicense, RevokeLicense) are NOT in ProLifeFeatures.xml
// but ARE real CommandPermissions declared in SoftwareProductsPage.acc, so a role can still be granted
// the raw string; they gate the Split/Revoke command visibility. This mirrors the role/permission model
// already validated in Tests/ProLifeApiPostman ("08 Multi-role Scenario").
//
// permissions is stored server-side as a ';'-delimited string; here we keep an array and join on seed.

const PASSWORD = 'ProLifeGui_2026!';

// --- reusable permission bundles (kept close to ProLifeFeatures.xml groupings) -------------------

const SENSOR_FULL = [
  'ViewSensors', 'ViewAllSensors', 'ViewSensorHistory',
  'AddSensor', 'RemoveSensor', 'EditSensor', 'ChangeSensor',
  'ChangeOrderForSensor', 'ChangeProductionStatus', 'ChangeHardwareConfiguration',
  'ChangeDeviceType', 'ChangeDescriptionForSensor', 'ChangeSerialNumberForSensor',
  'ChangeMacAddress', 'ChangeProjectForSensor',
  'BindSensor', 'UnbindSensor', 'CreateLicenseFile', 'TransferLicenses', 'ResetTransferCounter',
];

const ORDER_FULL = [
  'ViewOrders', 'ViewAllOrders', 'ViewOrderHistory',
  'AddOrder', 'RemoveOrder', 'EditOrder', 'ChangeOrder',
  'ChangeOrderProducts', 'ChangeOrderStatus', 'ChangeCustomer',
  'ChangeDescriptionForOrder', 'ChangePurchaseOrderId', 'ChangeDeliveryId',
];

const LICENSE_FULL = [
  'ViewLicenses', 'ViewAllLicenses', 'ViewLicenseHistory',
  'AddLicense', 'RemoveLicense', 'EditLicense', 'ChangeLicense',
  'ChangeLicenseNumber', 'ChangeExpiration', 'ChangeProductLicenses',
  'ChangeProductForLicense', 'ChangeOrderForLicense', 'ChangeProjectForLicense',
  'SplitLicense', 'RevokeLicense', 'CreateLicenseFile', // command-permission ids (see header)
];

const ACCOUNT_FULL = [
  'ViewAccounts', 'ViewAllAccounts', 'ViewAccountHistory',
  'AddAccount', 'RemoveAccount', 'EditAccount', 'ChangeAccount',
  'ChangeAccountGroups', 'ChangeCompanyAddress', 'ChangeAccountEmail',
  'ChangeAccountDescription', 'ChangeAccountName', 'ChangeCustomerId',
];

const ADMIN_FULL = [
  'ViewUsers', 'ViewRoles', 'ViewGroups',
  'ViewUserHistory', 'ViewRoleHistory', 'ViewGroupHistory',
  'ChangeUser', 'EditUser', 'AddUser', 'RemoveUser',
  'ChangeRole', 'EditRole', 'AddRole', 'RemoveRole',
  'ChangeGroup', 'EditGroup', 'AddGroup', 'RemoveGroup',
];

const WORKSPACE_FULL = ['ViewWorkspace', 'ViewUserActions', 'ViewAllUserActions', 'ViewAnalytics'];

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
    password: '1', // pre-existing superuser (bootstrapped via CreateSuperuser by Run-CiTests.ps1)
    seed: false,
    permissions: ['*'],
  },
  {
    // Genuine "everything for the ProLife domain" power user: sees every page and can edit every
    // field (so the *editor* "edit and save" paths actually run for a non-superuser too).
    key: 'fullAccess',
    title: 'Full Access',
    login: 'prolifegui_full',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Full Access',
    roleId: 'ProLifeGuiFullAccess',
    permissions: [
      ...WORKSPACE_FULL,
      ...ACCOUNT_FULL,
      ...SENSOR_FULL,
      ...ORDER_FULL,
      ...LICENSE_FULL,
      'ViewOrganizations',
      ...ADMIN_FULL,
    ],
  },
  {
    // Hardware specialist: full sensor rights (every command bar button + editable fields + revision
    // + bind/transfer/reset/license-file) plus ViewAccounts so customer filters / order combos work.
    // Exercises the whole Devices command bar and the device editor save path without being '*'.
    key: 'hardwareManager',
    title: 'Hardware Manager',
    login: 'prolifegui_hw',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Hardware Manager',
    roleId: 'ProLifeGuiHardwareManager',
    permissions: [...SENSOR_FULL, 'ViewAccounts', 'ViewRevisions'],
  },
  {
    // License specialist: full license rights incl. Split/Revoke command permissions, so the Software
    // Split/Revoke dialogs and the software editor save path run for a non-superuser.
    key: 'licenseManager',
    title: 'License Manager',
    login: 'prolifegui_lic',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui License Manager',
    roleId: 'ProLifeGuiLicenseManager',
    permissions: [...LICENSE_FULL, 'ViewAccounts', 'ViewRevisions'],
  },
  {
    // Order specialist: full order rights + revision, plus ViewAccounts for the customer filter/combo.
    key: 'orderManager',
    title: 'Order Manager',
    login: 'prolifegui_ord',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Order Manager',
    roleId: 'ProLifeGuiOrderManager',
    permissions: [...ORDER_FULL, 'ViewAccounts', 'ViewRevisions'],
  },
  // --- Editor-isolation users --------------------------------------------------------------------
  //
  // The Collection Document Service keys a user's OPEN-DOCUMENTS workspace by userId ONLY (server-side;
  // see CCollectionDocumentServiceControllerComp.cpp - GetOpenedDocumentList(userId), and the
  // On<Collection>DocumentChanged subscription that fans every open/close out to ALL of that user's
  // live sessions). So two DIFFERENT editor spec files driving document tabs as the SAME user at the
  // same time (Playwright runs different files on different workers in parallel) corrupt one shared
  // workspace: one file's navigation/closeAllDocumentTabs closes a tab another file is mid-test on, and
  // the victim's fields vanish from the DOM ("DescriptionInput ... no element with this objectName
  // path exists"). Intra-file blocks don't collide (fullyParallel is off -> one file = one worker,
  // sequential), so the fix is purely: give each document-editor spec its OWN dedicated user, so no two
  // of them ever share a server workspace. `isolatedSpec` pins the user to exactly one spec file (see
  // buildProjects.js): that user runs ONLY that file, and every OTHER project testIgnores it. Each
  // carries the full domain edit rights its editor needs to exercise create/edit/save (a superset of
  // the matching domain-manager role) - the per-user permission MATRIX for these editors is
  // deliberately traded away for determinism (collection specs still cover per-user gating).
  {
    key: 'devEditor',
    title: 'Device Editor (isolated)',
    login: 'prolifegui_deveditor',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Device Editor',
    roleId: 'ProLifeGuiDeviceEditor',
    permissions: [...SENSOR_FULL, 'ViewAccounts', 'ViewRevisions'],
    isolatedSpec: 'devices.editor.multiuser.test.js',
  },
  // ordEditor (the isolated Order Editor fixture user) used to own orders.editor.multiuser.test.js
  // exclusively via isolatedSpec, like its siblings below. By explicit request that spec now runs as
  // `su` (and `fullAccess`) instead, so it's covered by the default `su` validation pass rather than
  // needing a separate --project=ordEditor invocation - accepting the cross-file document-tab-
  // collision risk described in the block comment above, on the basis that no OTHER spec currently
  // opens a document tab as `su`/`fullAccess` in the same phase (su/fullAccess run collection-level
  // specs only, plus Support's own tab-opening tests are all @mutating/phase-2-serial, so there's no
  // actual concurrent pair right now). Revisit this if a future spec starts opening document tabs
  // under su/fullAccess too - see the isolation rationale above for what breaks.

  {
    key: 'accEditor',
    title: 'Account Editor (isolated)',
    login: 'prolifegui_acceditor',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Account Editor',
    roleId: 'ProLifeGuiAccountEditor',
    permissions: [...ACCOUNT_FULL, 'ViewRevisions'],
    isolatedSpec: 'accounts.editor.multiuser.test.js',
  },
  {
    key: 'swEditor',
    title: 'Software Editor (isolated)',
    login: 'prolifegui_sweditor',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Software Editor',
    roleId: 'ProLifeGuiSoftwareEditor',
    permissions: [...LICENSE_FULL, 'ViewAccounts', 'ViewRevisions'],
    isolatedSpec: 'software.editor.multiuser.test.js',
  },
  {
    // Drives concurrent-session-banner.multiuser.test.js - opens TWO simultaneous sessions of this
    // SAME user on purpose (that's the whole point of the test: observe RemoteCollectionView.qml's
    // "modified from another computer" banner, which only fires when a SECOND session of the SAME user
    // changes a row the FIRST session is looking at). Still needs its own isolated user so it doesn't
    // collide with some OTHER spec file's own document-tab activity under a shared matrix user.
    key: 'bannerEditor',
    title: 'Concurrent Session Banner (isolated)',
    login: 'prolifegui_bannereditor',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Concurrent Session Banner',
    roleId: 'ProLifeGuiConcurrentSessionBanner',
    permissions: [...SENSOR_FULL],
    isolatedSpec: 'concurrent-session-banner.multiuser.test.js',
  },
  {
    // Drives document-tabs.multiuser.test.js - the GENERIC multi-tab document workspace mechanism
    // (MultiDocumentCollectionView.qml/TabDelegate.qml), exercised via Devices purely as a convenient,
    // already-well-instrumented vehicle - the behavior under test isn't Devices-specific. Same
    // cross-file workspace-collision class as the other isolated editor users (opens document tabs).
    key: 'tabsEditor',
    title: 'Document Tabs (isolated)',
    login: 'prolifegui_tabseditor',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Document Tabs',
    roleId: 'ProLifeGuiDocumentTabs',
    permissions: [...SENSOR_FULL],
    isolatedSpec: 'document-tabs.multiuser.test.js',
  },
  {
    // Same cross-file workspace-collision class as devEditor/accEditor/swEditor above, but for
    // administration.editor.multiuser.test.js's Role/User/Group "New document" blocks - those also open
    // document tabs via the SAME server-side Collection Document Service, and administration.multiuser
    // .test.js's own collection-navigation tests (run under su/fullAccess/adminManager) call
    // closeAllDocumentTabs() on every openPage(), which can close a tab this spec is mid-test on.
    key: 'admEditor',
    title: 'Administration Editor (isolated)',
    login: 'prolifegui_admeditor',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Administration Editor',
    roleId: 'ProLifeGuiAdministrationEditor',
    permissions: [...ADMIN_FULL],
    isolatedSpec: 'administration.editor.multiuser.test.js',
  },
  {
    // Administration user: sees ONLY the Administration page (+ the universal Search). Validates that
    // the Admin area is reachable/gated and that no domain page leaks in for an admin-only role.
    key: 'adminManager',
    title: 'Administration Manager',
    login: 'prolifegui_admin',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Administration Manager',
    roleId: 'ProLifeGuiAdministrationManager',
    permissions: [...ADMIN_FULL],
  },
  {
    // Organizations viewer: sees ONLY the Organizations (Tenants) page (+ Search). This is the only
    // user that makes the Organizations page appear, so its baseline documents that page.
    key: 'orgViewer',
    title: 'Organizations Viewer',
    login: 'prolifegui_org',
    password: PASSWORD,
    seed: true,
    roleName: 'ProLifeGui Organizations Viewer',
    roleId: 'ProLifeGuiOrganizationsViewer',
    permissions: ['ViewOrganizations'],
  },
  {
    // Read-only Accounts: sees Accounts + Search; no New/Remove commands, every field read-only.
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
    // No domain permissions at all: only the universal Search page is reachable.
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

// Fast default subset for iterative/local runs: `su` (superuser baseline) + `fullAccess` (broadest
// non-superuser coverage - every page/command via real granted permissions, not '*', so editor
// save-paths etc. still get exercised without needing the full matrix). This does NOT validate
// per-user restrictions (accountsViewer/noAccess/etc. seeing less) - set PROLIFE_GUI_ALL_USERS=1 (or
// Run-CiTests.ps1 -AllUsers) to run the complete permission matrix when that's what you're testing.
const DEFAULT_USER_KEYS = ['su', 'fullAccess'];
const useAllUsers = () => process.env.PROLIFE_GUI_ALL_USERS === '1' || process.env.PROLIFE_GUI_ALL_USERS === 'true';

/**
 * The users that should actually get a Playwright project / storageState this run.
 *
 * `isolatedSpec` users (the editor-isolation users) are ALWAYS active, in both the fast default subset
 * and the full matrix: their whole point is that their dedicated editor spec runs under them and NO
 * other user - so if they weren't active, that editor spec would run under nobody at all (every other
 * project testIgnores it - see buildProjects.js). They add no matrix breadth (one user, one spec), so
 * including them by default costs one extra project per editor file, not a full re-run of everything.
 */
const activeUsers = () => {
  const matrix = useAllUsers() ? USERS : USERS.filter((u) => DEFAULT_USER_KEYS.includes(u.key));
  const isolated = USERS.filter((u) => u.isolatedSpec && !matrix.includes(u));
  return [...matrix, ...isolated];
};

module.exports = {
  USERS,
  GUEST,
  PASSWORD,
  DEFAULT_USER_KEYS,
  byKey,
  can,
  useAllUsers,
  activeUsers,
  authFile: (key) => `.auth/${key}.json`,
  seededUsers: () => USERS.filter((u) => u.seed),
};
