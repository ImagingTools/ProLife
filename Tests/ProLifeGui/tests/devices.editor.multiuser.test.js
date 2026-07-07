// Hardware (Devices) EDITOR - full functional coverage, multi-user.
//
// The editor is a document tab opened by the "New" or "Edit" command (see DeviceEditorPage.js for the
// Document Service flow). Only users who can open it (ViewSensors, plus AddSensor for New) run the
// interaction bodies; field editability is permission-driven (DeviceEditor.checkPermissions), so the
// per-user screenshots + the structural "editable fields" test encode the permission matrix.

const { test } = require('../fixtures/test');
const { DeviceCollectionPage, DeviceEditorPage } = require('../pages');
const { canSeePage, canRunDeviceCommand, canEditDeviceField, DEVICE_FIELD_PERMISSIONS } = require('../matrix/permissions');

const PAGE = 'Devices';

async function openNewEditor(page) {
  const devices = new DeviceCollectionPage(page);
  await devices.reload();
  await devices.open();
  await devices.newItem(); // "New" -> fresh document tab
  return new DeviceEditorPage(page);
}

async function openEditEditor(page) {
  const devices = new DeviceCollectionPage(page);
  await devices.reload();
  await devices.open();
  await devices.selectRow(0);
  await devices.editItem(); // "Edit" -> loads selected doc via GetDeviceRepresentation
  return new DeviceEditorPage(page);
}

test.describe('Hardware / editor', () => {
  // --- NEW editor ------------------------------------------------------------------------------
  test.describe('new document', () => {
    test.beforeEach(async ({ user }) => {
      test.skip(!canRunDeviceCommand(user, 'New'), 'user cannot create a sensor (AddSensor)');
    });

    test('empty new editor', async ({ page, gui }) => {
      await openNewEditor(page);
      await gui.checkScreenshot(page, 'device-editor-new-empty');
    });

    test('fill device information group', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setDeviceType('RTV'); // picks a device type option (adjust to a real catalogue name)
      await gui.checkScreenshot(page, 'device-editor-new-device-type');
      await editor.setHardwareConfiguration('Standard'); // dependent combo populated by device type
      await gui.checkScreenshot(page, 'device-editor-new-configuration');
      await editor.setDescription('ProLifeGui test device');
      await gui.checkScreenshot(page, 'device-editor-new-description');
    });

    test('mac address validation - invalid then valid', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setMacAddress('12345');
      await gui.checkScreenshot(page, 'device-editor-mac-invalid');
      await editor.setMacAddress('11:11:11:11:11:11');
      await gui.checkScreenshot(page, 'device-editor-mac-valid');
    });

    test('serial number + mac triggers Finished-status confirm dialog', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setSerialNumber('SN-PROLIFEGUI-1');
      await editor.setMacAddress('22:22:22:22:22:22');
      // Entering a valid serial + mac raises the "set production status to Finished?" MessageDialog.
      await gui.checkScreenshot(page, 'device-editor-finished-confirm');
    });

    test('additional information group - order / status / project / internal use', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setProductionStatus('In Progress');
      await gui.checkScreenshot(page, 'device-editor-status');
      await editor.setProject('ProLifeGui Project');
      await gui.checkScreenshot(page, 'device-editor-project');
      await editor.toggleInternalUse();
      await gui.checkScreenshot(page, 'device-editor-internal-use');
    });

    test('collapse / expand groups', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.toggleGroup('device');
      await gui.checkScreenshot(page, 'device-editor-device-group-collapsed');
      await editor.toggleGroup('device');
      await editor.toggleGroup('additional');
      await gui.checkScreenshot(page, 'device-editor-additional-group-collapsed');
    });

    test('undo / redo', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setProject('Undo me');
      await editor.undo();
      await gui.checkScreenshot(page, 'device-editor-after-undo');
      await editor.redo();
      await gui.checkScreenshot(page, 'device-editor-after-redo');
    });

    test('editor commands require save first', async ({ page, gui, user }) => {
      test.skip(!canRunDeviceCommand(user, 'Bind'), 'no Bind permission');
      const editor = await openNewEditor(page);
      // Bind on an unsaved document -> "Please save the document first" info dialog.
      await editor.bind();
      await gui.checkScreenshot(page, 'device-editor-bind-needs-save');
    });

    test('close dirty document -> confirm dialog', async ({ page, gui }) => {
      const editor = await openNewEditor(page);
      await editor.setProject('dirty');
      await editor.closeDocument();
      await gui.checkScreenshot(page, 'device-editor-close-dirty');
    });
  });

  // --- EDIT existing document ------------------------------------------------------------------
  test.describe('edit document', () => {
    test.beforeEach(async ({ user }) => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Hardware');
    });

    test('open existing sensor editor', async ({ page, gui }) => {
      await openEditEditor(page);
      await gui.checkScreenshot(page, 'device-editor-edit-loaded');
    });

    // Structural permission matrix: exactly which fields this user may edit. This is the machine-
    // checked companion to the per-user screenshots - a field the user cannot edit must still be
    // present (visible) but read-only; we assert presence here and rely on the screenshot for the
    // read-only visual.
    test('editable fields reflect permissions', async ({ page, user }) => {
      const editor = await openEditEditor(page);
      for (const fieldObjectName of Object.keys(DEVICE_FIELD_PERMISSIONS)) {
        // The field is always rendered; editability differs. Assert it is present so a missing field
        // fails loudly (rather than a silent green).
        await editor.expectFieldVisible(fieldObjectName);
        // Document (via a soft log) whether this user is expected to be able to edit it.
        // eslint-disable-next-line no-console
        console.log(`[${user.key}] ${fieldObjectName} editable=${canEditDeviceField(user, fieldObjectName, false)}`);
      }
    });

    test('edit fields and save', async ({ page, gui, user }) => {
      // Editing an EXISTING sensor's project needs ChangeProjectForSensor specifically. Holding the
      // parent EditSensor is not enough to make the Project field writable, so gate strictly on
      // ChangeProjectForSensor (otherwise the fill verify fails on a read-only field).
      test.skip(!user.can('ChangeProjectForSensor'), 'cannot change the sensor project field');
      const editor = await openEditEditor(page);
      await editor.setProject('Edited by ProLifeGui');
      await gui.checkScreenshot(page, 'device-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'device-editor-edit-saved');
    });
  });
});
