// Orders COLLECTION view - declared, not written. Everything structural (shared page, per-user skip,
// filter reset between tests, screenshot masking) comes from the kit's collection spec.

const fixtures = require('../fixtures/test');
const { defineCollectionSpec } = require('imtcore-gui-testkit/specs/collectionSpec');
const { OrderCollectionPage } = require('../pages');

defineCollectionSpec({ ...fixtures, defineTest: (t, b) => fixtures.test(t, b) }, {
  title: 'Orders / collection',
  pageId: 'Orders',
  requires: 'ViewOrders',
  prefix: 'orders',
  createPage: (page) => new OrderCollectionPage(page),
  scenarios: [
    { name: 'filter-text', title: 'filter - text search', search: 'test' },
    { name: 'filter-customer', title: 'filter - customers', filter: 'customers', option: 'QUISS', optional: true },
    { name: 'filter-creation-date', title: 'filter - creation date preset', dateFilter: 'creationDate', preset: 'Year_Last' },
    { name: 'filter-cleared', title: 'filter - clear all', clearAll: true, apply: [{ search: 'test' }] },
    // sortBy() addresses columns by header id, not by the visible caption: Orders has no "name" column
    // at all (HeaderIds: status/orderId/purchaseId/description/customerLink/added/timeStamp) -
    // "orderId" is the id behind the "Delivery-ID" caption.
    { name: 'sort-delivery-id', title: 'sort by delivery-id column', sort: 'orderId' },
    { name: 'pagination', title: 'pagination - page size and navigation', pagination: { size: 50, page: 2 } },
    { name: 'revision-dialog', title: 'revision dialog', command: 'Revision', requires: 'ViewRevisions' },
    { name: 'remove-dialog', title: 'remove confirmation dialog', command: 'Remove', requires: 'RemoveOrder' },
  ],
});
