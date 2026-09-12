// Software (SoftwareProducts) COLLECTION view - declared, not written (see the kit's collection spec).

const fixtures = require('../fixtures/test');
const { defineCollectionSpec } = require('imtcore-gui-testkit/specs/collectionSpec');
const { SoftwareCollectionPage } = require('../pages');

defineCollectionSpec({ ...fixtures, defineTest: (...args) => fixtures.test(...args) }, {
  title: 'Software / collection',
  pageId: 'SoftwareProducts',
  requires: 'ViewLicenses',
  prefix: 'software',
  createPage: (page) => new SoftwareCollectionPage(page),
  scenarios: [
    { name: 'filter-text', title: 'filter - text search', search: 'test' },
    { name: 'filter-license-status-paired', title: 'filter - license status', filter: 'licenseStatus', option: 'Show only paired licenses' },
    { name: 'filter-usage-internal', title: 'filter - usage (internal use)', filter: 'usage', option: 'For Internal Purposes' },
    { name: 'filter-customer-quiss', title: 'filter - customers', filter: 'customers', option: 'QUISS', optional: true },
    { name: 'filter-creation-date', title: 'filter - creation date preset', dateFilter: 'creationDate', preset: 'Year_Last' },
    {
      name: 'filter-cleared',
      title: 'filter - clear all',
      clearAll: true,
      apply: [{ search: 'test' }, { filter: 'licenseStatus', option: 'Show only paired licenses' }],
    },
    // sortBy() addresses columns by header id, not by the visible caption: "Name" -> "licenseName".
    { name: 'sort-name', title: 'sort by name column', sort: 'licenseName' },
    // restore: this is the only collection whose later scenarios would otherwise act on page 2 at
    // size 50 - clearAllFilters() resets neither.
    { name: 'pagination', title: 'pagination - page size and navigation', pagination: { size: 50, page: 2, restore: true } },
    { name: 'revision-dialog', title: 'revision dialog', command: 'Revision', requires: 'ViewRevisions' },
    { name: 'remove-dialog', title: 'remove confirmation dialog', command: 'Remove', requires: 'RemoveLicense' },
    { name: 'split-dialog', title: 'split dialog', command: 'Split', requires: 'SplitLicense' },
    { name: 'revoke-dialog', title: 'revoke dialog', command: 'Revoke', requires: 'RevokeLicense' },
  ],
});
