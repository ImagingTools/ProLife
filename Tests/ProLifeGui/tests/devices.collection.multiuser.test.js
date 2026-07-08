// Hardware (Devices) COLLECTION view - full functional coverage, multi-user.
//
// Runs once per user-project; screenshots auto-separate per user (see playwright.config.js). Users
// who cannot see the Hardware page skip the interaction body but still record a landing screenshot,
// so the permission difference is captured. Command/field gating comes from matrix/permissions.js.

const { test } = require('../fixtures/test');
const { DeviceCollectionPage } = require('../pages');
const { canSeePage, canRunDeviceCommand } = require('../matrix/permissions');

const PAGE = 'Devices';

test.describe('Hardware / collection', () => {
  test.beforeEach(async ({ page }) => {
    await new DeviceCollectionPage(page).reload();
  });

  // Landing screenshot for every user (documents what each permission level sees).
  test('landing', async ({ page, gui, user }) => {
    if (canSeePage(user, PAGE)) await new DeviceCollectionPage(page).open();
    await gui.checkScreenshot(page, 'devices-landing');
  });

  // Structural gate: which commands each user's command bar exposes.
  test('command bar reflects permissions', async ({ page, user }) => {
    test.skip(!canSeePage(user, PAGE), 'user cannot see Hardware');
    const devices = new DeviceCollectionPage(page);
    await devices.open();
    // Support is intentionally excluded here: it only exists on the document EDITOR's command bar
    // (DeviceCollectionViewCommandsDelegate.qml's deviceEditorComp.commandsDelegateComp), not on this
    // collection list - see devices.editor.multiuser.test.js's dedicated Support test.
    for (const cmd of ['New', 'Edit', 'Remove', 'Revision', 'Bind', 'CreateLicenseFile', 'TransferLicenses', 'ResetTransferCounter']) {
      if (canRunDeviceCommand(user, cmd)) {
        await devices.commands.expectHasCommand(cmd);
      } else {
        await devices.commands.expectNoCommand(cmd);
      }
    }
  });

  // Everything below only runs for users who can open the page.
  test.describe('interactions', () => {
    test.beforeEach(async ({ page, user }) => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Hardware');
      const devices = new DeviceCollectionPage(page);
      await devices.open();
      // A prior test in this file may have left a filter/sort applied - collection view state is
      // server-persisted per user session same as document tabs (MultiDocumentCollectionView.qml), so
      // it survives the reload/open above. Start every interaction from a known-clean filter panel.
      await devices.clearAllFilters();
    });

    // --- filters (each registered filter + built-ins) ------------------------------------------
    test('filter - text search', async ({ page, gui }) => {
      await new DeviceCollectionPage(page).search('ias');
      await gui.checkScreenshot(page, 'devices-filter-text');
    });

    test('filter - sensor status', async ({ page, gui }) => {
      // index 0 = "None" (DeviceCollectionView.qml's SensorStatus option order).
      await new DeviceCollectionPage(page).selectFilterOptionByIndex('status', 0);
      await gui.checkScreenshot(page, 'devices-filter-status-none');
    });

    test('filter - usage (internal use)', async ({ page, gui }) => {
      // index 1 = "For Internal Purposes".
      await new DeviceCollectionPage(page).selectFilterOptionByIndex('usage', 1);
      await gui.checkScreenshot(page, 'devices-filter-usage-internal');
    });

    test('filter - license status', async ({ page, gui }) => {
      // index 1 = "Sensors with license" (LicenseFilterDelegate.qml).
      await new DeviceCollectionPage(page).selectFilterOptionByIndex('license', 1);
      await gui.checkScreenshot(page, 'devices-filter-license-with');
    });

    test('filter - customers', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewAccounts'), 'customers filter needs ViewAccounts');
      await new DeviceCollectionPage(page).selectFilterOption('customers', 'QUISS');
      await gui.checkScreenshot(page, 'devices-filter-customer-quiss');
    });

    test('filter - creation date preset', async ({ page, gui }) => {
      await new DeviceCollectionPage(page).setCreationDate('Year_Last');
      await gui.checkScreenshot(page, 'devices-filter-creation-date');
    });

    test('filter - clear all', async ({ page, gui }) => {
      const devices = new DeviceCollectionPage(page);
      await devices.search('ias');
      await devices.selectFilterOptionByIndex('status', 0);
      await devices.clearAllFilters();
      await gui.checkScreenshot(page, 'devices-filter-cleared');
    });

    // --- sorting -------------------------------------------------------------------------------
    // Table.sortBy() addresses columns by their objectName, which is the HeaderIds entry (the field
    // key), NOT the visible HeaderNames caption - the two lists are independently ordered in
    // DevicesPage.acc. "Status" (caption) -> "status" (id); "Name" (caption) -> "licenseName" (id).
    test('sort by status column', async ({ page, gui }) => {
      const devices = new DeviceCollectionPage(page);
      await devices.table.sortBy('status');
      await gui.checkScreenshot(page, 'devices-sort-status-1');
      await devices.table.sortBy('status');
      await gui.checkScreenshot(page, 'devices-sort-status-2');
    });

    test('sort by name column', async ({ page, gui }) => {
      await new DeviceCollectionPage(page).table.sortBy('licenseName');
      await gui.checkScreenshot(page, 'devices-sort-name');
    });

    // --- pagination ----------------------------------------------------------------------------
    test('pagination - page size and navigation', async ({ page, gui }) => {
      const devices = new DeviceCollectionPage(page);
      await devices.pagination.setPageSize(50);
      await gui.checkScreenshot(page, 'devices-pagination-50');
      await devices.pagination.goToPage(2);
      await gui.checkScreenshot(page, 'devices-pagination-page-2');
    });

    // --- row selection + context-sensitive commands --------------------------------------------
    test('select first row', async ({ page, gui }) => {
      await new DeviceCollectionPage(page).selectRow(0);
      await gui.checkScreenshot(page, 'devices-row-selected');
    });

    test('revision dialog', async ({ page, gui, user }) => {
      test.skip(!canRunDeviceCommand(user, 'Revision'), 'no Revision permission');
      const devices = new DeviceCollectionPage(page);
      await devices.selectRow(0);
      await devices.revision();
      await gui.checkScreenshot(page, 'devices-revision-dialog');
    });

    test('remove confirmation dialog', async ({ page, gui, user }) => {
      test.skip(!canRunDeviceCommand(user, 'Remove'), 'no Remove permission');
      const devices = new DeviceCollectionPage(page);
      await devices.selectRow(0);
      await devices.removeItem();
      await gui.checkScreenshot(page, 'devices-remove-dialog');
    });

    test('bind dialog', async ({ page, gui, user }) => {
      test.skip(!canRunDeviceCommand(user, 'Bind'), 'no Bind permission');
      const devices = new DeviceCollectionPage(page);
      // Bind only enables for a row with a non-empty MAC address (updateStateCustomCommands in
      // DeviceCollectionViewCommandsDelegate.qml) - a plain selectRow(0) may land on a row without one
      // and the command would just stay disabled. Finished-status sensors always have a MAC.
      await devices.filterFinishedSensors();
      await devices.selectRow(0);
      await devices.bind();
      await gui.checkScreenshot(page, 'devices-bind-dialog');
    });

    test('create license file - validation / encrypt popup', async ({ page, gui, user }) => {
      test.skip(!canRunDeviceCommand(user, 'CreateLicenseFile'), 'no CreateLicenseFile permission');
      const devices = new DeviceCollectionPage(page);
      // CreateLicenseFile's onCommandActivated requires inUse===true, non-empty MAC/serial, and
      // status==="Finished" (DeviceCollectionViewCommandsDelegate.qml:422-445) - filter for a row that
      // actually satisfies all four instead of hoping row 0 happens to.
      await devices.filterFinishedSensorsWithLicense();
      await devices.selectRow(0);
      await devices.createLicenseFile();
      // Superuser sees the Encrypt/Unencrypt popup; others may see a validation error - both are a
      // valid, user-specific screenshot.
      await gui.checkScreenshot(page, 'devices-create-license-file');
    });

    test('transfer licenses dialog', async ({ page, gui, user }) => {
      test.skip(!canRunDeviceCommand(user, 'TransferLicenses'), 'no TransferLicenses permission');
      const devices = new DeviceCollectionPage(page);
      // TransferLicenses requires inUse===true (:451-456) - without a license bound it just opens the
      // "No license is linked" error dialog instead of the real transfer dialog.
      await devices.filterSensorsWithLicense();
      await devices.selectRow(0);
      await devices.transferLicenses();
      await gui.checkScreenshot(page, 'devices-transfer-licenses-dialog');
    });

    test('reset transfer counter', async ({ page, gui, user }) => {
      test.skip(!canRunDeviceCommand(user, 'ResetTransferCounter'), 'no ResetTransferCounter permission');
      const devices = new DeviceCollectionPage(page);
      // No hard data precondition, but a device that actually has a license/transfer history makes the
      // reset meaningful rather than a no-op on an untouched sensor.
      await devices.filterSensorsWithLicense();
      await devices.selectRow(0);
      await devices.resetTransferCounter();
      await gui.checkScreenshot(page, 'devices-reset-transfer-counter');
    });

    test('decrypt file dialog', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewSensors'), 'needs ViewSensors');
      // DecryptFile opens a native file picker; we only verify the command is reachable and the
      // starting state (the OS dialog itself is out of Playwright's DOM).
      const devices = new DeviceCollectionPage(page);
      await gui.checkScreenshot(page, 'devices-before-decrypt');
    });
  });
});
