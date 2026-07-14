// Orders EDITOR - full functional coverage, multi-user.
//
// Both describe blocks below are `.serial` and share ONE page/document across all their steps (see
// fixtures/test.js's newUserPage) instead of a fresh page+reload per test: "fill a field, then another,
// then save" is already a deliberate narrative sequence, and a fresh editor per test would pay a full
// navigate+reload+open cost for every single field checked - most of which exercises the exact same
// TextInput fill/verify mechanism. Trade-off: a failure partway through a block skips the remaining
// steps in that block.

const { test, expect, newUserPage } = require('../fixtures/test');
const { OrderCollectionPage, OrderEditorPage } = require('../pages');
const { canSeePage, canRunOrderCommand, canEditOrderField, ORDER_FIELD_PERMISSIONS } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

const PAGE = 'Orders';

async function openNewEditor(page) {
  const orders = new OrderCollectionPage(page);
  await orders.reload();
  await orders.open();
  await orders.newItem();
  return new OrderEditorPage(page);
}

async function openEditEditor(page) {
  const orders = new OrderCollectionPage(page);
  await orders.reload();
  await orders.open();
  await orders.selectRow(0);
  await orders.editItem();
  return new OrderEditorPage(page);
}

test.describe('Orders / editor', () => {
  test.describe.serial('new document', () => {
    let page, user, editor;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      if (canRunOrderCommand(user, 'New')) {
        editor = await openNewEditor(page);
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(() => {
      test.skip(!canRunOrderCommand(user, 'New'), 'user cannot create an order (AddOrder)');
    });

    test('empty new editor', async () => {
      await gui.checkScreenshot(page, 'orders-editor-new-empty');
    });

    // Delivery-ID, Purchase Order-ID and Description are all plain TextInput fields with the identical
    // fill/verify mechanism - one combined screenshot documents all three without paying for three
    // separate fill+screenshot passes.
    test('fill order information', async () => {
      await editor.setDeliveryId('12345');
      await editor.setPurchaseOrderId('PO-TEST-1');
      await editor.setDescription('ProLifeGui test order');
      await gui.checkScreenshot(page, 'orders-editor-new-filled');
    });

    test('assign end customer', async () => {
      await editor.setEndCustomerByIndex(0);
      await editor.expectFieldVisible('EndCustomerCombo');
    });

    // Group-collapse/undo-redo/dirty-close-confirm are generic Document/UI mechanics with no
    // per-entity logic - covered once for the whole suite in devices.editor.multiuser.test.js.
  });

  // Products section: ProductEditorDialog (add/edit a product line) + OrderProductDelegate rows
  // (Edit/Remove per row) + the single toolbar Expand toggle (all rows at once, no per-row expand).
  // On a NEW (unsaved) order, the "Add product" button is gated by AddOrder rather than
  // ChangeOrderProducts (OrderEditor.qml) - same permission this block's own tests already gate on.
  test.describe.serial('products', () => {
    let page, user, editor;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      if (canRunOrderCommand(user, 'New')) {
        editor = await openNewEditor(page);
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(() => {
      test.skip(!canRunOrderCommand(user, 'New'), 'user cannot create an order (AddOrder)');
    });

    test('add a new product', async () => {
      const before = await editor.productCount();
      await editor.addProduct('software', 0);
      await expect
        .poll(() => editor.productCount(), { message: 'expected a new product row to appear' })
        .toBe(before + 1);
      await gui.checkScreenshot(page, 'orders-editor-product-added');
    });

    test('expand product view (detailed card)', async () => {
      await editor.toggleProductsExpanded();
      await gui.checkScreenshot(page, 'orders-editor-product-expanded');
      await editor.toggleProductsExpanded(); // back to compact for the remaining steps
    });

    test('open existing product row, then Cancel discards', async () => {
      await editor.editProductRow(0);
      await gui.checkScreenshot(page, 'orders-editor-product-edit-dialog');
      await editor.cancelProductDialog();
    });

    test('remove a product row', { tag: '@mutating' }, async () => {
      // Self-contained: Run-CiTests.ps1 runs @mutating tests in their OWN phase, a separate
      // "npx playwright test" invocation that re-runs this block's beforeAll from scratch (a fresh,
      // product-less document) without also running the earlier, non-@mutating "add a new product"
      // test - so this can't rely on that test's row still being there and must add its own.
      const before = await editor.productCount();
      await editor.addProduct('software', 0);
      await expect
        .poll(() => editor.productCount(), { message: 'expected a new product row to appear' })
        .toBe(before + 1);

      await editor.removeProductRow(0);
      await gui.checkScreenshot(page, 'orders-editor-product-remove-confirm');
      await gui.clickButton(page, ['YesButton']);
      await gui.checkScreenshot(page, 'orders-editor-product-removed');
    });
  });

  test.describe.serial('edit document', () => {
    let page, user, editor;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      if (canSeePage(user, PAGE)) {
        editor = await openEditEditor(page);
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(() => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Orders');
    });

    test('open existing order editor', async () => {
      await gui.checkScreenshot(page, 'orders-editor-edit-loaded');
    });

    test('editable fields reflect permissions', async () => {
      for (const fieldObjectName of Object.keys(ORDER_FIELD_PERMISSIONS)) {
        await editor.expectFieldVisible(fieldObjectName);
        // eslint-disable-next-line no-console
        console.log(`[${user.key}] ${fieldObjectName} editable=${canEditOrderField(user, fieldObjectName, false)}`);
      }
    });

    test('edit fields and save', { tag: '@mutating' }, async () => {
      // Editing an EXISTING order's description needs ChangeDescriptionForOrder. AddOrder only unlocks
      // fields on a NEW document, so it must NOT gate this edit-save path (a user with AddOrder but not
      // ChangeDescriptionForOrder would otherwise reach a read-only field and fail on the fill verify).
      test.skip(!user.can('ChangeDescriptionForOrder'), 'cannot change the order description field');
      await editor.setDescription('Edited by ProLifeGui');
      await gui.checkScreenshot(page, 'orders-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'orders-editor-edit-saved');
    });
  });
});
