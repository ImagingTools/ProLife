// Orders COLLECTION view - full functional coverage, multi-user.

const { test } = require('../fixtures/test');
const { OrderCollectionPage } = require('../pages');
const { canSeePage, canRunOrderCommand } = require('../matrix/permissions');

const PAGE = 'Orders';

test.describe('Orders / collection', () => {
  test.beforeEach(async ({ page }) => {
    await new OrderCollectionPage(page).reload();
  });

  test('landing', async ({ page, gui, user }) => {
    if (canSeePage(user, PAGE)) await new OrderCollectionPage(page).open();
    await gui.checkScreenshot(page, 'orders-landing');
  });

  test('command bar reflects permissions', async ({ page, user }) => {
    test.skip(!canSeePage(user, PAGE), 'user cannot see Orders');
    const orders = new OrderCollectionPage(page);
    await orders.open();
    for (const cmd of ['New', 'Edit', 'Remove', 'Revision']) {
      if (canRunOrderCommand(user, cmd)) {
        await orders.commands.expectHasCommand(cmd);
      } else {
        await orders.commands.expectNoCommand(cmd);
      }
    }
  });

  test.describe('interactions', () => {
    test.beforeEach(async ({ page, user }) => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Orders');
      await new OrderCollectionPage(page).open();
    });

    test('filter - text search', async ({ page, gui }) => {
      await new OrderCollectionPage(page).search('test');
      await gui.checkScreenshot(page, 'orders-filter-text');
    });

    test('filter - customers', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewAccounts'), 'customers filter needs ViewAccounts');
      await new OrderCollectionPage(page).selectFilterOption('customers', 'QUISS');
      await gui.checkScreenshot(page, 'orders-filter-customer');
    });

    test('filter - creation date preset', async ({ page, gui }) => {
      await new OrderCollectionPage(page).setCreationDate('Year_Last');
      await gui.checkScreenshot(page, 'orders-filter-creation-date');
    });

    test('filter - clear all', async ({ page, gui }) => {
      const orders = new OrderCollectionPage(page);
      await orders.search('test');
      await orders.clearAllFilters();
      await gui.checkScreenshot(page, 'orders-filter-cleared');
    });

    test('sort by name column', async ({ page, gui }) => {
      const orders = new OrderCollectionPage(page);
      await orders.table.sortBy('name');
      await gui.checkScreenshot(page, 'orders-sort-name-1');
      await orders.table.sortBy('name');
      await gui.checkScreenshot(page, 'orders-sort-name-2');
    });

    test('pagination - page size and navigation', async ({ page, gui }) => {
      const orders = new OrderCollectionPage(page);
      await orders.pagination.setPageSize(50);
      await gui.checkScreenshot(page, 'orders-pagination-50');
      await orders.pagination.goToPage(2);
      await gui.checkScreenshot(page, 'orders-pagination-page-2');
    });

    test('select first row', async ({ page, gui }) => {
      await new OrderCollectionPage(page).selectRow(0);
      await gui.checkScreenshot(page, 'orders-row-selected');
    });

    test('revision dialog', async ({ page, gui, user }) => {
      test.skip(!canRunOrderCommand(user, 'Revision'), 'no Revision permission');
      const orders = new OrderCollectionPage(page);
      await orders.selectRow(0);
      await orders.revision();
      await gui.checkScreenshot(page, 'orders-revision-dialog');
    });

    test('remove confirmation dialog', async ({ page, gui, user }) => {
      test.skip(!canRunOrderCommand(user, 'Remove'), 'no Remove permission');
      const orders = new OrderCollectionPage(page);
      await orders.selectRow(0);
      await orders.removeItem();
      await gui.checkScreenshot(page, 'orders-remove-dialog');
    });
  });
});
