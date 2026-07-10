// BasePage/CollectionPage (generic base classes) and Administration/Organizations/Search (thin
// wrappers over ImtCore's own generic views) now live in the shared kit - re-exported here so no
// consumer of this barrel (tests, other page objects) needs to change its import.
const {
  BasePage,
  CollectionPage,
  AdministrationPage,
  OrganizationsPage,
  SearchPage,
  RoleCollectionPage,
  RoleEditorPage,
  UserCollectionPage,
  UserEditorPage,
  GroupCollectionPage,
  GroupEditorPage,
} = require('imtcore-gui-testkit/pages');
const { WorkspacePage } = require('./WorkspacePage');
const { DeviceCollectionPage, FILTERS: DEVICE_FILTERS } = require('./DeviceCollectionPage');
const { DeviceEditorPage } = require('./DeviceEditorPage');
const { SoftwareCollectionPage, FILTERS: SOFTWARE_FILTERS } = require('./SoftwareCollectionPage');
const { SoftwareEditorPage } = require('./SoftwareEditorPage');
const { OrderCollectionPage, FILTERS: ORDER_FILTERS } = require('./OrderCollectionPage');
const { OrderEditorPage } = require('./OrderEditorPage');
const { AccountCollectionPage, FILTERS: ACCOUNT_FILTERS } = require('./AccountCollectionPage');
const { AccountEditorPage } = require('./AccountEditorPage');

module.exports = {
  BasePage,
  CollectionPage,
  WorkspacePage,
  DeviceCollectionPage,
  DeviceEditorPage,
  DEVICE_FILTERS,
  SoftwareCollectionPage,
  SoftwareEditorPage,
  SOFTWARE_FILTERS,
  OrderCollectionPage,
  OrderEditorPage,
  ORDER_FILTERS,
  AccountCollectionPage,
  AccountEditorPage,
  ACCOUNT_FILTERS,
  AdministrationPage,
  OrganizationsPage,
  SearchPage,
  RoleCollectionPage,
  RoleEditorPage,
  UserCollectionPage,
  UserEditorPage,
  GroupCollectionPage,
  GroupEditorPage,
};
