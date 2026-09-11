// Accounts EDITOR - full functional coverage, multi-user.
//
// Both describe blocks below are `.serial` and share ONE page/document across all their steps (see
// fixtures/test.js's newUserPage) instead of a fresh page+reload per test: "fill a field, then another,
// then save" is already a deliberate narrative sequence, and a fresh editor per test would pay a full
// navigate+reload+open cost for every single field checked - most of which exercises the exact same
// TextInput fill/verify mechanism. Trade-off: a failure partway through a block skips the remaining
// steps in that block.

const { test, newUserPage } = require('../fixtures/test');
const { AccountCollectionPage, AccountEditorPage } = require('../pages');
const gui = require('imtcore-gui-testkit/lib/gui');

// Returns null when this user cannot get to a new-account editor at all - the page is not in their
// menu, or the collection offers no New command. The caller test.skip()s on that rather than failing.
async function openNewEditor(page) {
  const accounts = new AccountCollectionPage(page);
  await accounts.reload();
  if (!(await accounts.isAvailable())) return null;
  await accounts.open();
  if (!(await accounts.commands.isAvailable('New'))) return null;
  await accounts.newItem();
  return new AccountEditorPage(page);
}

// Opens the EDIT editor for the first existing account. Account rows are org-scoped: the specialist
// roles can open the page but their org resolves to ZERO customers, so there is no row to edit.
// Returns null in that case too.
async function openEditEditor(page) {
  const accounts = new AccountCollectionPage(page);
  await accounts.reload();
  if (!(await accounts.isAvailable())) return null;
  await accounts.open();
  if (!(await accounts.table.hasRows())) return null;
  await accounts.selectRow(0);
  await accounts.editItem();
  return new AccountEditorPage(page);
}

test.describe('Accounts / editor', () => {
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
      test.skip(!editor, 'creating an account is not available to this user');
    });

    test('empty new editor', async () => {
      await gui.checkScreenshot(page, 'accounts-editor-new-empty');
    });

    // Customer-ID/Account Name/Description/Email (customer+account group) and Country/City/Postal
    // Code/Street (address group) are all plain TextInput fields with the identical fill/verify
    // mechanism - one combined screenshot documents all eight without paying for eight separate
    // fill+screenshot passes.
    test('fill customer, account and address information', async () => {
      await editor.setCustomerId('CUST-TEST');
      await editor.setAccountName('ProLifeGui Test Account');
      await editor.setAccountDescription('Test description');
      await editor.setEmail('test@example.com');
      await editor.setCountry('DE');
      await editor.setCity('Berlin');
      await editor.setPostalCode('10115');
      await editor.setStreet('Test Street 1');
      await gui.checkScreenshot(page, 'accounts-editor-new-filled');
    });

    // Group-collapse/undo-redo/dirty-close-confirm are generic Document/UI mechanics with no
    // per-entity logic - covered once for the whole suite in devices.editor.multiuser.test.js.
  });

  test.describe.serial('edit document', () => {
    let page, editor;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page } = await newUserPage(browser, testInfo));
      editor = await openEditEditor(page);
    });

    test.afterAll(async () => {
      if (page) await page.context().close();
    });

    test.beforeEach(() => {
      test.skip(!editor, 'no account is available to edit for this user (page or rows missing)');
    });

    test('open existing account editor', async () => {
      await gui.checkScreenshot(page, 'accounts-editor-edit-loaded');
    });

    // GroupsTable is a checkable Table (AccountEditor.qml's groupsElement) listing all groups this
    // account can belong to; checking a row applies to the in-memory model immediately (no separate
    // "apply" step - see AccountEditor.qml's onCheckedItemsChanged), but still needs Save to persist.
    test('toggle a group membership checkbox', async () => {
      test.skip((await gui.countVisible(page, ['GroupsTable', 'TableRow_0'])) === 0, 'no groups available to toggle');
      await editor.groups.toggleRowCheck(0);
      await gui.checkScreenshot(page, 'accounts-editor-group-checked');
      await editor.groups.toggleRowCheck(0); // leave unchecked for the next test
    });

    test('edit fields and save', { tag: '@mutating' }, async () => {
      const edited = `Edited by ProLifeGui ${Date.now()}`;
      await editor.setAccountName(edited);
      await gui.checkScreenshot(page, 'accounts-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'accounts-editor-edit-saved');

      // Persistence check: a Save that only LOOKS successful in the client's own state (but never
      // actually round-tripped/committed server-side) would still pass every assertion above - closing
      // and reopening the SAME document from a clean collection reload proves the new value was really
      // written, not just held in this still-open document's in-memory representation. Accounts has no
      // immutable "added" column to sort by for a stable row 0 (unlike devices/orders/software - see
      // this file's own header note), so relocate the SAME account by searching for the just-written
      // (unique, timestamped) name instead of assuming row-order stability.
      await editor.closeDocument();
      const accounts = new AccountCollectionPage(page);
      await accounts.reload();
      await accounts.open();
      await accounts.search(edited);
      await accounts.selectRow(0);
      await accounts.editItem();
      editor = new AccountEditorPage(page);
      await editor.accountName.waitForValue(edited);
      await gui.checkScreenshot(page, 'accounts-editor-edit-reopened');
    });
  });
});
