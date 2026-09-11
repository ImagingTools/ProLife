// Accounts COLLECTION view - declared, not written (see the kit's collection spec).
//
// Account (customer) rows are org-scoped: the specialist roles can open the page but their org
// resolves to ZERO rows. Row-driven scenarios skip on an empty collection by themselves, and the
// customer collection is small enough that it never spans more than one page, which the pagination
// scenario is also adaptive about.

const fixtures = require('../fixtures/test');
const { defineCollectionSpec } = require('imtcore-gui-testkit/specs/collectionSpec');
const { AccountCollectionPage } = require('../pages');

// defineTest must be declared HERE - Playwright files a test under the source location that called
// test(), and that decides where snapshotPathTemplate puts this spec's baselines.
defineCollectionSpec({ ...fixtures, defineTest: (title, body) => fixtures.test(title, body) }, {
  title: 'Accounts / collection',
  pageId: 'Accounts',
  prefix: 'accounts',
  createPage: (page) => new AccountCollectionPage(page),
  scenarios: [
    { name: 'filter-text', title: 'filter - text search', search: 'test' },
    { name: 'filter-creation-date', title: 'filter - creation date preset', dateFilter: 'creationDate', preset: 'Year_Last' },
    { name: 'filter-cleared', title: 'filter - clear all', clearAll: true, apply: [{ search: 'test' }] },
    { name: 'sort-name', title: 'sort by name column', sort: 'name' },
    { name: 'pagination', title: 'pagination - page size and navigation', pagination: { size: 50, page: 2 } },
    { name: 'remove-dialog', title: 'remove confirmation dialog', command: 'Remove' },
  ],
});
