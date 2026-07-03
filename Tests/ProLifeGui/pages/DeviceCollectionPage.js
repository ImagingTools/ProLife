// DeviceCollectionPage - the Hardware page (pageId "Devices") collection view.
//
// Grounded in:
//   ProLife/Qml/Include/prolifeqml/DeviceCollectionView.qml            (filters registered)
//   ProLife/Qml/Include/prolifeqml/DeviceCollectionViewCommandsDelegate.qml (commands + dialogs)
//   ImtCore RemoteCollectionView/CollectionView/TableBase/Pagination/FilterPanelDecorator (base UI)
//
// Command bar ids (button objectName = "<id>Button", under CommandsView):
//   New, Edit, Remove, Revision                          (base collection commands)
//   Bind, CreateLicenseFile, TransferLicenses,           (custom, enabled when exactly 1 row selected;
//   ResetTransferCounter, DecryptFile, Support            Bind also needs a non-empty MAC)
//
// Filters (FilterPanel -> "<filterId>", options are text-keyed, each has a ClearButton; the panel has
// ClearAllFilters and a built-in CreationDateFilter + DocumentState + SearchTextInput):
//   SensorStatusFilter, UsageFilter, LicenseFilter, LicenseCreationDateFilter,
//   CustomersFilter, ProductsFilter, LicensesFilter, DateFilter, DocumentState
//
// Commands are permission-gated server-side (New<-AddSensor, Remove<-RemoveSensor,
// Bind<-BindSensor, CreateLicenseFile<-CreateLicenseFile, TransferLicenses<-TransferLicenses, ...);
// see matrix/permissions.js.

const { CollectionPage } = require('./CollectionPage');
const gui = require('../lib/gui');

// Filter ids as registered in DeviceCollectionView.qml / FilterPanelDecorator.qml.
const FILTERS = {
  status: 'SensorStatusFilter',
  usage: 'UsageFilter',
  license: 'LicenseFilter',
  licenseCreationDate: 'LicenseCreationDateFilter',
  customers: 'CustomersFilter',
  products: 'ProductsFilter',
  licenses: 'LicensesFilter',
  creationDate: 'CreationDateFilter',
  documentState: 'DocumentState',
};

class DeviceCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'Devices');
  }

  // --- custom commands (each throws if the command button is missing/hidden) --------------------

  bind() {
    return this.runCommand('Bind');
  }
  createLicenseFile() {
    return this.runCommand('CreateLicenseFile');
  }
  transferLicenses() {
    return this.runCommand('TransferLicenses');
  }
  resetTransferCounter() {
    return this.runCommand('ResetTransferCounter');
  }
  decryptFile() {
    return this.runCommand('DecryptFile');
  }
  support() {
    return this.runCommand('Support');
  }

  // --- filters ----------------------------------------------------------------------------------

  /** Pick an option in a registered field filter by its option text (e.g. status "None"). */
  async selectFilterOption(filterKey, optionText) {
    const filterId = FILTERS[filterKey] || filterKey;
    await this.filters.combo(filterId).select(optionText);
    return this;
  }

  /** Open the built-in creation-date filter and pick a preset ('Month_Current', 'Year_Last', ...). */
  async setCreationDate(preset) {
    await this.filters.dateFilter(FILTERS.creationDate, preset);
    return this;
  }

  /** Clear one registered filter. */
  async clearFilter(filterKey) {
    await this.filters.clearFilter(FILTERS[filterKey] || filterKey);
    return this;
  }

  /** Clear every active filter. */
  async clearAllFilters() {
    await gui.clickButton(this.page, ['FilterPanel', 'ClearAllFilters']);
    return this;
  }

  static get FILTERS() {
    return FILTERS;
  }
}

module.exports = { DeviceCollectionPage, FILTERS };
