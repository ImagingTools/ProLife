// Software (SoftwareProducts) COLLECTION view - full functional coverage, multi-user.
//
// Runs once per user-project; screenshots auto-separate per user (see playwright.config.js). Users
// who cannot see the Software page skip the interaction body but still record a landing screenshot,
// so the permission difference is captured. Command/field gating comes from matrix/permissions.js.
//
// 'landing' and 'command bar reflects permissions' keep the default fresh-page-per-test fixture (their
// whole point is documenting the COLD load state). 'interactions' is `.serial` and shares ONE page
// opened once in beforeAll, exactly as devices.collection does - these tests need a clean filter panel,
// not a cold load, and clearAllFilters() gives that without a WASM reboot. This file was the largest
// remaining source of boot tax in the suite: 14 tests, every one of them a full reload.
// Trade-off: a failure partway through 'interactions' skips the remaining steps in that block.

const { test, newUserPage } = require('../fixtures/test');
const { SoftwareCollectionPage } = require('../pages');
const { canSeePage, canRunSoftwareCommand } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

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

  // Everything below only runs for users who can open the page. One shared page/session for the whole
  // block (see this file's header), reset per-test via clearAllFilters().
  test.describe.serial('interactions', () => {
    let page, user, software;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      software = new SoftwareCollectionPage(page);
      // newUserPage() only opens a blank page - nothing has navigated to the app yet.
      await software.reload();
      if (canSeePage(user, PAGE)) await software.open();
    });

    test.afterAll(async () => {
      if (page) await page.context().close();
    });

    test.beforeEach(async () => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Software');
      // A prior test in this block may have left a filter or sort applied - collection view state is
      // server-persisted per user session. clearAllFilters() covers registered filters, the built-in
      // search and sorting (CollectionViewBase.qml's onClearAllFilters), cheaply and without navigation.
      await software.clearAllFilters();
    });

    // --- filters (each registered filter + built-ins) ------------------------------------------
    test('filter - text search', async () => {
      await software.search('test');
      await gui.checkScreenshot(page, 'software-filter-text', await software.timestampColumnMasks());
    });

    test('filter - license status', async () => {
      await software.selectFilterOption('licenseStatus', 'Show only paired licenses');
      await gui.checkScreenshot(page, 'software-filter-license-status-paired', await software.timestampColumnMasks());
    });

    test('filter - usage (internal use)', async () => {
      await software.selectFilterOption('usage', 'For Internal Purposes');
      await gui.checkScreenshot(page, 'software-filter-usage-internal', await software.timestampColumnMasks());
    });

    test('filter - customers', async () => {
      test.skip(!user.can('ViewAccounts'), 'customers filter needs ViewAccounts');
      // Org-scoped users resolve to zero customers, so the Customers filter has no "QUISS" entry - skip
      // rather than fail on a missing option (see AccountCollection's org-scoping note).
      test.skip(
        !(await software.filters.combo('CustomersFilter').hasOption('QUISS')),
        'no QUISS customer visible to this user (org-scoped)'
      );
      await software.selectFilterOption('customers', 'QUISS');
      await gui.checkScreenshot(page, 'software-filter-customer-quiss', await software.timestampColumnMasks());
    });

    test('filter - creation date preset', async () => {
      await software.setCreationDate('Year_Last');
      await gui.checkScreenshot(page, 'software-filter-creation-date', await software.timestampColumnMasks());
    });

    test('filter - clear all', async () => {
      await software.search('test');
      await software.selectFilterOption('licenseStatus', 'Show only paired licenses');
      await software.clearAllFilters();
      await gui.checkScreenshot(page, 'software-filter-cleared', await software.timestampColumnMasks());
    });

    // --- sorting -------------------------------------------------------------------------------
    // Table.sortBy() addresses columns by their objectName, which is the HeaderIds entry (the field
    // key), NOT the visible HeaderNames caption - the two lists are independently ordered in
    // SoftwareProductsPage.acc. "Name" (caption) -> "licenseName" (id).
    test('sort by name column', async () => {
      await software.table.sortBy('licenseName');
      await gui.checkScreenshot(page, 'software-sort-name-1', await software.timestampColumnMasks());
      await software.table.sortBy('licenseName');
      await gui.checkScreenshot(page, 'software-sort-name-2', await software.timestampColumnMasks());
    });

    // --- pagination ----------------------------------------------------------------------------
    test('pagination - page size and navigation', async () => {
      await software.pagination.setPageSize(50);
      await gui.checkScreenshot(page, 'software-pagination-50', await software.timestampColumnMasks());
      // Navigate to page 2 only if the collection actually spans more than one page at this size -
      // a collection that fits on a single page legitimately has no page-2 button (data-adaptive).
      if (await software.pagination.hasPage(2)) {
        await software.pagination.goToPage(2);
        await gui.checkScreenshot(page, 'software-pagination-page-2', await software.timestampColumnMasks());
      }
      // Page size is NOT covered by clearAllFilters(), so on a shared page it would leak into every
      // later test in this block. Put it back so the rest of the chain sees the default.
      await software.pagination.setPageSize(25);
    });

    // --- row selection + context-sensitive commands --------------------------------------------
    test('revision dialog', async () => {
      test.skip(!canRunSoftwareCommand(user, 'Revision'), 'no Revision permission');
      test.skip(!(await software.table.hasRows()), 'collection is empty for this user');
      await software.selectRow(0);
      await software.revision();
      await gui.checkScreenshot(page, 'software-revision-dialog', await software.timestampColumnMasks());
      await gui.dismissDialog(page);
    });

    test('remove confirmation dialog', async () => {
      test.skip(!canRunSoftwareCommand(user, 'Remove'), 'no Remove permission');
      test.skip(!(await software.table.hasRows()), 'collection is empty for this user');
      await software.selectRow(0);
      await software.removeItem();
      await gui.checkScreenshot(page, 'software-remove-dialog', await software.timestampColumnMasks());
      await gui.dismissDialog(page);
    });

    test('split dialog', async () => {
      test.skip(!canRunSoftwareCommand(user, 'Split'), 'no Split permission');
      test.skip(!(await software.table.hasRows()), 'collection is empty for this user');
      await software.selectRow(0);
      await software.split();
      await gui.checkScreenshot(page, 'software-split-dialog', await software.timestampColumnMasks());
      await gui.dismissDialog(page);
    });

    test('revoke dialog', async () => {
      test.skip(!canRunSoftwareCommand(user, 'Revoke'), 'no Revoke permission');
      test.skip(!(await software.table.hasRows()), 'collection is empty for this user');
      await software.selectRow(0);
      await software.revoke();
      await gui.checkScreenshot(page, 'software-revoke-dialog', await software.timestampColumnMasks());
      await gui.dismissDialog(page);
    });
  });
});
