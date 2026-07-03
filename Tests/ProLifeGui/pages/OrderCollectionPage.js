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

const { CollectionPage } = require('./CollectionPage');
const gui = require('../lib/gui');

const FILTERS = {
  customers: 'CustomersFilter',
  creationDate: 'CreationDateFilter',
  documentState: 'DocumentState',
};

class OrderCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'Orders');
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
    await gui.clickButton(this.page, ['FilterPanel', 'ClearAllFilters']);
    return this;
  }

  static get FILTERS() {
    return FILTERS;
  }
}

module.exports = { OrderCollectionPage, FILTERS };
