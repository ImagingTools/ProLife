// Orders COLLECTION view - full functional coverage, multi-user.
//
// 'landing' and 'command bar reflects permissions' keep the default fresh-page-per-test fixture (their
// whole point is documenting the COLD load state). 'interactions' is `.serial` and shares ONE page
// opened once in beforeAll (see fixtures/test.js's newUserPage) instead of reload()-ing per test - same
// pattern as devices.collection.multiuser.test.js, applied here to cut the per-test WASM-reboot cost.
// Trade-off: a failure partway through 'interactions' skips the remaining steps in that block.

const { test, newUserPage } = require('../fixtures/test');
const { OrderCollectionPage } = require('../pages');
const { canSeePage, canRunOrderCommand } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

const PAGE = 'Orders';

test.describe('Orders / collection', () => {
  test.beforeEach(async ({ page }) => {
    await new OrderCollectionPage(page).reload();
  });

  test('landing', async ({ page, gui, user }) => {
    const orders = new OrderCollectionPage(page);
    if (canSeePage(user, PAGE)) await orders.open();
    await gui.checkScreenshot(page, 'orders-landing', await orders.timestampColumnMasks());
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

  test.describe.serial('interactions', () => {
    let page, user, orders;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      orders = new OrderCollectionPage(page);
      await orders.reload();
      if (canSeePage(user, PAGE)) {
        await orders.open();
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(async () => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Orders');
      // A prior test in this block may have left a filter/sort applied - collection view state is
      // server-persisted per user session same as document tabs (MultiDocumentCollectionView.qml).
      await orders.clearAllFilters();
    });

    test('filter - text search', async () => {
      await orders.search('test');
      await gui.checkScreenshot(page, 'orders-filter-text', await orders.timestampColumnMasks());
    });

    test('filter - customers', async () => {
      test.skip(!user.can('ViewAccounts'), 'customers filter needs ViewAccounts');
      // Org-scoped users resolve to zero customers, so the Customers filter has no "QUISS" entry - skip
      // rather than fail on a missing option (see AccountCollection's org-scoping note).
      test.skip(
        !(await orders.filters.combo('CustomersFilter').hasOption('QUISS')),
        'no QUISS customer visible to this user (org-scoped)'
      );
      await orders.selectFilterOption('customers', 'QUISS');
      await gui.checkScreenshot(page, 'orders-filter-customer', await orders.timestampColumnMasks());
    });

    test('filter - creation date preset', async () => {
      await orders.setCreationDate('Year_Last');
      await gui.checkScreenshot(page, 'orders-filter-creation-date', await orders.timestampColumnMasks());
    });

    test('filter - clear all', async () => {
      await orders.search('test');
      await orders.clearAllFilters();
      await gui.checkScreenshot(page, 'orders-filter-cleared', await orders.timestampColumnMasks());
    });

    // Table.sortBy() addresses columns by their objectName, which is the HeaderIds entry (the field
    // key), NOT the visible HeaderNames caption. Orders has no "name"/"Name" column at all
    // (OrdersPage.acc's HeaderIds: status/orderId/purchaseId/description/customerLink/added/
    // timeStamp) - "orderId" is the id behind the "Delivery-ID" caption and is sortable.
    test('sort by delivery-id column', async () => {
      await orders.table.sortBy('orderId');
      await gui.checkScreenshot(page, 'orders-sort-delivery-id-1', await orders.timestampColumnMasks());
      await orders.table.sortBy('orderId');
      await gui.checkScreenshot(page, 'orders-sort-delivery-id-2', await orders.timestampColumnMasks());
    });

    test('pagination - page size and navigation', async () => {
      await orders.pagination.setPageSize(50);
      await gui.checkScreenshot(page, 'orders-pagination-50', await orders.timestampColumnMasks());
      await orders.pagination.goToPage(2);
      await gui.checkScreenshot(page, 'orders-pagination-page-2', await orders.timestampColumnMasks());
    });

    test('revision dialog', async () => {
      test.skip(!canRunOrderCommand(user, 'Revision'), 'no Revision permission');
      await orders.selectRow(0);
      await orders.revision();
      await gui.checkScreenshot(page, 'orders-revision-dialog', await orders.timestampColumnMasks());
      // This dialog stays open for the rest of the chain (shared page, no reload between tests) -
      // dismiss it so it doesn't block the next test's clicks.
      await gui.dismissDialog(page);
    });

    test('remove confirmation dialog', async () => {
      test.skip(!canRunOrderCommand(user, 'Remove'), 'no Remove permission');
      await orders.selectRow(0);
      await orders.removeItem();
      await gui.checkScreenshot(page, 'orders-remove-dialog', await orders.timestampColumnMasks());
      await gui.dismissDialog(page); // decline - must not actually remove the order
    });
  });
});
