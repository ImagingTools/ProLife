// Single source of truth for the ProLife GUI test users.
//
// Consumed by:
//   - playwright.config.js  -> one Playwright *project* per user (each with its own storageState)
//   - global-setup.js       -> UI-logs-in each user and mints its storageState
//   - fixtures/test.js      -> exposes the current user (resolved from the project name) to tests
//   - scripts/seed-fixture-users.js (via fixtures/seed.js) -> bakes the roles/users into puma.backup
//
// One spec file, one user - the same model as Lisa/Tests/LisaGui. Playwright runs different FILES on
// different workers (fullyParallel is off, so a file is never split), but the server keeps a lot of
// state PER USER: the open-documents workspace (CCollectionDocumentServiceControllerComp keys it by
// userId and fans every open/close out to all of that user's sessions), filters, sort, the selected
// row, column layout, the last-open page. Two spec files driving the same user at the same time close
// each other's documents and change each other's screenshots. `isolatedSpec` pins every user below to
// exactly one file (see the kit's buildProjects.js), so no two files ever share a user.
//
// Every seeded user gets the SAME full ProLife permission set on purpose: a missing permission makes
// its tests skip green instead of fail, and a per-user permission matrix is not what these specs are
// for. `permissions` is what gets GRANTED to the user's role when seeding (fixtures/seed.js joins it
// into the ';'-delimited string the server stores). Codes are the FeatureId strings from
// Impl/ProLifeServer/ProLifeFeatures.xml, except SplitLicense/RevokeLicense, which are
// CommandPermissions declared in SoftwareProductsPage.acc - a role can be granted the raw string
// either way.
//
// The superuser bypasses permission checks server-side and is therefore sent an EMPTY list,
// indistinguishable from a user granted nothing; `permissions: ['*']` is what tells the two apart, and
// defineUsers refuses a superuser without it.
//
// Changing this list means regenerating puma.backup (Generate-Backups.ps1): the users are baked in
// there, global-setup.js only logs them in.

const { defineUsers } = require('imtcore-gui-testkit/fixtures/defineUsers');

const PASSWORD = 'ProLifeGui_2026!';

const FULL_ACCESS = [
  // WorkspaceManagement
  'ViewWorkspace', 'ViewUserActions', 'ViewAllUserActions', 'ViewAnalytics',
  // AccountManagement
  'ViewAccounts', 'ViewAllAccounts', 'ViewAccountHistory',
  'AddAccount', 'RemoveAccount', 'EditAccount', 'ChangeAccount',
  'ChangeAccountGroups', 'ChangeCompanyAddress', 'ChangeAccountEmail',
  'ChangeAccountDescription', 'ChangeAccountName', 'ChangeCustomerId',
  // SensorManagement
  'ViewSensors', 'ViewAllSensors', 'ViewSensorHistory',
  'AddSensor', 'RemoveSensor', 'EditSensor', 'ChangeSensor',
  'ChangeOrderForSensor', 'ChangeProductionStatus', 'ChangeHardwareConfiguration',
  'ChangeDeviceType', 'ChangeDescriptionForSensor', 'ChangeSerialNumberForSensor',
  'ChangeMacAddress', 'ChangeProjectForSensor',
  'BindSensor', 'UnbindSensor', 'CreateLicenseFile', 'TransferLicenses', 'ResetTransferCounter',
  // OrderManagement
  'ViewOrders', 'ViewAllOrders', 'ViewOrderHistory',
  'AddOrder', 'RemoveOrder', 'EditOrder', 'ChangeOrder',
  'ChangeOrderProducts', 'ChangeOrderStatus', 'ChangeCustomer',
  'ChangeDescriptionForOrder', 'ChangePurchaseOrderId', 'ChangeDeliveryId',
  // LicenseManagement (+ the SplitLicense/RevokeLicense command permissions, see the header)
  'ViewLicenses', 'ViewAllLicenses', 'ViewLicenseHistory',
  'AddLicense', 'RemoveLicense', 'EditLicense', 'ChangeLicense',
  'ChangeLicenseNumber', 'ChangeExpiration', 'ChangeProductLicenses',
  'ChangeProductForLicense', 'ChangeOrderForLicense', 'ChangeProjectForLicense',
  'SplitLicense', 'RevokeLicense',
  // OrganizationManagement
  'ViewOrganizations',
  // RevisionManagement - the Revision command is gated by ViewRevisions, not by the View*History ones
  'ViewRevisions',
  // Administration
  'ViewUsers', 'ViewRoles', 'ViewGroups',
  'ViewUserHistory', 'ViewRoleHistory', 'ViewGroupHistory',
  'ChangeUser', 'EditUser', 'AddUser', 'RemoveUser',
  'ChangeRole', 'EditRole', 'AddRole', 'RemoveRole',
  'ChangeGroup', 'EditGroup', 'AddGroup', 'RemoveGroup',
];

// A seeded user pinned to one spec file. The login, the role name and the role code all derive from
// the key, so the three can never drift apart.
function specUser(key, title, spec) {
  return {
    key,
    title: `${title} (isolated)`,
    login: `prolifegui_${key.toLowerCase()}`,
    password: PASSWORD,
    seed: true,
    roleName: `ProLifeGui ${title}`,
    roleId: `ProLifeGui${key.charAt(0).toUpperCase()}${key.slice(1)}`,
    permissions: FULL_ACCESS,
    isolatedSpec: spec,
  };
}

const USERS = [
  {
    key: 'su',
    title: 'Superuser',
    login: 'su',
    // Pre-existing superuser: baked into puma.backup, and bootstrapped via CreateSuperuser by
    // Run-CiTests.ps1 as a safety net.
    password: '1',
    seed: false,
    permissions: ['*'],
    // "Create license file" offers the Encrypt/Unencrypt choice to the superuser only, so that flow is
    // su's spec. su is also the peer that edits a device in concurrent-session-banner.test.js and the
    // account login.guest.test.js signs in with; both are fresh sessions that open no documents while
    // this file runs.
    isolatedSpec: 'devices.license-file.test.js',
  },
  specUser('accountsCollection', 'Accounts Collection', 'accounts.collection.test.js'),
  specUser('accountsEditor', 'Accounts Editor', 'accounts.editor.test.js'),
  specUser('administration', 'Administration', 'administration.test.js'),
  specUser('administrationEditor', 'Administration Editor', 'administration.editor.test.js'),
  // The WATCHING session of the banner spec. The banner fires only on a change made by a different
  // USER (RemoteCollectionChangeListener.qml compares ownerId to the current user), so the change is
  // made by `su`.
  specUser('concurrentSessionBanner', 'Concurrent Session Banner', 'concurrent-session-banner.test.js'),
  specUser('devicesCollection', 'Devices Collection', 'devices.collection.test.js'),
  specUser('devicesEditor', 'Devices Editor', 'devices.editor.test.js'),
  specUser('ordersCollection', 'Orders Collection', 'orders.collection.test.js'),
  specUser('ordersEditor', 'Orders Editor', 'orders.editor.test.js'),
  specUser('organizations', 'Organizations', 'organizations.test.js'),
  specUser('search', 'Search', 'search.test.js'),
  specUser('sessionExpiry', 'Session Expiry', 'session-expiry.test.js'),
  specUser('softwareCollection', 'Software Collection', 'software.collection.test.js'),
  specUser('softwareEditor', 'Software Editor', 'software.editor.test.js'),
  specUser('support', 'Support', 'support.test.js'),
  specUser('userProfile', 'User Profile', 'user-profile.test.js'),
  specUser('workspace', 'Workspace', 'workspace.test.js'),
];

module.exports = defineUsers({ users: USERS });
module.exports.PASSWORD = PASSWORD;
