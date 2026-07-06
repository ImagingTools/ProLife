// Accounts EDITOR - full functional coverage, multi-user.

const { test } = require('../fixtures/test');
const { AccountCollectionPage, AccountEditorPage } = require('../pages');
const { canSeePage, canRunAccountCommand, canEditAccountField, ACCOUNT_FIELD_PERMISSIONS } = require('../matrix/permissions');

const PAGE = 'Accounts';

async function openNewEditor(page) {
  const accounts = new AccountCollectionPage(page);
  await accounts.reload();
  await accounts.open();
  await accounts.newItem();
  return new AccountEditorPage(page);
}

async function openEditEditor(page) {
  const accounts = new AccountCollectionPage(page);
  await accounts.reload();
  await accounts.open();
  await accounts.selectRow(0);
  await accounts.editItem();
  return new AccountEditorPage(page);
}

test.describe('Accounts / editor', () => {
  test.describe('new document', () => {
    test.beforeEach(async ({ user }) => {
      test.skip(!canRunAccountCommand(user, 'New'), 'user cannot create an account (AddAccount)');
    });

    test('empty new editor', async ({ page, gui }) => {
      await openNewEditor(page);
      await gui.checkScreenshot(page, 'accounts-editor-new-empty');
    });

    test('fill customer and account information', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setCustomerId('CUST-TEST');
      await gui.checkScreenshot(page, 'accounts-editor-customer-id');
      await editor.setAccountName('ProLifeGui Test Account');
      await gui.checkScreenshot(page, 'accounts-editor-account-name');
      await editor.setAccountDescription('Test description');
      await gui.checkScreenshot(page, 'accounts-editor-description');
      await editor.setEmail('test@example.com');
      await gui.checkScreenshot(page, 'accounts-editor-email');
    });

    test('fill company address', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setCountry('DE');
      await editor.setCity('Berlin');
      await editor.setPostalCode('10115');
      await editor.setStreet('Test Street 1');
      await gui.checkScreenshot(page, 'accounts-editor-address');
    });

    test('collapse / expand groups', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.toggleGroup('customer');
      await gui.checkScreenshot(page, 'accounts-editor-customer-group-collapsed');
      await editor.toggleGroup('account');
      await gui.checkScreenshot(page, 'accounts-editor-account-group-collapsed');
      await editor.toggleGroup('address');
      await gui.checkScreenshot(page, 'accounts-editor-address-group-collapsed');
    });

    test('undo / redo', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setAccountName('Undo me');
      await editor.undo();
      await gui.checkScreenshot(page, 'accounts-editor-after-undo');
      await editor.redo();
      await gui.checkScreenshot(page, 'accounts-editor-after-redo');
    });

    test('close dirty document -> confirm dialog', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setAccountName('dirty');
      await editor.closeDocument();
      await gui.checkScreenshot(page, 'accounts-editor-close-dirty');
    });
  });

  test.describe('edit document', () => {
    test.beforeEach(async ({ user }) => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Accounts');
    });

    test('open existing account editor', async ({ page, gui }) => {
      await openEditEditor(page);
      await gui.checkScreenshot(page, 'accounts-editor-edit-loaded');
    });

    test('editable fields reflect permissions', async ({ page, user }) => {
      const editor = await openEditEditor(page);
      for (const fieldObjectName of Object.keys(ACCOUNT_FIELD_PERMISSIONS)) {
        await editor.expectFieldVisible(fieldObjectName);
        // eslint-disable-next-line no-console
        console.log(`[${user.key}] ${fieldObjectName} editable=${canEditAccountField(user, fieldObjectName, false)}`);
      }
    });

    test('edit fields and save', async ({ page, gui, user }) => {
      // Editing an EXISTING account's name needs ChangeAccountName. AddAccount only unlocks fields on
      // a NEW document, so it must NOT gate this edit-save path (a user with AddAccount but not
      // ChangeAccountName would otherwise reach a read-only field and fail on the fill verify).
      test.skip(!user.can('ChangeAccountName'), 'cannot change the account name field');
      const editor = await openEditEditor(page);
      await editor.setAccountName('Edited by ProLifeGui');
      await gui.checkScreenshot(page, 'accounts-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'accounts-editor-edit-saved');
    });
  });
});
