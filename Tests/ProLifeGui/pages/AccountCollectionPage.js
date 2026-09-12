// AccountCollectionPage - the Accounts (Customers) page collection view.
//
// Grounded in ProLife/Qml/Include/prolifeqml/AccountCollectionView.qml. Accounts has no
// Added/Last Modified columns (HeaderIds: customerId/name/email/description), so nothing needs masking.

const { CollectionPage } = require('imtcore-gui-testkit/pages/CollectionPage');

const FILTERS = {
  creationDate: 'CreationDateFilter',
};

class AccountCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'Accounts', { filters: FILTERS });
  }
}

module.exports = { AccountCollectionPage, FILTERS };
