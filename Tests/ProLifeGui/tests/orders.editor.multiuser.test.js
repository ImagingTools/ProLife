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
  // Sort by "added" (creation date, immutable) before picking row 0 - the default (unsorted) view's
  // row 0 is whichever order the server currently orders first, and that shifts whenever a Save changes
  // an order's Last Modified timestamp (including this file's OWN "edit fields and save" test, which
  // reopens this same helper afterwards to verify persistence) - see the identical, more detailed note
  // in devices.editor.multiuser.test.js's openEditEditor.
  await orders.table.sortBy('added');
  await orders.selectRow(0);
  await orders.editItem();
  return new OrderEditorPage(page);
}

/**
 * Like openEditEditor(), but scans for a row whose Status is NOT "Closed" instead of blindly taking
 * row 0 - a Closed order's ProductEditorDialog leaves OK permanently disabled (confirmed live: the
 * dialog otherwise looks completely normal, License/Product both correctly resolved, but the click
 * silently no-ops), which "add a product to an existing order" needs to avoid. Returns null if every
 * visible row happens to be Closed (a data state, not a failure - the caller should test.skip()).
 */
async function openEditableOrderEditor(page) {
  const orders = new OrderCollectionPage(page);
  await orders.reload();
  await orders.open();
  const rows = page.locator('[objectName^="TableRow_"][visible]');
  const rowCount = await rows.count();
  for (let i = 0; i < rowCount; i++) {
    // eslint-disable-next-line no-await-in-loop
    const rowText = await rows.nth(i).textContent();
    if (!rowText.includes('Closed')) {
      // eslint-disable-next-line no-await-in-loop
      await orders.selectRow(i);
      // eslint-disable-next-line no-await-in-loop
      await orders.editItem();
      return new OrderEditorPage(page);
    }
  }
  return null;
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

    // OrderEditor.qml validates Delivery-ID INLINE and in real time (instanceIdInput's own
    // Component.onCompleted -> validate(), re-run on every keystroke) - confirmed live this is NOT a
    // Save-triggered modal dialog (OrderValidator.qml's documentValidator/showErrorDialog path never
    // actually fires here): an empty/invalid value shows a red error message right under the field and
    // keeps the Save command disabled the whole time, so clicking a disabled Save silently does
    // nothing. The error Text itself carries no objectName, so this asserts on its visible TEXT
    // (Playwright's own text locator) rather than a structural path.
    test('save blocked - invalid delivery-id', async () => {
      await page.getByText('Enter a 5-digit or 8-digit number').waitFor({ state: 'visible', timeout: 10000 });
      await gui.checkScreenshot(page, 'orders-editor-save-blocked-invalid-delivery-id');
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

    // Delivery-ID is now valid (set above) but Customer is still unset - exercises the Customer combo's
    // OWN inline validation (isSelectionRequired/errorText on customerCB - same real-time, no-modal-
    // dialog mechanism as the Delivery-ID check above, not OrderValidator.qml's Save-triggered path).
    test('save blocked - customer required', async () => {
      await page.getByText('Please select a customer').waitFor({ state: 'visible', timeout: 10000 });
      await gui.checkScreenshot(page, 'orders-editor-save-blocked-no-customer');
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
        // ProductEditorDialog's OK stays disabled on an order that isn't otherwise valid (confirmed
        // live: License/Product both resolve correctly, but OK silently no-ops) - the WHOLE order's own
        // validity gates it, not just the dialog's own fields. A brand-new order has neither a valid
        // Delivery-ID nor a Customer by default, and this block only ever exercises the Products
        // section, never the "new document" block's own validation-focused tests. Customer is
        // position-based (live seeded Accounts data, no stable caption) - same convention as the
        // product catalogue combos themselves.
        await editor.setDeliveryId('12345');
        await editor.setCustomerByIndex(0);
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
      const added = await editor.addAnyProduct('software');
      test.skip(!added, 'no seeded software product in this catalogue currently has a linkable license');
      await expect
        .poll(() => editor.productCount(), { message: 'expected a new product row to appear' })
        .toBe(before + 1);
      await gui.checkScreenshot(page, 'orders-editor-product-added');
    });

    // Hardware follows the exact same shape as Software (an existing-device link via
    // HardwareProductEditor.qml's "ProductDeviceCombo", mirroring "ProductLicenseCombo") - previously
    // completely unexercised by any test (addProduct() used to skip the device combo entirely for
    // 'hardware', which would have silently left OK disabled - see OrderEditorPage.js's own comment).
    test('add a new hardware product', async () => {
      const before = await editor.productCount();
      const added = await editor.addAnyProduct('hardware');
      test.skip(!added, 'no seeded hardware product in this catalogue currently has a linkable device');
      await expect
        .poll(() => editor.productCount(), { message: 'expected a new product row to appear' })
        .toBe(before + 1);
      await gui.checkScreenshot(page, 'orders-editor-hardware-product-added');
    });

    // A genuine EDIT (not just Cancel, see the test below): re-opens the row "add a new product" left
    // behind, re-selects a license (proving the combo and OK are both live/interactive on an EXISTING
    // row, not only a freshly-added one), and confirms via OK rather than discarding.
    test('edit existing product row, change license, then OK applies it', async () => {
      test.skip((await editor.productCount()) === 0, 'no product row available (see "add a new product" above)');
      await editor.editProductRow(0);
      await gui.checkScreenshot(page, 'orders-editor-product-edit-dialog-open');
      await gui.selectIndex(page, ['ProductLicenseCombo'], 0);
      await editor.confirmProductDialog();
      await gui.expectHidden(page, ['ProductLicenseCombo'], 'ProductEditorDialog should close after OK');
      await gui.checkScreenshot(page, 'orders-editor-product-edited');
    });

    test('expand product view (detailed card)', async () => {
      await editor.toggleProductsExpanded();
      await gui.checkScreenshot(page, 'orders-editor-product-expanded');
      await editor.toggleProductsExpanded(); // back to compact for the remaining steps
    });

    test('open existing product row, then Cancel discards', async () => {
      // Depends on "add a new product" above having actually added a row - that test itself
      // test.skip()s when no seeded catalogue product currently has a linkable license (a data state,
      // not a failure), which leaves nothing here to open. Check the live count rather than a shared
      // flag so this holds regardless of test order/reruns.
      test.skip((await editor.productCount()) === 0, 'no product row available (see "add a new product" above)');
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
      const added = await editor.addAnyProduct('software');
      test.skip(!added, 'no seeded software product in this catalogue currently has a linkable license');
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
      const edited = `Edited by ProLifeGui ${Date.now()}`;
      await editor.setDescription(edited);
      await gui.checkScreenshot(page, 'orders-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'orders-editor-edit-saved');

      // Persistence check: a Save that only LOOKS successful in the client's own state (but never
      // actually round-tripped/committed server-side) would still pass every assertion above - closing
      // and reopening the SAME document from a clean collection reload proves the new value was really
      // written, not just held in this still-open document's in-memory representation.
      await editor.closeDocument();
      editor = await openEditEditor(page);
      await editor.description.waitForValue(edited);
      await gui.checkScreenshot(page, 'orders-editor-edit-reopened');
    });

    // Adding a product to an EXISTING (already-saved) order is gated by ChangeOrderProducts, not
    // AddOrder (which only unlocks the Products section on a brand-new, unsaved document - see this
    // file's own header comment) - a genuinely different code path from the 'products' describe block
    // above, previously never exercised against a real, persisted order.
    test('add a product to an existing order and save persists it', { tag: '@mutating' }, async () => {
      test.skip(!user.can('ChangeOrderProducts'), 'cannot change products on an existing order');
      // NOT the block's shared `editor` (openEditEditor's "row 0 sorted by added") - a Closed order
      // rejects product changes (ProductEditorDialog's OK stays permanently disabled), so this needs
      // its own search for a non-Closed order instead. Last test in this describe.serial block, so
      // reassigning `editor` here doesn't affect anything earlier.
      await editor.closeDocument();
      editor = await openEditableOrderEditor(page);
      test.skip(!editor, 'every visible order is currently Closed - none can accept product changes');
      const before = await editor.productCount();
      const added = await editor.addAnyProduct('software');
      test.skip(!added, 'no seeded software product in this catalogue currently has a linkable license');
      await expect
        .poll(() => editor.productCount(), { message: 'expected a new product row to appear' })
        .toBe(before + 1);
      await gui.checkScreenshot(page, 'orders-editor-existing-order-product-added');

      // Delivery-ID uniquely identifies this specific order (unlike openEditEditor's "row 0 sorted by
      // added", which has no way to target the SAME non-Closed order this test deliberately searched
      // for above) - read it now, before closing, so the reopen step below can search for it directly.
      const deliveryIdInput = page.locator('[objectName="DeliveryIdInput"] input, [objectName="DeliveryIdInput"] [objectName="TextInput"]').first();
      const deliveryId = await deliveryIdInput.evaluate((el) => (el.tagName === 'INPUT' ? el.value : el.textContent)).then((v) => v.trim());

      await editor.save();
      await gui.checkScreenshot(page, 'orders-editor-existing-order-product-saved');

      // Persistence check - same reasoning as 'edit fields and save' above: close and reopen from a
      // clean collection reload, proving the product row survives a real server round-trip.
      await editor.closeDocument();
      const orders = new OrderCollectionPage(page);
      await orders.reload();
      await orders.open();
      await orders.search(deliveryId);
      await orders.selectRow(0);
      await orders.editItem();
      editor = new OrderEditorPage(page);
      await expect
        .poll(() => editor.productCount(), { message: 'expected the added product row to survive reopening' })
        .toBe(before + 1);
      await gui.checkScreenshot(page, 'orders-editor-existing-order-product-reopened');
    });
  });
});
