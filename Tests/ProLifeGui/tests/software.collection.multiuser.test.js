// Software (SoftwareProducts) COLLECTION view - full functional coverage, multi-user.
//
// Runs once per user-project; screenshots auto-separate per user (see playwright.config.js). Users
// who cannot see the Software page skip the interaction body but still record a landing screenshot,
// so the permission difference is captured. Command/field gating comes from matrix/permissions.js.

const { test } = require('../fixtures/test');
const { SoftwareCollectionPage } = require('../pages');
const { canSeePage, canRunSoftwareCommand } = require('../matrix/permissions');

const PAGE = 'SoftwareProducts';

test.describe('Software / collection', () => {
  test.beforeEach(async ({ page }) => {
    await new SoftwareCollectionPage(page).reload();
  });

  // Landing screenshot for every user (documents what each permission level sees).
  test('landing', async ({ page, gui, user }) => {
    const software = new SoftwareCollectionPage(page);
    if (canSeePage(user, PAGE)) await software.open();
    await gui.checkScreenshot(page, 'software-landing', await software.timestampColumnMasks());
  });

  // Structural gate: which commands each user's command bar exposes.
  test('command bar reflects permissions', async ({ page, user }) => {
    test.skip(!canSeePage(user, PAGE), 'user cannot see Software');
    const software = new SoftwareCollectionPage(page);
    await software.open();
    for (const cmd of ['New', 'Edit', 'Remove', 'Revision', 'Split', 'Revoke']) {
      if (canRunSoftwareCommand(user, cmd)) {
        await software.commands.expectHasCommand(cmd);
      } else {
        await software.commands.expectNoCommand(cmd);
      }
    }
  });

  // Everything below only runs for users who can open the page.
  test.describe('interactions', () => {
    test.beforeEach(async ({ page, user }) => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Software');
      await new SoftwareCollectionPage(page).open();
    });

    // --- filters (each registered filter + built-ins) ------------------------------------------
    test('filter - text search', async ({ page, gui }) => {
      const software = new SoftwareCollectionPage(page);
      await software.search('test');
      await gui.checkScreenshot(page, 'software-filter-text', await software.timestampColumnMasks());
    });

    test('filter - license status', async ({ page, gui }) => {
      const software = new SoftwareCollectionPage(page);
      await software.selectFilterOption('licenseStatus', 'Show only paired licenses');
      await gui.checkScreenshot(page, 'software-filter-license-status-paired', await software.timestampColumnMasks());
    });

    test('filter - usage (internal use)', async ({ page, gui }) => {
      const software = new SoftwareCollectionPage(page);
      await software.selectFilterOption('usage', 'For Internal Purposes');
      await gui.checkScreenshot(page, 'software-filter-usage-internal', await software.timestampColumnMasks());
    });

    test('filter - customers', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewAccounts'), 'customers filter needs ViewAccounts');
      const software = new SoftwareCollectionPage(page);
      // Org-scoped users resolve to zero customers, so the Customers filter has no "QUISS" entry - skip
      // rather than fail on a missing option (see AccountCollection's org-scoping note).
      test.skip(
        !(await software.filters.combo('CustomersFilter').hasOption('QUISS')),
        'no QUISS customer visible to this user (org-scoped)'
      );
      await software.selectFilterOption('customers', 'QUISS');
      await gui.checkScreenshot(page, 'software-filter-customer-quiss', await software.timestampColumnMasks());
    });

    test('filter - creation date preset', async ({ page, gui }) => {
      const software = new SoftwareCollectionPage(page);
      await software.setCreationDate('Year_Last');
      await gui.checkScreenshot(page, 'software-filter-creation-date', await software.timestampColumnMasks());
    });

    test('filter - clear all', async ({ page, gui }) => {
      const software = new SoftwareCollectionPage(page);
      await software.search('test');
      await software.selectFilterOption('licenseStatus', 'Show only paired licenses');
      await software.clearAllFilters();
      await gui.checkScreenshot(page, 'software-filter-cleared', await software.timestampColumnMasks());
    });

    // --- sorting -------------------------------------------------------------------------------
    // Table.sortBy() addresses columns by their objectName, which is the HeaderIds entry (the field
    // key), NOT the visible HeaderNames caption - the two lists are independently ordered in
    // SoftwareProductsPage.acc. "Name" (caption) -> "licenseName" (id).
    test('sort by name column', async ({ page, gui }) => {
      const software = new SoftwareCollectionPage(page);
      await software.table.sortBy('licenseName');
      await gui.checkScreenshot(page, 'software-sort-name-1', await software.timestampColumnMasks());
      await software.table.sortBy('licenseName');
      await gui.checkScreenshot(page, 'software-sort-name-2', await software.timestampColumnMasks());
    });

    // --- pagination ----------------------------------------------------------------------------
    test('pagination - page size and navigation', async ({ page, gui }) => {
      const software = new SoftwareCollectionPage(page);
      await software.pagination.setPageSize(50);
      await gui.checkScreenshot(page, 'software-pagination-50', await software.timestampColumnMasks());
      // Navigate to page 2 only if the collection actually spans more than one page at this size -
      // a collection that fits on a single page legitimately has no page-2 button (data-adaptive).
      if (await software.pagination.hasPage(2)) {
        await software.pagination.goToPage(2);
        await gui.checkScreenshot(page, 'software-pagination-page-2', await software.timestampColumnMasks());
      }
    });

    // --- row selection + context-sensitive commands --------------------------------------------
    test('revision dialog', async ({ page, gui, user }) => {
      test.skip(!canRunSoftwareCommand(user, 'Revision'), 'no Revision permission');
      const software = new SoftwareCollectionPage(page);
      test.skip(!(await software.table.hasRows()), 'collection is empty for this user');
      await software.selectRow(0);
      await software.revision();
      await gui.checkScreenshot(page, 'software-revision-dialog', await software.timestampColumnMasks());
      await gui.dismissDialog(page);
    });

    test('remove confirmation dialog', async ({ page, gui, user }) => {
      test.skip(!canRunSoftwareCommand(user, 'Remove'), 'no Remove permission');
      const software = new SoftwareCollectionPage(page);
      test.skip(!(await software.table.hasRows()), 'collection is empty for this user');
      await software.selectRow(0);
      await software.removeItem();
      await gui.checkScreenshot(page, 'software-remove-dialog', await software.timestampColumnMasks());
      await gui.dismissDialog(page);
    });

    test('split dialog', async ({ page, gui, user }) => {
      test.skip(!canRunSoftwareCommand(user, 'Split'), 'no Split permission');
      const software = new SoftwareCollectionPage(page);
      test.skip(!(await software.table.hasRows()), 'collection is empty for this user');
      await software.selectRow(0);
      await software.split();
      await gui.checkScreenshot(page, 'software-split-dialog', await software.timestampColumnMasks());
      await gui.dismissDialog(page);
    });

    test('revoke dialog', async ({ page, gui, user }) => {
      test.skip(!canRunSoftwareCommand(user, 'Revoke'), 'no Revoke permission');
      const software = new SoftwareCollectionPage(page);
      test.skip(!(await software.table.hasRows()), 'collection is empty for this user');
      await software.selectRow(0);
      await software.revoke();
      await gui.checkScreenshot(page, 'software-revoke-dialog', await software.timestampColumnMasks());
      await gui.dismissDialog(page);
    });
  });
});
