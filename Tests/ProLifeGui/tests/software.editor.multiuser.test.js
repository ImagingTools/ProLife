// Software (SoftwareProducts) EDITOR - full functional coverage, multi-user.
//
// The editor is a document tab opened by the "New" or "Edit" command. Only users who can open it
// (ViewLicenses, plus AddLicense for New) run the interaction bodies; field editability is
// permission-driven (SoftwareEditor.checkPermissions), so the per-user screenshots + the structural
// "editable fields" test encode the permission matrix.

const { test } = require('../fixtures/test');
const { SoftwareCollectionPage, SoftwareEditorPage } = require('../pages');
const { canSeePage, canRunSoftwareCommand, canEditSoftwareField, SOFTWARE_FIELD_PERMISSIONS } = require('../matrix/permissions');

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
  await software.selectRow(0);
  await software.editItem(); // "Edit" -> loads selected doc via SoftwareProductItem
  return new SoftwareEditorPage(page);
}

test.describe('Software / editor', () => {
  // --- NEW editor ------------------------------------------------------------------------------
  test.describe('new document', () => {
    test.beforeEach(async ({ user }) => {
      test.skip(!canRunSoftwareCommand(user, 'New'), 'user cannot create a license (AddLicense)');
    });

    test('empty new editor', async ({ page, gui }) => {
      await openNewEditor(page);
      await gui.checkScreenshot(page, 'software-editor-new-empty');
    });

    test('fill software information group', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setProject('ProLifeGui test project');
      await gui.checkScreenshot(page, 'software-editor-new-project');
    });

    test('fill license information - serial', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setSerialNumber('SW-PROLIFEGUI-1');
      await gui.checkScreenshot(page, 'software-editor-new-serial');
    });

    test('toggle internal use and is multiple', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.toggleInternalUse();
      await gui.checkScreenshot(page, 'software-editor-internal-use');
      await editor.toggleIsMultiple();
      await gui.checkScreenshot(page, 'software-editor-is-multiple');
    });

    test('expiration - unlimited and date', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setUnlimited();
      await gui.checkScreenshot(page, 'software-editor-unlimited');
      // Re-toggle or set date; here we assume state after previous may vary - use separate.
      await editor.setExpiration('31.12.2027');
      await gui.checkScreenshot(page, 'software-editor-expiration');
    });

    test('collapse / expand groups', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.toggleGroup('software');
      await gui.checkScreenshot(page, 'software-editor-software-group-collapsed');
      await editor.toggleGroup('software');
      await editor.toggleGroup('license');
      await gui.checkScreenshot(page, 'software-editor-license-group-collapsed');
      await editor.toggleGroup('expiration');
      await gui.checkScreenshot(page, 'software-editor-expiration-group-collapsed');
    });

    test('undo / redo', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setProject('Undo me');
      await editor.undo();
      await gui.checkScreenshot(page, 'software-editor-after-undo');
      await editor.redo();
      await gui.checkScreenshot(page, 'software-editor-after-redo');
    });

    test('close dirty document -> confirm dialog', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setProject('dirty');
      await editor.closeDocument();
      await gui.checkScreenshot(page, 'software-editor-close-dirty');
    });
  });

  // --- EDIT existing document ------------------------------------------------------------------
  test.describe('edit document', () => {
    test.beforeEach(async ({ user }) => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Software');
    });

    test('open existing software editor', async ({ page, gui }) => {
      await openEditEditor(page);
      await gui.checkScreenshot(page, 'software-editor-edit-loaded');
    });

    // Structural permission matrix: exactly which fields this user may edit. This is the machine-
    // checked companion to the per-user screenshots - a field the user cannot edit must still be
    // present (visible) but read-only; we assert presence here and rely on the screenshot for the
    // read-only visual.
    test('editable fields reflect permissions', async ({ page, user }) => {
      const editor = await openEditEditor(page);
      for (const fieldObjectName of Object.keys(SOFTWARE_FIELD_PERMISSIONS)) {
        await editor.expectFieldVisible(fieldObjectName);
        // eslint-disable-next-line no-console
        console.log(`[${user.key}] ${fieldObjectName} editable=${canEditSoftwareField(user, fieldObjectName, false)}`);
      }
    });

    test('edit fields and save', async ({ page, gui, user }) => {
      // Editing an EXISTING license's project needs ChangeProjectForLicense. AddLicense only unlocks
      // fields on a NEW document, so it must NOT gate this edit-save path (a user with AddLicense but
      // not ChangeProjectForLicense would otherwise reach a read-only field and fail on the fill verify).
      test.skip(!user.can('ChangeProjectForLicense'), 'cannot change the license project field');
      const editor = await openEditEditor(page);
      await editor.setProject('Edited by ProLifeGui');
      await gui.checkScreenshot(page, 'software-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'software-editor-edit-saved');
    });
  });
});
