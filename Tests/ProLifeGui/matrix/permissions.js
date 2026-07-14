// UI element -> required permission code(s), mirroring the server-side gating in
//   Partitura/ProLifeQmlVoce.arp/Pages.acc            (each page's PagePermissions)
//   Partitura/ImtGraphQlVoce.arp/PagesController.acc  (Workspace / Search / Administration)
//   Impl/ProLifeServer/ProLifeFeatures.xml             (the permission catalogue)
//
// A page/command is visible to a user if the user holds AT LEAST ONE of the listed permissions
// (that is how PagePermissions works). The Search page uses "*" (visible to everyone).
//
// This drives:
//   - per-user test.skip decisions (don't try to open a page a user cannot see),
//   - optional structural permission-matrix assertions (expectHasPage / expectNoPage),
//   - documentation of which screenshots are expected to differ per user.

const { can } = require('../fixtures/users');

// pageId -> permission codes that make its MenuPanel button appear.
//
// These lists are transcribed VERBATIM from each page's PagePermissions in
//   Partitura/ProLifeQmlVoce.arp/Pages.acc            (Accounts/Devices/Orders/SoftwareProducts/Tenants)
//   Partitura/ImtGraphQlVoce.arp/PagesController.acc  (Workspace/Administration/Search)
// A page is shown if the user holds AT LEAST ONE of its codes. Keeping the FULL list matters once
// users hold granular Change* permissions (e.g. a user with only ChangeMacAddress must still be
// expected to see Hardware) - an incomplete list would make the structural "menu reflects
// permissions" assertions wrong for such users.
//
// The pageId keys double as the MenuPanel button ids: MenuPanel.qml names each button
// `model["id"] + "Button"`, and the pages' model ids are exactly these keys (Accounts/Devices/
// Orders/SoftwareProducts come from CollectionId, the rest from PageId).
const PAGE_PERMISSIONS = {
  Workspace: ['ViewWorkspace'],
  Accounts: [
    'ViewAccounts', 'AddAccount', 'RemoveAccount',
    'ChangeCustomerId', 'ChangeAccountDescription', 'ChangeAccountName',
    'ChangeAccountEmail', 'ChangeCompanyAddress', 'ChangeAccountGroups',
  ],
  Devices: [
    // NOTE: DevicesPage.acc lists a bare "Remove" here too (an app-config quirk, not a real
    // FeatureId); it is harmless because no role ever holds it, and is omitted rather than mirrored.
    'ViewSensors', 'AddSensor', 'RemoveSensor',
    'ChangeProjectForSensor', 'ChangeMacAddress', 'ChangeSerialNumberForSensor',
    'ChangeDescriptionForSensor', 'ChangeDeviceType', 'ChangeHardwareConfiguration',
    'ChangeProductionStatus', 'ChangeOrderForSensor',
  ],
  Orders: [
    'ViewOrders', 'AddOrder', 'RemoveOrder',
    'ChangeDeliveryId', 'ChangePurchaseOrderId', 'ChangeDescriptionForOrder',
    'ChangeCustomer', 'ChangeOrderStatus', 'ChangeOrderProducts',
  ],
  SoftwareProducts: ['ViewLicenses'],
  Tenants: ['ViewOrganizations'], // "Organizations" page (TenantsPage, IsVisible=true in ProLife)
  Administration: ['ViewUsers', 'ViewRoles', 'ViewGroups', 'ChangeUser', 'ChangeRole', 'ChangeGroups'],
  Search: ['*'], // universal
};

/**
 * Can this user see the given page? '*' pages are visible to everyone; superusers ('*' perms) see all.
 * @param {import('../fixtures/users').TestUser} user
 * @param {string} pageId
 */
function canSeePage(user, pageId) {
  const perms = PAGE_PERMISSIONS[pageId] || [];
  if (perms.includes('*')) return true;
  return perms.some((p) => can(user, p));
}

/** The set of pages a given user is expected to see (for matrix assertions/documentation). */
function visiblePages(user) {
  return Object.keys(PAGE_PERMISSIONS).filter((pageId) => canSeePage(user, pageId));
}

// --- Hardware (Devices) command gating ---------------------------------------------------------
// commandId -> permission that makes the command bar button appear/enabled.
// From ProLifeFeatures.xml + DeviceCollectionViewCommandsDelegate.qml. New/Edit/Remove are the base
// collection commands; the rest are the custom sensor commands (also require exactly one selected row
// with a non-empty MAC for Bind, per updateStateCustomCommands()).
const DEVICE_COMMAND_PERMISSIONS = {
  New: ['AddSensor'],
  Edit: ['EditSensor', 'ChangeSensor', 'ChangeMacAddress', 'ChangeProductionStatus'],
  Remove: ['RemoveSensor'],
  // The Revision command (BaseCollectionCommands.acc's shared "Revision" command, opening the document
  // history) is gated by ViewRevisions, NOT the per-entity View*History permission. Verified against
  // the seeded roles: fullAccess holds ViewSensorHistory/ViewLicenseHistory/ViewOrderHistory but NOT
  // ViewRevisions, and its command bar correctly shows no Revision button; the specialist roles
  // (hardwareManager/licenseManager/orderManager) explicitly carry ViewRevisions and do show it. The
  // View*History permissions gate the history DATA, not the command's visibility.
  Revision: ['ViewRevisions'],
  Bind: ['BindSensor'],
  CreateLicenseFile: ['CreateLicenseFile'],
  TransferLicenses: ['TransferLicenses'],
  ResetTransferCounter: ['ResetTransferCounter'],
  Support: ['*'],
};

function canRunDeviceCommand(user, commandId) {
  // "su" is the bootstrapped superuser (CUserControllerComp::OnCreateSuperuser in ImtCore) - it is
  // never assigned a role, so its real server-side permission set is EMPTY, not "everything". That's
  // invisible for most Devices commands only because Bind/CreateLicenseFile/TransferLicenses have no
  // registered command-permission entry at all in Partitura/ProLifeVoce.arp/DevicePermissions.acc's
  // PermissionsJoiner (so they're ungated for ANY logged-in user - a separate finding, not a su quirk).
  // ResetTransferCounter is the one Devices command that IS actually permission-checked
  // (CCommandsControllerComp::GetRepresentationFromGuiElementContainer), and su genuinely fails that
  // check server-side. Model that one real exception here instead of treating su's '*' as a true
  // bypass; use a role-based user (e.g. "fullAccess", which holds ResetTransferCounter via
  // SENSOR_FULL in fixtures/users.js) to exercise this command end-to-end.
  if (user.key === 'su' && commandId === 'ResetTransferCounter') return false;

  const perms = DEVICE_COMMAND_PERMISSIONS[commandId] || [];
  if (perms.includes('*')) return true;
  return perms.some((p) => can(user, p));
}

// --- DeviceEditor field editability (DeviceEditor.checkPermissions) -----------------------------
// editor field objectName -> permission that makes it editable (readOnly=false / changeable=true).
// For a NEW document, AddSensor unlocks every field.
const DEVICE_FIELD_PERMISSIONS = {
  DescriptionInput: ['ChangeDescriptionForSensor'],
  SerialNumberInput: ['ChangeSerialNumberForSensor'],
  MacAddressInput: ['ChangeMacAddress'],
  OrderCombo: ['ChangeOrderForSensor'],
  EndCustomerCombo: ['ChangeOrderForSensor'],
  ProductionStatusCombo: ['ChangeProductionStatus'],
  ProjectInput: ['ChangeProjectForSensor'],
  StationInput: ['ChangeProjectForSensor'],
  AreaInput: ['ChangeProjectForSensor'],
  DeviceTypeCombo: ['ChangeHardwareConfiguration', 'ChangeDeviceType'],
  HardwareConfigurationCombo: ['ChangeHardwareConfiguration', 'ChangeDeviceType'],
};

function canEditDeviceField(user, fieldObjectName, isNew = false) {
  if (isNew && can(user, 'AddSensor')) return true;
  const perms = DEVICE_FIELD_PERMISSIONS[fieldObjectName] || [];
  return perms.some((p) => can(user, p));
}

// --- Software (SoftwareProducts) command gating ------------------------------------------------
// commandId -> permission(s) that make the command bar button appear/enabled.
// Base: New/Edit/Remove/Revision from StandardCollectionCommands + custom from SoftwareProductsPage.acc
// CreateLicenseFile enabled conditionally in delegate (also requires hardware link + serial).
const SOFTWARE_COMMAND_PERMISSIONS = {
  New: ['AddLicense'],
  Edit: ['ViewLicenses', 'ChangeLicense', 'ChangeProjectForLicense', 'ChangeOrderForLicense', 'ChangeProductForLicense', 'ChangeProductLicenses', 'ChangeLicenseNumber', 'ChangeExpiration'],
  Remove: ['RemoveLicense'],
  // Gated by ViewRevisions, not ViewLicenseHistory - see the DEVICE_COMMAND_PERMISSIONS.Revision note.
  Revision: ['ViewRevisions'],
  CreateLicenseFile: ['CreateLicenseFile'],
  Split: ['SplitLicense'],
  Revoke: ['RevokeLicense'],
  // Support is universal like in devices if present
  Support: ['*'],
};

function canRunSoftwareCommand(user, commandId) {
  const perms = SOFTWARE_COMMAND_PERMISSIONS[commandId] || [];
  if (perms.includes('*')) return true;
  return perms.some((p) => can(user, p));
}

// --- SoftwareEditor field editability ----------------------------------------------------------
// editor field objectName -> permission(s) that make it editable.
// For a NEW document, AddLicense unlocks every field.
const SOFTWARE_FIELD_PERMISSIONS = {
  ProjectInput: ['ChangeProjectForLicense'],
  OrderCombo: ['ChangeOrderForLicense'],
  ProductCombo: ['ChangeProductForLicense'],
  LicenseCombo: ['ChangeProductLicenses'],
  SerialNumberInput: ['ChangeLicenseNumber'],
  ExpirationDatePicker: ['ChangeExpiration'],
  UnlimitedSwitch: ['ChangeExpiration'],
  InternalUseSwitch: ['ChangeLicense'],
  // IsMultipleSwitch / ProductCountSpinBox gated by hierarchy (hasChildren/hasParent) even if editable
};

function canEditSoftwareField(user, fieldObjectName, isNew = false) {
  if (isNew && can(user, 'AddLicense')) return true;
  const perms = SOFTWARE_FIELD_PERMISSIONS[fieldObjectName] || [];
  return perms.some((p) => can(user, p));
}

// --- Orders command gating ---------------------------------------------------------------------
const ORDER_COMMAND_PERMISSIONS = {
  New: ['AddOrder'],
  Edit: ['ViewOrders', 'ChangeDeliveryId', 'ChangePurchaseOrderId', 'ChangeDescriptionForOrder', 'ChangeCustomer', 'ChangeOrderStatus', 'ChangeOrderProducts'],
  Remove: ['RemoveOrder'],
  // Gated by ViewRevisions only (ViewOrderHistory gates the history data, not the command's
  // visibility) - see the DEVICE_COMMAND_PERMISSIONS.Revision note. fullAccess has ViewOrderHistory
  // but not ViewRevisions and correctly shows no Revision button.
  Revision: ['ViewRevisions'],
  // SetDescription is context menu, driven by ChangeDescriptionForOrder
};

function canRunOrderCommand(user, commandId) {
  const perms = ORDER_COMMAND_PERMISSIONS[commandId] || [];
  if (perms.includes('*')) return true;
  return perms.some((p) => can(user, p));
}

const ORDER_FIELD_PERMISSIONS = {
  DeliveryIdInput: ['ChangeDeliveryId'],
  PurchaseOrderIdInput: ['ChangePurchaseOrderId'],
  DescriptionInput: ['ChangeDescriptionForOrder'],
  CustomerCombo: ['ChangeCustomer'],
  EndCustomerCombo: ['ChangeCustomer'],
  OrderStatusCombo: ['ChangeOrderStatus'],
};

function canEditOrderField(user, fieldObjectName, isNew = false) {
  if (isNew && can(user, 'AddOrder')) return true;
  const perms = ORDER_FIELD_PERMISSIONS[fieldObjectName] || [];
  return perms.some((p) => can(user, p));
}

// --- Accounts command gating -------------------------------------------------------------------
const ACCOUNT_COMMAND_PERMISSIONS = {
  New: ['AddAccount'],
  Edit: ['ViewAccounts', 'ChangeAccount', 'ChangeAccountName', 'ChangeAccountDescription', 'ChangeAccountEmail', 'ChangeCustomerId', 'ChangeCompanyAddress', 'ChangeAccountGroups'],
  Remove: ['RemoveAccount'],
  // No Revision typically for accounts; groups are special table
};

function canRunAccountCommand(user, commandId) {
  const perms = ACCOUNT_COMMAND_PERMISSIONS[commandId] || [];
  if (perms.includes('*')) return true;
  return perms.some((p) => can(user, p));
}

const ACCOUNT_FIELD_PERMISSIONS = {
  CustomerIdInput: ['ChangeCustomerId'],
  AccountNameInput: ['ChangeAccountName'],
  AccountDescriptionInput: ['ChangeAccountDescription'],
  EmailInput: ['ChangeAccountEmail'],
  CountryInput: ['ChangeCompanyAddress'],
  CityInput: ['ChangeCompanyAddress'],
  PostalCodeInput: ['ChangeCompanyAddress'],
  StreetInput: ['ChangeCompanyAddress'],
  GroupsTable: ['ChangeAccountGroups'],
};

function canEditAccountField(user, fieldObjectName, isNew = false) {
  if (isNew && can(user, 'AddAccount')) return true;
  const perms = ACCOUNT_FIELD_PERMISSIONS[fieldObjectName] || [];
  return perms.some((p) => can(user, p));
}

// --- Administration (Roles/Users/Groups) command gating ----------------------------------------
// Unlike Devices/Orders/Accounts/Software (separate AddX/ChangeX/RemoveX permissions), the server
// gates New/Edit/Remove for each of these three entities behind a SINGLE "Change*" permission -
// confirmed for Roles via ImtAuthVoce.arp/RoleCollectionDocumentService.acc's CommandPermissions
// (UpdateRoleFromRepresentation -> ChangeRole; GetRoleRepresentation -> ViewRoles). Users/Groups
// mirror the same one-permission-per-entity shape (PAGE_PERMISSIONS.Administration already lists
// exactly ChangeUser/ChangeRole/ChangeGroups, no separate Add*/Remove* ids).
const ROLE_COMMAND_PERMISSIONS = {
  New: ['ChangeRole'],
  Edit: ['ChangeRole'],
  Remove: ['ChangeRole'],
};

function canRunRoleCommand(user, commandId) {
  const perms = ROLE_COMMAND_PERMISSIONS[commandId] || [];
  return perms.some((p) => can(user, p));
}

const ROLE_FIELD_PERMISSIONS = {
  RoleNameInput: ['ChangeRole'],
  RoleDescriptionInput: ['ChangeRole'],
};

function canEditRoleField(user, fieldObjectName) {
  const perms = ROLE_FIELD_PERMISSIONS[fieldObjectName] || [];
  return perms.some((p) => can(user, p));
}

const USER_COMMAND_PERMISSIONS = {
  New: ['ChangeUser'],
  Edit: ['ChangeUser'],
  Remove: ['ChangeUser'],
};

function canRunUserCommand(user, commandId) {
  const perms = USER_COMMAND_PERMISSIONS[commandId] || [];
  return perms.some((p) => can(user, p));
}

const USER_FIELD_PERMISSIONS = {
  UsernameInput: ['ChangeUser'],
  UserNameInput: ['ChangeUser'],
  MailInput: ['ChangeUser'],
};

function canEditUserField(user, fieldObjectName) {
  const perms = USER_FIELD_PERMISSIONS[fieldObjectName] || [];
  return perms.some((p) => can(user, p));
}

const GROUP_COMMAND_PERMISSIONS = {
  New: ['ChangeGroups'],
  Edit: ['ChangeGroups'],
  Remove: ['ChangeGroups'],
};

function canRunGroupCommand(user, commandId) {
  const perms = GROUP_COMMAND_PERMISSIONS[commandId] || [];
  return perms.some((p) => can(user, p));
}

const GROUP_FIELD_PERMISSIONS = {
  GroupNameInput: ['ChangeGroups'],
  GroupDescriptionInput: ['ChangeGroups'],
};

function canEditGroupField(user, fieldObjectName) {
  const perms = GROUP_FIELD_PERMISSIONS[fieldObjectName] || [];
  return perms.some((p) => can(user, p));
}

module.exports = {
  PAGE_PERMISSIONS,
  canSeePage,
  visiblePages,
  DEVICE_COMMAND_PERMISSIONS,
  canRunDeviceCommand,
  DEVICE_FIELD_PERMISSIONS,
  canEditDeviceField,
  SOFTWARE_COMMAND_PERMISSIONS,
  canRunSoftwareCommand,
  SOFTWARE_FIELD_PERMISSIONS,
  canEditSoftwareField,
  ORDER_COMMAND_PERMISSIONS,
  canRunOrderCommand,
  ORDER_FIELD_PERMISSIONS,
  canEditOrderField,
  ACCOUNT_COMMAND_PERMISSIONS,
  canRunAccountCommand,
  ACCOUNT_FIELD_PERMISSIONS,
  canEditAccountField,
  ROLE_COMMAND_PERMISSIONS,
  canRunRoleCommand,
  ROLE_FIELD_PERMISSIONS,
  canEditRoleField,
  USER_COMMAND_PERMISSIONS,
  canRunUserCommand,
  USER_FIELD_PERMISSIONS,
  canEditUserField,
  GROUP_COMMAND_PERMISSIONS,
  canRunGroupCommand,
  GROUP_FIELD_PERMISSIONS,
  canEditGroupField,
};
