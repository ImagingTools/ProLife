// SoftwareCollectionPage - the Software (SoftwareProducts) page collection view.
//
// Grounded in ProLife/Qml/Include/prolifeqml/SoftwareProductCollectionView.qml.
// "Added"/"Last Modified" (HeaderIds "added"/"timeStamp") change over time and after edits, so they
// are masked out of every screenshot of this collection rather than asserted on.

const { CollectionPage } = require('imtcore-gui-testkit/pages/CollectionPage');

const FILTERS = {
  licenseStatus: 'LicenseStatusFilter',
  usage: 'UsageFilter',
  products: 'ProductsFilter',
  licenses: 'LicensesFilter',
  customers: 'CustomersFilter',
  licenseCreationDate: 'LicenseCreationDateFilter',
  creationDate: 'CreationDateFilter',
};

class SoftwareCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'SoftwareProducts', { filters: FILTERS, maskColumns: ['added', 'timeStamp'] });
  }
}

module.exports = { SoftwareCollectionPage, FILTERS };
