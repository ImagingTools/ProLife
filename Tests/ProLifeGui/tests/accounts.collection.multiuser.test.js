// Accounts COLLECTION view - full functional coverage, multi-user.

const { test } = require('../fixtures/test');
const { AccountCollectionPage } = require('../pages');
const { canSeePage, canRunAccountCommand } = require('../matrix/permissions');

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

  test.describe('interactions', () => {
    test.beforeEach(async ({ page, user }) => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Accounts');
      await new AccountCollectionPage(page).open();
    });

    test('filter - text search', async ({ page, gui }) => {
      await new AccountCollectionPage(page).search('test');
      await gui.checkScreenshot(page, 'accounts-filter-text');
    });

    test('filter - creation date preset', async ({ page, gui }) => {
      await new AccountCollectionPage(page).setCreationDate('Year_Last');
      await gui.checkScreenshot(page, 'accounts-filter-creation-date');
    });

    test('filter - clear all', async ({ page, gui }) => {
      const accounts = new AccountCollectionPage(page);
      await accounts.search('test');
      await accounts.clearAllFilters();
      await gui.checkScreenshot(page, 'accounts-filter-cleared');
    });

    test('sort by name column', async ({ page, gui }) => {
      const accounts = new AccountCollectionPage(page);
      await accounts.table.sortBy('name');
      await gui.checkScreenshot(page, 'accounts-sort-name-1');
      await accounts.table.sortBy('name');
      await gui.checkScreenshot(page, 'accounts-sort-name-2');
    });

    test('pagination - page size and navigation', async ({ page, gui }) => {
      const accounts = new AccountCollectionPage(page);
      await accounts.pagination.setPageSize(50);
      await gui.checkScreenshot(page, 'accounts-pagination-50');
      await accounts.pagination.goToPage(2);
      await gui.checkScreenshot(page, 'accounts-pagination-page-2');
    });

    test('select first row', async ({ page, gui }) => {
      await new AccountCollectionPage(page).selectRow(0);
      await gui.checkScreenshot(page, 'accounts-row-selected');
    });

    test('remove confirmation dialog', async ({ page, gui, user }) => {
      test.skip(!canRunAccountCommand(user, 'Remove'), 'no Remove permission');
      const accounts = new AccountCollectionPage(page);
      await accounts.selectRow(0);
      await accounts.removeItem();
      await gui.checkScreenshot(page, 'accounts-remove-dialog');
    });
  });
});
