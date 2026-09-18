// OrderCollectionPage - the Orders page collection view.
//
// Grounded in ProLife/Qml/Include/prolifeqml/OrderCollectionView.qml.
// "Added"/"Last Modified" (OrdersPage.acc's HeaderIds "added"/"timeStamp") change over time and after
// edits, so they are masked out of every screenshot of this collection rather than asserted on.

const { CollectionPage } = require('imtcore-gui-testkit/pages/CollectionPage');

const FILTERS = {
  customers: 'CustomersFilter',
  creationDate: 'CreationDateFilter',
};

class OrderCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'Orders', { filters: FILTERS, maskColumns: ['added', 'timeStamp'] });
  }
}

module.exports = { OrderCollectionPage, FILTERS };
