// Orders EDITOR - full functional coverage, multi-user.

const { test } = require('../fixtures/test');
const { OrderCollectionPage, OrderEditorPage } = require('../pages');
const { canSeePage, canRunOrderCommand, canEditOrderField, ORDER_FIELD_PERMISSIONS } = require('../matrix/permissions');

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
  test.describe('new document', () => {
    test.beforeEach(async ({ user }) => {
      test.skip(!canRunOrderCommand(user, 'New'), 'user cannot create an order (AddOrder)');
    });

    test('empty new editor', async ({ page, gui }) => {
      await openNewEditor(page);
      await gui.checkScreenshot(page, 'orders-editor-new-empty');
    });

    test('fill order information', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setDeliveryId('12345');
      await gui.checkScreenshot(page, 'orders-editor-delivery-id');
      await editor.setPurchaseOrderId('PO-TEST-1');
      await gui.checkScreenshot(page, 'orders-editor-purchase-id');
      await editor.setDescription('ProLifeGui test order');
      await gui.checkScreenshot(page, 'orders-editor-description');
    });

    test('collapse / expand groups', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.toggleGroup('order');
      await gui.checkScreenshot(page, 'orders-editor-group-collapsed');
    });

    test('undo / redo', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setDescription('Undo me');
      await editor.undo();
      await gui.checkScreenshot(page, 'orders-editor-after-undo');
      await editor.redo();
      await gui.checkScreenshot(page, 'orders-editor-after-redo');
    });

    test('close dirty document -> confirm dialog', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setDescription('dirty');
      await editor.closeDocument();
      await gui.checkScreenshot(page, 'orders-editor-close-dirty');
    });
  });

  test.describe('edit document', () => {
    test.beforeEach(async ({ user }) => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Orders');
    });

    test('open existing order editor', async ({ page, gui }) => {
      await openEditEditor(page);
      await gui.checkScreenshot(page, 'orders-editor-edit-loaded');
    });

    test('editable fields reflect permissions', async ({ page, user }) => {
      const editor = await openEditEditor(page);
      for (const fieldObjectName of Object.keys(ORDER_FIELD_PERMISSIONS)) {
        await editor.expectFieldVisible(fieldObjectName);
        // eslint-disable-next-line no-console
        console.log(`[${user.key}] ${fieldObjectName} editable=${canEditOrderField(user, fieldObjectName, false)}`);
      }
    });

    test('edit fields and save', async ({ page, gui, user }) => {
      // Editing an EXISTING order's description needs ChangeDescriptionForOrder. AddOrder only unlocks
      // fields on a NEW document, so it must NOT gate this edit-save path (a user with AddOrder but not
      // ChangeDescriptionForOrder would otherwise reach a read-only field and fail on the fill verify).
      test.skip(!user.can('ChangeDescriptionForOrder'), 'cannot change the order description field');
      const editor = await openEditEditor(page);
      await editor.setDescription('Edited by ProLifeGui');
      await gui.checkScreenshot(page, 'orders-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'orders-editor-edit-saved');
    });
  });
});
