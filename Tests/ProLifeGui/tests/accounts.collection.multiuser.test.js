// Accounts COLLECTION view - full functional coverage, multi-user.
//
// NOTE ON DATA VISIBILITY: account (customer) rows are org-scoped. su and fullAccess see the full
// customer list; the specialist roles (hardwareManager/licenseManager/orderManager/accountsViewer)
// hold ViewAccounts (so the page opens and command bar renders) but their org legitimately resolves
// to ZERO customer rows. Row-interaction tests therefore guard on the collection actually having a
// row for this user and skip when it is empty - selecting/opening a row that does not exist is a
// data mismatch, not a real failure. The customer collection is also small (a handful of real
// customers), so it never spans more than one page - pagination navigation is data-adaptive too.
// Accounts has no Added/Last Modified columns (HeaderIds: customerId/name/email/description), so
// there is nothing time-dependent to mask here.
//
// 'landing' and 'command bar reflects permissions' keep the default fresh-page-per-test fixture (their
// whole point is documenting the COLD load state). 'interactions' is `.serial` and shares ONE page
// opened once in beforeAll (see fixtures/test.js's newUserPage) instead of reload()-ing per test - same
// pattern as devices.collection.multiuser.test.js, applied here to cut the per-test WASM-reboot cost.
// Trade-off: a failure partway through 'interactions' skips the remaining steps in that block.

const { test, newUserPage } = require('../fixtures/test');
const { AccountCollectionPage } = require('../pages');
const { canSeePage, canRunAccountCommand } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

const PAGE = 'Accounts';

test.describe('Accounts / collection', () => {
  test.beforeEach(async ({ page }) => {
    await new AccountCollectionPage(page).reload();
  });

  test('landing', async ({ page, gui, user }) => {
    if (canSeePage(user, PAGE)) await new AccountCollectionPage(page).open();
    await gui.checkScreenshot(page, 'accounts-landing');
  });

  test('command bar reflects permissions', async ({ page, user }) => {
    test.skip(!canSeePage(user, PAGE), 'user cannot see Accounts');
    const accounts = new AccountCollectionPage(page);
    await accounts.open();
    for (const cmd of ['New', 'Edit', 'Remove']) {
      if (canRunAccountCommand(user, cmd)) {
        await accounts.commands.expectHasCommand(cmd);
      } else {
        await accounts.commands.expectNoCommand(cmd);
      }
    }
  });

  test.describe.serial('interactions', () => {
    let page, user, accounts;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      accounts = new AccountCollectionPage(page);
      await accounts.reload();
      if (canSeePage(user, PAGE)) {
        await accounts.open();
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(async () => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Accounts');
      // A prior test in this block may have left a filter/sort applied - collection view state is
      // server-persisted per user session same as document tabs (MultiDocumentCollectionView.qml).
      await accounts.clearAllFilters();
    });

    test('filter - text search', async () => {
      await accounts.search('test');
      await gui.checkScreenshot(page, 'accounts-filter-text');
    });

    test('filter - creation date preset', async () => {
      await accounts.setCreationDate('Year_Last');
      await gui.checkScreenshot(page, 'accounts-filter-creation-date');
    });

    test('filter - clear all', async () => {
      await accounts.search('test');
      await accounts.clearAllFilters();
      await gui.checkScreenshot(page, 'accounts-filter-cleared');
    });

    test('sort by name column', async () => {
      await accounts.table.sortBy('name');
      await gui.checkScreenshot(page, 'accounts-sort-name-1');
      await accounts.table.sortBy('name');
      await gui.checkScreenshot(page, 'accounts-sort-name-2');
    });

    test('pagination - page size and navigation', async () => {
      await accounts.pagination.setPageSize(50);
      await gui.checkScreenshot(page, 'accounts-pagination-50');
      // The customer collection fits on one page, so page 2 legitimately does not exist - only
      // navigate (and screenshot page 2) when the data actually spans more than one page.
      if (await accounts.pagination.hasPage(2)) {
        await accounts.pagination.goToPage(2);
        await gui.checkScreenshot(page, 'accounts-pagination-page-2');
      }
    });

    test('remove confirmation dialog', async () => {
      test.skip(!canRunAccountCommand(user, 'Remove'), 'no Remove permission');
      test.skip(!(await accounts.table.hasRows()), 'account collection is empty for this user (org-scoped)');
      await accounts.selectRow(0);
      await accounts.removeItem();
      await gui.checkScreenshot(page, 'accounts-remove-dialog');
      await gui.dismissDialog(page);
    });
  });
});
