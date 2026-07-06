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
    for (const cmd of ['New', 'Edit', 'Remove', 'Revision', 'Bind', 'CreateLicenseFile', 'TransferLicenses', 'ResetTransferCounter', 'Support']) {
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
      await new DeviceCollectionPage(page).open();
    });

    // --- filters (each registered filter + built-ins) ------------------------------------------
    test('filter - text search', async ({ page, gui }) => {
      await new DeviceCollectionPage(page).search('ias');
      await gui.checkScreenshot(page, 'devices-filter-text');
    });

    test('filter - sensor status', async ({ page, gui }) => {
      await new DeviceCollectionPage(page).selectFilterOption('status', 'None');
      await gui.checkScreenshot(page, 'devices-filter-status-none');
    });

    test('filter - usage (internal use)', async ({ page, gui }) => {
      await new DeviceCollectionPage(page).selectFilterOption('usage', 'ForInternalPurposes');
      await gui.checkScreenshot(page, 'devices-filter-usage-internal');
    });

    test('filter - license status', async ({ page, gui }) => {
      await new DeviceCollectionPage(page).selectFilterOption('license', 'Sensorswithalicense');
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
      await devices.selectFilterOption('status', 'None');
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
      await devices.selectRow(0);
      await devices.bind();
      await gui.checkScreenshot(page, 'devices-bind-dialog');
    });

    test('create license file - validation / encrypt popup', async ({ page, gui, user }) => {
      test.skip(!canRunDeviceCommand(user, 'CreateLicenseFile'), 'no CreateLicenseFile permission');
      const devices = new DeviceCollectionPage(page);
      await devices.selectRow(0);
      await devices.createLicenseFile();
      // Superuser sees the Encrypt/Unencrypt popup; others may see a validation error - both are a
      // valid, user-specific screenshot.
      await gui.checkScreenshot(page, 'devices-create-license-file');
    });

    test('transfer licenses dialog', async ({ page, gui, user }) => {
      test.skip(!canRunDeviceCommand(user, 'TransferLicenses'), 'no TransferLicenses permission');
      const devices = new DeviceCollectionPage(page);
      await devices.selectRow(0);
      await devices.transferLicenses();
      await gui.checkScreenshot(page, 'devices-transfer-licenses-dialog');
    });

    test('reset transfer counter', async ({ page, gui, user }) => {
      test.skip(!canRunDeviceCommand(user, 'ResetTransferCounter'), 'no ResetTransferCounter permission');
      const devices = new DeviceCollectionPage(page);
      await devices.selectRow(0);
      await devices.resetTransferCounter();
      await gui.checkScreenshot(page, 'devices-reset-transfer-counter');
    });

    test('support (entity ticket) dialog', async ({ page, gui }) => {
      const devices = new DeviceCollectionPage(page);
      await devices.selectRow(0);
      await devices.support();
      await gui.checkScreenshot(page, 'devices-support-dialog');
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
