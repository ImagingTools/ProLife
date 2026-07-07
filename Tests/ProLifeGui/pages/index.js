const { BasePage } = require('./BasePage');
const { CollectionPage } = require('./CollectionPage');
const { WorkspacePage } = require('./WorkspacePage');
const { DeviceCollectionPage, FILTERS: DEVICE_FILTERS } = require('./DeviceCollectionPage');
const { DeviceEditorPage } = require('./DeviceEditorPage');
const { SoftwareCollectionPage, FILTERS: SOFTWARE_FILTERS } = require('./SoftwareCollectionPage');
const { SoftwareEditorPage } = require('./SoftwareEditorPage');
const { OrderCollectionPage, FILTERS: ORDER_FILTERS } = require('./OrderCollectionPage');
const { OrderEditorPage } = require('./OrderEditorPage');
const { AccountCollectionPage, FILTERS: ACCOUNT_FILTERS } = require('./AccountCollectionPage');
const { AccountEditorPage } = require('./AccountEditorPage');
const { AdministrationPage } = require('./AdministrationPage');
const { OrganizationsPage } = require('./OrganizationsPage');
const { SearchPage } = require('./SearchPage');

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
};
