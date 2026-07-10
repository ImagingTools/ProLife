// OrderCollectionPage - the Orders page (pageId "Orders") collection view.
//
// Grounded in:
//   ProLife/Qml/Include/prolifeqml/OrderCollectionView.qml
//   ProLife/Qml/Include/prolifeqml/OrderEditor.qml + OrderEditorCommandsDelegate
//
// Filters:
//   CustomersFilter (dynamic), built-in CreationDateFilter, DocumentState, SearchTextInput
//
// Commands: New, Edit, Remove, Revision (standard); SetDescription in context menu.

const { CollectionPage } = require('imtcore-gui-testkit/pages/CollectionPage');
const gui = require('imtcore-gui-testkit/lib/gui');

// "Added"/"Last Modified" column header ids (OrdersPage.acc's HeaderIds "added"/"timeStamp") - their
// values change over time and after edits, so they're masked in screenshots rather than asserted on.
const TIMESTAMP_HEADER_IDS = ['added', 'timeStamp'];

const FILTERS = {
  customers: 'CustomersFilter',
  creationDate: 'CreationDateFilter',
  documentState: 'DocumentState',
};

class OrderCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'Orders');
  }

  /** Screenshot masks for the Added/Last Modified columns - see TIMESTAMP_HEADER_IDS above. */
  timestampColumnMasks() {
    return this.columnMasks(TIMESTAMP_HEADER_IDS);
  }

  async selectFilterOption(filterKey, optionText) {
    const filterId = FILTERS[filterKey] || filterKey;
    await this.filters.combo(filterId).select(optionText);
    return this;
  }

  async setCreationDate(preset) {
    await this.filters.dateFilter(FILTERS.creationDate, preset);
    return this;
  }

  async clearFilter(filterKey) {
    await this.filters.clearFilter(FILTERS[filterKey] || filterKey);
    return this;
  }

  async clearAllFilters() {
    await this.filters.clearAllFilters();
    return this;
  }

  static get FILTERS() {
    return FILTERS;
  }
}

module.exports = { OrderCollectionPage, FILTERS };
