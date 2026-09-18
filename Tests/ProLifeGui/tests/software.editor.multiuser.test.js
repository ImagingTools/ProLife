// Software (SoftwareProducts) EDITOR - full functional coverage, multi-user.
//
// The editor is a document tab opened by the "New" or "Edit" command. Only users who can open it
// (ViewLicenses, plus AddLicense for New) run the interaction bodies; field editability is
// permission-driven (SoftwareEditor.checkPermissions), so the per-user screenshots + the structural
// "editable fields" test encode the permission matrix.
//
// Both describe blocks below are `.serial` and share ONE page/document across all their steps (see
// fixtures/test.js's newUserPage) instead of a fresh page+reload per test: "fill a field, then another,
// then save" is already a deliberate narrative sequence, and a fresh editor per test would pay a full
// navigate+reload+open cost for every single field checked - most of which exercises the exact same
// TextInput fill/verify mechanism. Trade-off: a failure partway through a block skips the remaining
// steps in that block.

const { test, newUserPage } = require('../fixtures/test');
const { SoftwareCollectionPage, SoftwareEditorPage } = require('../pages');
const gui = require('imtcore-gui-testkit/lib/gui');

// Returns null when this user cannot get to a new-license editor at all - the page is not in their
// menu, or the collection offers no New command. The caller test.skip()s on that rather than failing.
async function openNewEditor(page) {
  const software = new SoftwareCollectionPage(page);
  await software.reload();
  if (!(await software.isAvailable())) return null;
  await software.open();
  if (!(await software.commands.isAvailable('New'))) return null;
  await software.newItem(); // "New" -> fresh document tab
  return new SoftwareEditorPage(page);
}

async function openEditEditor(page) {
  const software = new SoftwareCollectionPage(page);
  await software.reload();
  if (!(await software.isAvailable())) return null;
  await software.open();
  // Sort by "added" (creation date, header id "added" - SoftwareProductsPage.acc) before picking row 0:
  // the default (unsorted) view's row 0 is whichever license the server currently orders first, and
  // that ordering shifts whenever ANOTHER test mutates a license's Last Modified timestamp - which
  // several @mutating tests elsewhere in this run do, and this phase runs serially after them. Without
  // a stable sort, row 0 here can silently become a DIFFERENT license than whichever one the baseline
  // screenshots were captured against, showing as every single field mismatching (confirmed live on the
  // devices equivalent of this same helper). "added" is immutable creation metadata none of those
  // mutations touch, so sorting by it pins row 0 to the same license regardless of what ran earlier.
  await software.table.sortBy('added');
  await software.selectRow(0);
  await software.editItem(); // "Edit" -> loads selected doc via SoftwareProductItem
  return new SoftwareEditorPage(page);
}

test.describe('Software / editor', () => {
  // --- NEW editor: one continuous document, steps build on each other in order ------------------
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
      test.skip(!editor, 'creating a license is not available to this user');
    });

    test('empty new editor', async () => {
      await gui.checkScreenshot(page, 'software-editor-new-empty');
    });

    // Product/License/Software-ID live on the General MultiPageView page and each validate INLINE
    // (isSelectionRequired-style checks) - confirmed live this is NOT a Save-triggered modal dialog
    // (LicenseValidator.qml's documentValidator/showErrorDialog path never actually fires here): each
    // missing required field shows its own red error message right under it and keeps the Save command
    // disabled the whole time, so clicking a disabled Save silently does nothing. The error Text itself
    // carries no objectName, so this asserts on visible TEXT (Playwright's own text locator) for all
    // three, rather than a structural path.
    test('save blocked - missing required fields', async () => {
      await editor.openEditorPage('General');
      await page.getByText('Please select a product').waitFor({ state: 'visible', timeout: 10000 });
      await page.getByText('Please select a license').waitFor({ state: 'visible', timeout: 10000 });
      await page.getByText('Please enter the software-ID').waitFor({ state: 'visible', timeout: 10000 });
      await gui.checkScreenshot(page, 'software-editor-save-blocked-empty');
    });

    // Project (Additional page) and Serial Number (General page).
    test('fill software and license information', async () => {
      await editor.setProject('ProLifeGui test project');
      await editor.setSerialNumber('SW-PROLIFEGUI-1');
      await gui.checkScreenshot(page, 'software-editor-new-project-and-serial');
    });

    // Two Switch toggles, same mechanism - one final screenshot after both.
    test('toggle internal use and is multiple', async () => {
      await editor.toggleInternalUse();
      await editor.toggleIsMultiple();
      await gui.checkScreenshot(page, 'software-editor-internal-use-and-multiple');
    });

    test('expiration - unlimited and date', async () => {
      // Unlimited ON hides the date picker (an unlimited license has no expiry) - capture that state.
      // Asked for by state, not toggled blindly: whichever way a new license starts, both halves of
      // this test then get the state they describe.
      await editor.setUnlimited(true);
      await gui.checkScreenshot(page, 'software-editor-unlimited');
      // A concrete expiration date needs Unlimited OFF, which re-shows the picker.
      await editor.setUnlimited(false);
      await editor.setExpiration('31.12.2027');
      await gui.checkScreenshot(page, 'software-editor-expiration');
    });

    // Group-collapse/undo-redo/dirty-close-confirm are generic Document/UI mechanics with no
    // per-entity logic - covered once for the whole suite in devices.editor.multiuser.test.js.
  });

  // --- EDIT existing document: also one continuous document --------------------------------------
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
      test.skip(!editor, 'Software is not available to this user');
    });

    test('open existing software editor', async () => {
      await gui.checkScreenshot(page, 'software-editor-edit-loaded');
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
      await editor.setProject(edited);
      await gui.checkScreenshot(page, 'software-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'software-editor-edit-saved');

      // Persistence check: a Save that only LOOKS successful in the client's own state (but never
      // actually round-tripped/committed server-side) would still pass every assertion above - closing
      // and reopening the SAME document from a clean collection reload proves the new value was really
      // written, not just held in this still-open document's in-memory representation. openEditEditor's
      // own sortBy('added') keeps this the SAME license as before (immutable creation date, unaffected
      // by this Save's Last Modified bump).
      await editor.closeDocument();
      editor = await openEditEditor(page);
      // Project lives on the Additional sub-page and a reopened editor lands on General - see the
      // identical note in devices.editor.multiuser.test.js.
      await editor.openEditorPage("Additional");
      await editor.project.waitForValue(edited);
      await gui.checkScreenshot(page, 'software-editor-edit-reopened');
    });
  });
});
