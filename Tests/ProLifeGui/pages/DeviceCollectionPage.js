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
const { TableConfigDialog } = require('../controls');
const gui = require('../lib/gui');

// "Added"/"Last Modified" column header ids (DevicesPage.acc's CollectionHeadersProvider /
// DeviceItemTypeMetaInfo.s_added / s_timeStamp) - their values change over time and after edits, so
// they're masked in screenshots rather than asserted on.
const TIMESTAMP_HEADER_IDS = ['added', 'timeStamp'];

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

  // --- CreateLicenseFile's Encrypt/Unencrypt choice (superuser only) -----------------------------
  // DeviceCollectionViewCommandsDelegate.qml's onCreateLicenseFile() only shows this popup for
  // AuthorizationController.loggedUserIsSuperuser() - every other user's CreateLicenseFile call
  // skips straight to sending the request (or a validation error dialog). The popup is opened
  // directly (not via a ComboBox), and its items aren't text-addressable (PopupMenuDialog's own
  // default delegate doesn't override objectName like ComboBox.qml's does) - only position works:
  // index 0 = "Encrypted" (Encrypt), index 1 = "Unencrypted" (NotEncrypt), per the addItem() order
  // in encryptPopupMenuModel.
  chooseEncrypted() {
    return gui.clickPopupItemByIndex(this.page, 0);
  }
  chooseNotEncrypted() {
    return gui.clickPopupItemByIndex(this.page, 1);
  }

  // --- column configuration (right-click a header) -----------------------------------------------

  /** Open the "Table configuration" dialog by right-clicking a column header. */
  async openColumnConfig(headerId = 'status') {
    const dialog = new TableConfigDialog(this.page);
    await dialog.openViaHeader(headerId);
    return dialog;
  }

  /** Screenshot masks for the Added/Last Modified columns - see TIMESTAMP_HEADER_IDS above. */
  timestampColumnMasks() {
    return this.table.columnMasks(TIMESTAMP_HEADER_IDS);
  }

  // --- filters ----------------------------------------------------------------------------------

  /** Pick an option in a registered field filter by its option text (e.g. status "None"). */
  async selectFilterOption(filterKey, optionText) {
    const filterId = FILTERS[filterKey] || filterKey;
    await this.filters.combo(filterId).select(optionText);
    return this;
  }

  /**
   * Pick an option in a registered field filter BY POSITION (see DeviceCollectionView.qml for the
   * source-defined option order backing each filterKey - e.g. 'status' is
   * None/Accepted/InProgress/Canceled/OnHold/Finished/Defect/InRepair/Decommissioned, indices 0-8;
   * 'license' is WithoutLicense/WithLicense, indices 0-1, per LicenseFilterDelegate.qml).
   */
  async selectFilterOptionByIndex(filterKey, index) {
    const filterId = FILTERS[filterKey] || filterKey;
    await this.filters.combo(filterId).selectIndex(index);
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

  // --- precondition filters for context-sensitive commands --------------------------------------
  // Bind/CreateLicenseFile/TransferLicenses/ResetTransferCounter only actually DO something on a row
  // that satisfies a server-side data precondition (DeviceCollectionViewCommandsDelegate.qml); a bare
  // selectRow(0) picks whatever the table's default sort happens to put first, which may not qualify
  // (e.g. no MAC address, no license), making the command silently no-op instead of exercising it.
  // These filter first so row 0 is guaranteed to be a row the command is actually meant to act on.

  /** Rows with SensorStatus "Finished" (index 5) - Finished sensors always carry a MAC address. */
  async filterFinishedSensors() {
    await this.clearAllFilters();
    await this.selectFilterOptionByIndex('status', 5);
    return this;
  }

  /** Rows with LicenseStatus "Sensors with license" (index 1) - satisfies s_inUse === true. */
  async filterSensorsWithLicense() {
    await this.clearAllFilters();
    await this.selectFilterOptionByIndex('license', 1);
    return this;
  }

  /** Finished AND licensed - CreateLicenseFile's precondition (inUse + MAC + serial + status). */
  async filterFinishedSensorsWithLicense() {
    await this.clearAllFilters();
    await this.selectFilterOptionByIndex('status', 5);
    await this.selectFilterOptionByIndex('license', 1);
    return this;
  }

  static get FILTERS() {
    return FILTERS;
  }
}

module.exports = { DeviceCollectionPage, FILTERS };
