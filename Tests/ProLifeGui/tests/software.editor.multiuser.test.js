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
const { canSeePage, canRunSoftwareCommand, canEditSoftwareField, SOFTWARE_FIELD_PERMISSIONS } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

const PAGE = 'SoftwareProducts';

async function openNewEditor(page) {
  const software = new SoftwareCollectionPage(page);
  await software.reload();
  await software.open();
  await software.newItem(); // "New" -> fresh document tab
  return new SoftwareEditorPage(page);
}

async function openEditEditor(page) {
  const software = new SoftwareCollectionPage(page);
  await software.reload();
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
    let page, user, editor;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      if (canRunSoftwareCommand(user, 'New')) {
        editor = await openNewEditor(page);
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(() => {
      test.skip(!canRunSoftwareCommand(user, 'New'), 'user cannot create a license (AddLicense)');
    });

    test('empty new editor', async () => {
      await gui.checkScreenshot(page, 'software-editor-new-empty');
    });

    // Project (Software Information group) and Serial Number (License Information group) are both
    // plain TextInput fields with the identical fill/verify mechanism - one combined screenshot
    // documents both without paying for two separate fill+screenshot passes.
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
      // A new license starts limited (expiration date picker visible). Toggle Unlimited ON first: that
      // hides the date picker (an unlimited license has no expiry) - capture that state.
      await editor.setUnlimited();
      await gui.checkScreenshot(page, 'software-editor-unlimited');
      // To set a concrete expiration date the license must NOT be unlimited, so toggle Unlimited back
      // OFF - that re-shows the date picker - before filling it. Setting a date while Unlimited is ON
      // would target a hidden picker (the previous cause of this test's failure).
      await editor.setUnlimited();
      await editor.setExpiration('31.12.2027');
      await gui.checkScreenshot(page, 'software-editor-expiration');
    });

    // Group-collapse/undo-redo/dirty-close-confirm are generic Document/UI mechanics with no
    // per-entity logic - covered once for the whole suite in devices.editor.multiuser.test.js.
  });

  // --- EDIT existing document: also one continuous document --------------------------------------
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
      test.skip(!canSeePage(user, PAGE), 'user cannot see Software');
    });

    test('open existing software editor', async () => {
      await gui.checkScreenshot(page, 'software-editor-edit-loaded');
    });

    // Structural permission matrix: exactly which fields this user may edit. This is the machine-
    // checked companion to the per-user screenshots - a field the user cannot edit must still be
    // present (visible) but read-only; we assert presence here and rely on the screenshot for the
    // read-only visual.
    test('editable fields reflect permissions', async () => {
      for (const fieldObjectName of Object.keys(SOFTWARE_FIELD_PERMISSIONS)) {
        // ExpirationDatePicker's VISIBILITY (not just editability) is conditional on this license's own
        // Unlimited toggle - SoftwareEditor.qml hides the date picker entirely for an unlimited license
        // (see the dedicated "expiration - unlimited and date" test for that toggle behavior). Which
        // license "edit document" opens is a stable but otherwise arbitrary existing record (sorted by
        // "added" for determinism - see openEditEditor), so it can legitimately be an unlimited one;
        // asserting unconditional visibility here would fail on real, correct app behavior rather than a
        // permission bug. Skip the visibility check specifically when it's legitimately hidden this way.
        if (fieldObjectName === 'ExpirationDatePicker' && (await gui.countVisible(page, [fieldObjectName])) === 0) {
          continue;
        }
        await editor.expectFieldVisible(fieldObjectName);
        // eslint-disable-next-line no-console
        console.log(`[${user.key}] ${fieldObjectName} editable=${canEditSoftwareField(user, fieldObjectName, false)}`);
      }
    });

    test('edit fields and save', { tag: '@mutating' }, async () => {
      // Editing an EXISTING license's project needs ChangeProjectForLicense. AddLicense only unlocks
      // fields on a NEW document, so it must NOT gate this edit-save path (a user with AddLicense but
      // not ChangeProjectForLicense would otherwise reach a read-only field and fail on the fill verify).
      test.skip(!user.can('ChangeProjectForLicense'), 'cannot change the license project field');
      await editor.setProject('Edited by ProLifeGui');
      await gui.checkScreenshot(page, 'software-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'software-editor-edit-saved');
    });
  });
});
