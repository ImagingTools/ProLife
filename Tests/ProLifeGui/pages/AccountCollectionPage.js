// AccountCollectionPage - the Accounts page (pageId "Accounts") collection view.
//
// Grounded in:
//   ProLife/Qml/Include/prolifeqml/AccountCollectionView.qml
//
// Primarily uses built-in search. Limited custom filters.
// Commands: New, Edit, Remove (SetDescription via context for ChangeAccountDescription).

const { CollectionPage } = require('imtcore-gui-testkit/pages/CollectionPage');
const gui = require('imtcore-gui-testkit/lib/gui');

const FILTERS = {
  creationDate: 'CreationDateFilter',
  documentState: 'DocumentState',
};

class AccountCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'Accounts');
  }

  async setCreationDate(preset) {
    await this.filters.dateFilter(FILTERS.creationDate, preset);
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

module.exports = { AccountCollectionPage, FILTERS };
