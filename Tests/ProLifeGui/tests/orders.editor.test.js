// Orders EDITOR - full functional coverage.
//
// Both describe blocks below are `.serial` and share ONE page/document across all their steps (see
// fixtures/test.js's newUserPage) instead of a fresh page+reload per test: "fill a field, then another,
// then save" is already a deliberate narrative sequence, and a fresh editor per test would pay a full
// navigate+reload+open cost for every single field checked - most of which exercises the exact same
// TextInput fill/verify mechanism. Trade-off: a failure partway through a block skips the remaining
// steps in that block.

const { test, expect, newUserPage } = require('../fixtures/test');
const { OrderCollectionPage, OrderEditorPage } = require('../pages');
const gui = require('imtcore-gui-testkit/lib/gui');

// Returns null when this user cannot get to a new-order editor at all - the page is not in their menu,
// or the collection offers no New command. The caller test.skip()s on that rather than failing.
async function openNewEditor(page) {
  const orders = new OrderCollectionPage(page);
  await orders.reload();
  if (!(await orders.isAvailable())) return null;
  await orders.open();
  if (!(await orders.commands.isAvailable('New'))) return null;
  await orders.newItem();
  return new OrderEditorPage(page);
}

async function openEditEditor(page) {
  const orders = new OrderCollectionPage(page);
  await orders.reload();
  if (!(await orders.isAvailable())) return null;
  await orders.open();
  // Sort by "added" (creation date, immutable) before picking row 0 - the default (unsorted) view's
  // row 0 is whichever order the server currently orders first, and that shifts whenever a Save changes
  // an order's Last Modified timestamp (including this file's OWN "edit fields and save" test, which
  // reopens this same helper afterwards to verify persistence) - see the identical, more detailed note
  // in devices.editor.test.js's openEditEditor.
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
  if (!(await orders.isAvailable())) return null;
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
    let page, editor;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page } = await newUserPage(browser, testInfo));
      editor = await openNewEditor(page);
    });

    test.afterAll(async () => {
      if (page) await page.context().close();
    });

    test.beforeEach(() => {
      test.skip(!editor, 'creating an order is not available to this user');
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
    // per-entity logic - covered once for the whole suite in devices.editor.test.js.
  });

  // Products section: ProductEditorDialog (add/edit a product line) + OrderProductDelegate rows
  // (Edit/Remove per row) + the single toolbar Expand toggle (all rows at once, no per-row expand).
  // On a NEW (unsaved) order, the "Add product" button is gated by AddOrder rather than
  // ChangeOrderProducts (OrderEditor.qml) - same permission this block's own tests already gate on.
  test.describe.serial('products', () => {
    let page, editor, hasCustomer;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page } = await newUserPage(browser, testInfo));
      editor = await openNewEditor(page);
      if (editor) {
        // ProductEditorDialog's OK stays disabled on an order that isn't otherwise valid (confirmed
        // live: License/Product both resolve correctly, but OK silently no-ops) - the WHOLE order's own
        // validity gates it, not just the dialog's own fields. A brand-new order has neither a valid
        // Delivery-ID nor a Customer by default, and this block only ever exercises the Products
        // section, never the "new document" block's own validation-focused tests. Customer is
        // position-based (live seeded Accounts data, no stable caption) - same convention as the
        // product catalogue combos themselves.
        await editor.setDeliveryId('12345');
        // The Customer list is built from the accounts this user's org can see, and for an org-scoped
        // user it resolves to none - the combo then never opens a popup at all (ComboBox.qml returns
        // early on an empty model), which surfaced as "did not open its popup after 3 attempts" in
        // beforeAll and took the whole block down. No customer to pick is a data state, not a fault.
        hasCustomer = (await editor.customer.optionCount()) > 0;
        if (hasCustomer) await editor.setCustomerByIndex(0);
      }
    });

    test.afterAll(async () => {
      if (page) await page.context().close();
    });

    test.beforeEach(() => {
      test.skip(!editor, 'creating an order is not available to this user');
      // Adding a product needs an otherwise-valid order, and that needs a customer.
      test.skip(!hasCustomer, 'no customer is visible to this user, so no valid order can be built');
    });

    test('add a new product', async () => {
      const before = await editor.productCount();
      const added = await editor.addProduct('software', '12.10177');
      test.skip(!added, 'the seeded 12.10177 software product has no linkable license');
      await expect
        .poll(() => editor.productCount(), { message: 'expected a new product row to appear' })
        // Greater than, not exactly one more: linking an existing instance can bring several lines in
        // at once (confirmed live - one pick added four), and what this test is about is that a line
        // was really added, not how the server expands the link.
        .toBeGreaterThan(before);
      await expect(editor.productRowByCategory('software')).toHaveScreenshot('orders-editor-product-added.png', {
        threshold: 0.05,
        maxDiffPixels: 0,
      });
    });

    // Hardware follows the exact same shape as Software - the same [Software|Hardware] toggle and the
    // same instance picker, just listing devices instead of licenses.
    test('add a new hardware product', async () => {
      const before = await editor.productCount();
      const added = await editor.addProduct('hardware', '60.11226');
      test.skip(!added, 'the seeded 60.11226 hardware product has no linkable device');
      await expect
        .poll(() => editor.productCount(), { message: 'expected a new product row to appear' })
        // Greater than, not exactly one more: linking an existing instance can bring several lines in
        // at once (confirmed live - one pick added four), and what this test is about is that a line
        // was really added, not how the server expands the link.
        .toBeGreaterThan(before);
      const hardwareRow = editor.productRowByCategory('hardware');
      await expect(hardwareRow).toHaveScreenshot('orders-editor-hardware-product-added.png', {
        mask: [hardwareRow.getByText(/^([0-9a-f]{2}:){5}[0-9a-f]{2}$/i).locator('..')],
        threshold: 0.05,
        maxDiffPixels: 0,
      });
    });

    // Re-opens the row "add a new product" left behind. A LINKED line is read-only in this dialog
    // (ProductEditor.qml locks the chrome while editing, so the instance picker is gone and the nested
    // editor is a viewer) - there is no license left to re-pick, so what this proves is that the row's
    // own Edit command opens the dialog on that line and the primary button closes it again.
    test('open an existing product row, then close it again', async () => {
      test.skip((await editor.productCount()) === 0, 'no product row available (see "add a new product" above)');
      await editor.editProductRowByCategory('hardware');
      await gui.expectVisible(page, ['ProductCategorySegmented'], 'the product dialog should open on this row');
      const dialog = page.locator('[objectName="Dialog"][visible]').first();
      await expect(dialog).toHaveScreenshot('orders-editor-product-edit-dialog-open.png', {
        mask: [dialog.getByText(/^([0-9a-f]{2}:){5}[0-9a-f]{2}$/i).locator('..')],
        threshold: 0.05,
        maxDiffPixels: 0,
      });
      // Closed with Cancel, not the primary button: a linked line is read-only here (the chrome is
      // locked while editing), so there is nothing for the primary button to apply and it stays put.
      await editor.cancelProductDialog();
      await gui.expectHidden(page, ['ProductCategorySegmented'], 'the product dialog should close again');
      await gui.checkScreenshot(page, 'orders-editor-product-edited', await editor.productMasks());
    });

    test('collapse product view (compact card)', async () => {
      await editor.toggleProductsExpanded();
      await expect(editor.productRowByCategory('hardware')).toHaveScreenshot('orders-editor-product-expanded.png', {
        threshold: 0.05,
        maxDiffPixels: 0,
      });
      await editor.toggleProductsExpanded(); // back to detailed for the remaining steps
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
        // Greater than, not exactly one more: linking an existing instance can bring several lines in
        // at once (confirmed live - one pick added four), and what this test is about is that a line
        // was really added, not how the server expands the link.
        .toBeGreaterThan(before);

      await editor.removeProductRow(0);
      await gui.checkScreenshot(page, 'orders-editor-product-remove-confirm', await editor.productMasks());
      await gui.clickButton(page, ['YesButton']);
      await gui.checkScreenshot(page, 'orders-editor-product-removed', await editor.productMasks());
    });
  });

  test.describe.serial('edit document', () => {
    let page, editor;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page } = await newUserPage(browser, testInfo));
      // A CLOSED order cannot be saved at all - the server refuses the whole update ("Product ... cannot
      // be inserted to collection for order", surfaced as an Internal error toast), so editing row 0 was
      // testing a flow the product does not support. Pick a non-Closed one, as the products test below
      // already does.
      editor = await openEditableOrderEditor(page);
    });

    test.afterAll(async () => {
      if (page) await page.context().close();
    });

    test.beforeEach(() => {
      test.skip(!editor, 'no open order is available to this user to edit');
    });

    test('open existing order editor', async () => {
      await gui.checkScreenshot(page, 'orders-editor-edit-loaded');
    });

    test('edit fields and save', { tag: '@mutating' }, async () => {
      // Closing the document and booting the collection again to reopen it is a second full app
      // round-trip on top of this test's own edits - more than the suite-wide 60s cap allows.
      test.setTimeout(150_000);
      // Keyed to the user, not to the clock: this value is typed into a field that several screenshots
      // then capture, so a per-run number guarantees they never match their baseline. Per-user keeps it
      // unique between projects, which the edit needs - re-typing the SAME value changes nothing and
      // leaves the document clean, and then there is nothing for Save to commit.
      const edited = `Edited by ProLifeGui (${test.info().project.name})`;
      // Read before editing: this is what relocates the SAME order after the reopen below.
      // Back to General first: adding the product left the editor on its Products sub-page, where this
      // field is not visible - and an unreadable field now answers null rather than hanging, which
      // reached the search as a non-string.
      await editor.openEditorPage('General');
      const deliveryId = await editor.deliveryId.value();
      expect(deliveryId, 'the order needs a Delivery-ID to be found again after reopening').toBeTruthy();
      await editor.setDescription(edited);
      await gui.checkScreenshot(page, 'orders-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'orders-editor-edit-saved');

      // Persistence check: a Save that only LOOKS successful in the client's own state (but never
      // actually round-tripped/committed server-side) would still pass every assertion above - closing
      // and reopening the SAME document from a clean collection reload proves the new value was really
      // written, not just held in this still-open document's in-memory representation.
      await editor.closeDocument();
      // Relocate THIS order by its Delivery-ID rather than by row position: the Save just changed its
      // Last Modified, and openEditableOrderEditor scans in whatever order the collection is currently
      // in, so "the first non-Closed row" need not be the order this test just edited.
      const orders = new OrderCollectionPage(page);
      await orders.reload();
      await orders.open();
      await orders.search(deliveryId);
      await orders.selectRow(0);
      await orders.editItem();
      editor = new OrderEditorPage(page);
      await editor.description.waitForValue(edited);
      await gui.checkScreenshot(page, 'orders-editor-edit-reopened');
    });

    // Adding a product to an EXISTING (already-saved) order is a genuinely different code path from
    // the 'products' describe block above, which works on a brand-new unsaved document - previously
    // never exercised against a real, persisted order.
    test('add a product to an existing order and save persists it', { tag: '@mutating' }, async () => {
      // The longest flow in the suite: a product dialog, a save, then a full close-reload-search-reopen
      // round trip - two complete app boots on top of everything else.
      test.setTimeout(240_000);
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
        // Greater than, not exactly one more: linking an existing instance can bring several lines in
        // at once (confirmed live - one pick added four), and what this test is about is that a line
        // was really added, not how the server expands the link.
        .toBeGreaterThan(before);
      await gui.checkScreenshot(page, 'orders-editor-existing-order-product-added', await editor.productMasks());

      // Delivery-ID uniquely identifies this specific order (unlike openEditEditor's "row 0 sorted by
      // added", which has no way to target the SAME non-Closed order this test deliberately searched
      // for above) - read it now, before closing, so the reopen step below can search for it directly.
      // Back to General first: adding the product left the editor on its Products sub-page, where this
      // field is not visible - and an unreadable field answers null, which reached the search as a
      // non-string ("keyboard.type: expected string, got object").
      await editor.openEditorPage('General');
      const deliveryId = await editor.deliveryId.value();
      expect(deliveryId, 'the order needs a Delivery-ID to be found again after reopening').toBeTruthy();

      await editor.save();
      await gui.checkScreenshot(page, 'orders-editor-existing-order-product-saved', await editor.productMasks());

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
        .toBeGreaterThan(before);
      await gui.checkScreenshot(page, 'orders-editor-existing-order-product-reopened', await editor.productMasks());
    });
  });
});
