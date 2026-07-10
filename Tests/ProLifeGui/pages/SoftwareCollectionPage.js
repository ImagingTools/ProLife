// SoftwareCollectionPage - the Software page (pageId "SoftwareProducts") collection view.
//
// Grounded in:
//   ProLife/Qml/Include/prolifeqml/SoftwareProductCollectionView.qml            (filters registered)
//   ProLife/Qml/Include/prolifeqml/SoftwareProductsCollectionViewCommandsDelegate.qml (commands + dialogs)
//   ImtCore RemoteCollectionView/CollectionView/TableBase/Pagination/FilterPanelDecorator (base UI)
//
// Command bar ids (button objectName = "<id>Button", under CommandsView):
//   New, Edit, Remove, Revision                          (base collection commands)
//   Split, Revoke                                        (custom, enabled when exactly 1 row selected;
//                                                          Split needs isMultiple && productCount > 1)
//
// Filters (FilterPanel -> "<filterId>", options are text-keyed, each has a ClearButton; the panel has
// ClearAllFilters and a built-in CreationDateFilter + DocumentState + SearchTextInput):
//   LicenseStatusFilter, UsageFilter, ProductsFilter, LicensesFilter,
//   CustomersFilter, LicenseCreationDateFilter, DateFilter, DocumentState
//
// Commands are permission-gated server-side (New<-AddLicense, Remove<-RemoveLicense,
// Split<-SplitLicense, Revoke<-RevokeLicense, CreateLicenseFile<-CreateLicenseFile, ...);
// see matrix/permissions.js.

const { CollectionPage } = require('imtcore-gui-testkit/pages/CollectionPage');
const gui = require('imtcore-gui-testkit/lib/gui');

// "Added"/"Last Modified" column header ids (SoftwareProductsPage.acc's HeaderIds "added"/"timeStamp")
// - their values change over time and after edits, so they're masked in screenshots, not asserted on.
const TIMESTAMP_HEADER_IDS = ['added', 'timeStamp'];

// Filter ids as registered in SoftwareProductCollectionView.qml / FilterPanelDecorator.qml.
const FILTERS = {
  licenseStatus: 'LicenseStatusFilter',
  usage: 'UsageFilter',
  products: 'ProductsFilter',
  licenses: 'LicensesFilter',
  customers: 'CustomersFilter',
  licenseCreationDate: 'LicenseCreationDateFilter',
  creationDate: 'CreationDateFilter',
  documentState: 'DocumentState',
};

class SoftwareCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'SoftwareProducts');
  }

  // --- custom commands (each throws if the command button is missing/hidden) --------------------

  split() {
    return this.runCommand('Split');
  }
  revoke() {
    return this.runCommand('Revoke');
  }

  /** Screenshot masks for the Added/Last Modified columns - see TIMESTAMP_HEADER_IDS above. */
  timestampColumnMasks() {
    return this.columnMasks(TIMESTAMP_HEADER_IDS);
  }

  // --- filters ----------------------------------------------------------------------------------

  /** Pick an option in a registered field filter by its option text (e.g. status "Paired"). */
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

  /** Open the license creation date filter and pick a preset. */
  async setLicenseCreationDate(preset) {
    await this.filters.dateFilter(FILTERS.licenseCreationDate, preset);
    return this;
  }

  /** Clear one registered filter. */
  async clearFilter(filterKey) {
    await this.filters.clearFilter(FILTERS[filterKey] || filterKey);
    return this;
  }

  /** Clear every active filter. */
  async clearAllFilters() {
    await this.filters.clearAllFilters();
    return this;
  }

  static get FILTERS() {
    return FILTERS;
  }
}

module.exports = { SoftwareCollectionPage, FILTERS };
