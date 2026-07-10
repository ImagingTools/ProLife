// Hardware (Devices) COLLECTION view - full functional coverage, multi-user.
//
// Runs once per user-project; screenshots auto-separate per user (see playwright.config.js). Users
// who cannot see the Hardware page skip the interaction body but still record a landing screenshot,
// so the permission difference is captured. Command/field gating comes from matrix/permissions.js.
//
// 'landing' and 'command bar reflects permissions' keep the default fresh-page-per-test fixture (their
// whole point is documenting the COLD load state). 'interactions' and its nested 'column configuration'
// block are `.serial` and share ONE page opened once in beforeAll (see fixtures/test.js's newUserPage)
// instead of reload()-ing per test - most of this file's cost was the WASM reboot, and these tests don't
// need a cold load, just a clean filter panel, which clearAllFilters() gives cheaply without navigation.
// Trade-off: a failure partway through 'interactions' skips the remaining steps in that block.

const { test, expect, newUserPage } = require('../fixtures/test');
const { DeviceCollectionPage } = require('../pages');
const { canSeePage, canRunDeviceCommand } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

const PAGE = 'Devices';

test.describe('Hardware / collection', () => {
  test.beforeEach(async ({ page }) => {
    await new DeviceCollectionPage(page).reload();
  });

  // Landing screenshot for every user (documents what each permission level sees).
  test('landing', async ({ page, gui, user }) => {
    const devices = new DeviceCollectionPage(page);
    if (canSeePage(user, PAGE)) await devices.open();
    await gui.checkScreenshot(page, 'devices-landing', await devices.timestampColumnMasks());
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

  // Everything below only runs for users who can open the page. One shared page/session for the
  // whole block (see file header) - opened once in beforeAll, filters reset per-test via beforeEach.
  test.describe.serial('interactions', () => {
    let page, user, devices;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      devices = new DeviceCollectionPage(page);
      // newUserPage() only opens a blank page - unlike the page fixture, nothing has navigated to the
      // app yet, so load it once here before the very first interaction.
      await devices.reload();
      if (canSeePage(user, PAGE)) {
        await devices.open();
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(async () => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Hardware');
      // A prior test in this block may have left a filter/sort applied - collection view state is
      // server-persisted per user session same as document tabs (MultiDocumentCollectionView.qml).
      // Start every interaction from a known-clean filter panel (cheap - no navigation needed).
      await devices.clearAllFilters();
    });

    // --- filters (each registered filter + built-ins) ------------------------------------------
    test('filter - text search', async () => {
      await devices.search('ias');
      await gui.checkScreenshot(page, 'devices-filter-text', await devices.timestampColumnMasks());
    });

    test('filter - sensor status', async () => {
      // index 0 = "None" (DeviceCollectionView.qml's SensorStatus option order).
      await devices.selectFilterOptionByIndex('status', 0);
      await gui.checkScreenshot(page, 'devices-filter-status-none', await devices.timestampColumnMasks());
    });

    test('filter - usage (internal use)', async () => {
      // index 1 = "For Internal Purposes".
      await devices.selectFilterOptionByIndex('usage', 1);
      await gui.checkScreenshot(page, 'devices-filter-usage-internal', await devices.timestampColumnMasks());
    });

    test('filter - license status', async () => {
      // index 1 = "Sensors with license" (LicenseFilterDelegate.qml).
      await devices.selectFilterOptionByIndex('license', 1);
      await gui.checkScreenshot(page, 'devices-filter-license-with', await devices.timestampColumnMasks());
    });

    test('filter - customers', async () => {
      test.skip(!user.can('ViewAccounts'), 'customers filter needs ViewAccounts');
      // The Customers filter list is populated from the customers this user's org can see; an
      // org-scoped user (which resolves to zero customers) legitimately has no "QUISS" entry, so skip
      // rather than fail on a missing option - see AccountCollection's org-scoping note.
      test.skip(
        !(await devices.filters.combo('CustomersFilter').hasOption('QUISS')),
        'no QUISS customer visible to this user (org-scoped)'
      );
      await devices.selectFilterOption('customers', 'QUISS');
      await gui.checkScreenshot(page, 'devices-filter-customer-quiss', await devices.timestampColumnMasks());
    });

    test('filter - creation date preset', async () => {
      await devices.setCreationDate('Year_Last');
      await gui.checkScreenshot(page, 'devices-filter-creation-date', await devices.timestampColumnMasks());
    });

    test('filter - clear all', async () => {
      await devices.search('ias');
      await devices.selectFilterOptionByIndex('status', 0);
      await devices.clearAllFilters();
      await gui.checkScreenshot(page, 'devices-filter-cleared', await devices.timestampColumnMasks());
    });

    // --- sorting -------------------------------------------------------------------------------
    // Table.sortBy() addresses columns by their objectName, which is the HeaderIds entry (the field
    // key), NOT the visible HeaderNames caption - the two lists are independently ordered in
    // DevicesPage.acc. "Status" (caption) -> "status" (id); "Name" (caption) -> "licenseName" (id).
    test('sort by status column', async () => {
      await devices.table.sortBy('status');
      await gui.checkScreenshot(page, 'devices-sort-status-1', await devices.timestampColumnMasks());
      await devices.table.sortBy('status');
      await gui.checkScreenshot(page, 'devices-sort-status-2', await devices.timestampColumnMasks());
    });

    // --- pagination ----------------------------------------------------------------------------
    test('pagination - page size and navigation', async () => {
      await devices.pagination.setPageSize(50);
      await gui.checkScreenshot(page, 'devices-pagination-50', await devices.timestampColumnMasks());
      await devices.pagination.goToPage(2);
      await gui.checkScreenshot(page, 'devices-pagination-page-2', await devices.timestampColumnMasks());
    });

    // --- row selection + context-sensitive commands --------------------------------------------
    test('revision dialog', async () => {
      test.skip(!canRunDeviceCommand(user, 'Revision'), 'no Revision permission');
      await devices.selectRow(0);
      await devices.revision();
      await gui.checkScreenshot(page, 'devices-revision-dialog', await devices.timestampColumnMasks());
      // This document/dialog stays open for the rest of the chain (shared page, no reload between
      // tests) - dismiss it so it doesn't block the next test's clicks.
      await gui.dismissDialog(page);
    });

    test('remove confirmation dialog', async () => {
      test.skip(!canRunDeviceCommand(user, 'Remove'), 'no Remove permission');
      await devices.selectRow(0);
      await devices.removeItem();
      await gui.checkScreenshot(page, 'devices-remove-dialog', await devices.timestampColumnMasks());
      await gui.dismissDialog(page); // decline - must not actually remove the device
    });

    test('bind dialog', async () => {
      test.skip(!canRunDeviceCommand(user, 'Bind'), 'no Bind permission');
      // Bind only enables for a row with a non-empty MAC address (updateStateCustomCommands in
      // DeviceCollectionViewCommandsDelegate.qml) - a plain selectRow(0) may land on a row without one
      // and the command would just stay disabled. Finished-status sensors always have a MAC.
      await devices.filterFinishedSensors();
      await devices.selectRow(0);
      await devices.bind();
      await gui.checkScreenshot(page, 'devices-bind-dialog', await devices.timestampColumnMasks());
      await gui.dismissDialog(page);
    });

    // The dialog above only documents its landing state (Product + Used Licenses as loaded). The rest
    // of its actual functionality - opening the nested "Available Licenses" editor, binding/unbinding a
    // license, and the Save -> "Apply changes" project-name prompt -> real updateDeviceBindingRequest -
    // was previously untested (HardwareProductBindingDialog.qml / HardwareProductBindingEditor.qml).
    test.describe('bind dialog - full functionality', () => {
      test.beforeEach(() => {
        test.skip(!canRunDeviceCommand(user, 'Bind'), 'no Bind permission');
      });

      test('open "Bind New Licenses", then Cancel discards', async () => {
        // "Bind New Licenses" only enables once a Product is selected - the ComboBox only
        // auto-selects one when the row already has an existing bound license (Product is otherwise
        // editable/empty, HardwareProductBindingDialog.qml's updateGui()) - filter for a row that has
        // one already instead of hoping row 0 happens to.
        await devices.filterFinishedSensorsWithLicense();
        await devices.selectRow(0);
        await devices.bind();
        await devices.openBindNewLicenses();
        await gui.checkScreenshot(page, 'devices-bind-available-licenses-dialog', await devices.timestampColumnMasks());
        await devices.cancelBindNewLicenses();
        // Back on the (unchanged) Bind dialog - the nested editor's own checkable table is gone (its
        // "RowCheckBox" only exists on that table's rows, never the Bind dialog's own "Used Licenses").
        await gui.expectHidden(page, ['Dialog', 'RowCheckBox'], 'nested editor should be closed after Cancel');
        await gui.dismissDialog(page);
      });

      test('bind a new license, then Save applies it via the project prompt', { tag: '@mutating' }, async () => {
        // Same precondition as above - a row with an existing bound license so Product is pre-selected
        // and "Bind New Licenses" is enabled.
        await devices.filterFinishedSensorsWithLicense();
        await devices.selectRow(0);
        await devices.bind();
        await devices.openBindNewLicenses();
        // The nested "Available Licenses" list is the product's licenses NOT already bound to a device;
        // for some sensors (and once earlier binds in this serial phase have consumed the spares) it's
        // legitimately empty - there is then nothing to bind, which is a data state, not a failure.
        // Dismiss both dialogs and skip so the shared serial page is left clean for the next test.
        if ((await gui.countAny(page, ['Dialog', 'TableRow_0', 'RowCheckBox'])) === 0) {
          await devices.cancelBindNewLicenses();
          await gui.dismissDialog(page);
          test.skip(true, 'no available (unbound) licenses to bind for this sensor');
        }
        await devices.checkAvailableLicense(0);
        await devices.confirmBindNewLicenses();
        // Back on the Bind dialog - the checked license now shows in "Used Licenses".
        await gui.checkScreenshot(page, 'devices-bind-license-added', await devices.timestampColumnMasks());
        await devices.saveBinding();
        await gui.checkScreenshot(page, 'devices-bind-apply-changes-prompt');
        await devices.confirmProjectPrompt('GUI test project');
        await gui.waitForStable(page);
        // updateDeviceBindingRequest closes the dialog on success (HardwareProductBindingDialog.qml's
        // updateDeviceBindingRequest.onFinished calls productEditorDialog.finished(Enums.cancel)).
        await gui.expectHidden(page, ['Dialog'], 'Bind dialog should close after a successful save');
      });

      test('unbind an existing license', async () => {
        test.skip(!user.can('UnbindSensor'), 'needs UnbindSensor');
        // The row filtered by filterFinishedSensorsWithLicense() has at least one used license (see
        // 'create license file' tests above), so its Bind dialog opens with "Used Licenses" pre-filled.
        await devices.filterFinishedSensorsWithLicense();
        await devices.selectRow(0);
        await devices.bind();
        // "Used Licenses" is normally pre-filled for a licensed sensor, but which sensor lands at row 0
        // is data-dependent per user (and prior binds/unbinds in the run can change it), so a given
        // sensor can open the Bind dialog with no used-license row. Nothing to unbind then - a data
        // state, not a failure - so dismiss and skip, keeping the shared page clean for the next test.
        if ((await gui.countAny(page, ['Dialog', 'TableRow_0'])) === 0) {
          await gui.dismissDialog(page);
          test.skip(true, 'sensor has no used license to unbind');
        }
        await devices.selectUsedLicenseRow(0);
        await devices.unbindLicense();
        await gui.checkScreenshot(page, 'devices-bind-license-unbound', await devices.timestampColumnMasks());
        // Discard rather than Save - keeps this test independent from the "bind a new license" test
        // above instead of compounding mutations to the same device across the run.
        await gui.dismissDialog(page);
      });
    });

    test('create license file - validation (non-superuser)', { tag: '@mutating' }, async () => {
      test.skip(!canRunDeviceCommand(user, 'CreateLicenseFile'), 'no CreateLicenseFile permission');
      test.skip(user.key === 'su', 'su sees the Encrypt/Unencrypt choice popup instead - see the dedicated su tests below');
      // CreateLicenseFile's onCommandActivated requires inUse===true, non-empty MAC/serial, and
      // status==="Finished" (DeviceCollectionViewCommandsDelegate.qml:422-445) - filter for a row that
      // actually satisfies all four instead of hoping row 0 happens to.
      await devices.filterFinishedSensorsWithLicense();
      await devices.selectRow(0);
      await devices.createLicenseFile();
      await gui.checkScreenshot(page, 'devices-create-license-file', await devices.timestampColumnMasks());
      await gui.dismissDialog(page); // no-op if this path didn't open a dialog (success/no permission)
    });

    test('create license file - "No license is linked" error (non-superuser)', async () => {
      test.skip(!canRunDeviceCommand(user, 'CreateLicenseFile'), 'no CreateLicenseFile permission');
      test.skip(user.key === 'su', 'su sees the Encrypt/Unencrypt choice popup instead - see the dedicated su tests below');
      // The success case above proves the command works; this deliberately picks a row that fails the
      // FIRST validation check (inUse === false) so the "No license is linked" error path itself stays
      // covered instead of only ever being hit by accident.
      await devices.filterSensorsWithoutLicense();
      await devices.selectRow(0);
      await devices.createLicenseFile();
      await gui.checkScreenshot(page, 'devices-create-license-file-no-license-error', await devices.timestampColumnMasks());
      await gui.dismissDialog(page);
    });

    // The Encrypt/Unencrypt choice popup (encryptPopupMenuDialog) only appears for the superuser
    // (AuthorizationController.loggedUserIsSuperuser() in onCreateLicenseFile()) - every other user's
    // CreateLicenseFile either succeeds silently or shows a validation error, covered above. Both
    // popup choices actually submit the request (CreateLicenseFileRequest -> a real file write via
    // FileIO on the web build), so exercise both instead of only ever screenshotting the choice popup.
    test.describe('create license file - encrypt choice (superuser)', () => {
      test.beforeEach(() => {
        test.skip(user.key !== 'su', 'Encrypt/Unencrypt popup is superuser-only');
      });

      test('choose Encrypted', { tag: '@mutating' }, async () => {
        await devices.filterFinishedSensorsWithLicense();
        await devices.selectRow(0);
        await devices.createLicenseFile();
        await gui.checkScreenshot(page, 'devices-create-license-file-encrypt-popup');
        await devices.chooseEncrypted();
        await gui.checkScreenshot(page, 'devices-create-license-file-encrypted', await devices.timestampColumnMasks());
      });

      test('choose Unencrypted', { tag: '@mutating' }, async () => {
        await devices.filterFinishedSensorsWithLicense();
        await devices.selectRow(0);
        await devices.createLicenseFile();
        await devices.chooseNotEncrypted();
        await gui.checkScreenshot(page, 'devices-create-license-file-not-encrypted', await devices.timestampColumnMasks());
      });
    });

    test('transfer licenses dialog', async () => {
      test.skip(!canRunDeviceCommand(user, 'TransferLicenses'), 'no TransferLicenses permission');
      // TransferLicenses requires inUse===true (:451-456) - without a license bound it just opens the
      // "No license is linked" error dialog instead of the real transfer dialog.
      await devices.filterSensorsWithLicense();
      await devices.selectRow(0);
      await devices.transferLicenses();
      await gui.checkScreenshot(page, 'devices-transfer-licenses-dialog', await devices.timestampColumnMasks());
      await gui.dismissDialog(page);
    });

    test('reset transfer counter', { tag: '@mutating' }, async () => {
      test.skip(!canRunDeviceCommand(user, 'ResetTransferCounter'), 'no ResetTransferCounter permission');
      // No hard data precondition, but a device that actually has a license/transfer history makes the
      // reset meaningful rather than a no-op on an untouched sensor.
      await devices.filterSensorsWithLicense();
      await devices.selectRow(0);
      // Selecting a row shows the right-hand Licenses panel, which narrows the command bar; for users
      // with the fullest Hardware command set that pushes the low-priority ResetTransferCounter button
      // into the "..." overflow menu, where it isn't directly clickable (no addressable More-menu node
      // on this build). The command itself is verified for the superuser, whose bar fits it - so skip
      // when it overflowed rather than fail on a hidden button.
      test.skip(
        (await gui.countVisible(page, ['CommandsView', 'ResetTransferCounterButton'])) === 0,
        'ResetTransferCounter overflowed into the "..." menu for this user'
      );
      await devices.resetTransferCounter();
      // resetTransferCounter() sends the request immediately (no confirm step) and shows a
      // showInfoDialog result - dismiss it before the next test.
      await gui.checkScreenshot(page, 'devices-reset-transfer-counter', await devices.timestampColumnMasks());
      await gui.dismissDialog(page);
    });

    test('decrypt file dialog', async () => {
      test.skip(!user.can('ViewSensors'), 'needs ViewSensors');
      // DecryptFile opens a native file picker; we only verify the command is reachable and the
      // starting state (the OS dialog itself is out of Playwright's DOM).
      await gui.checkScreenshot(page, 'devices-before-decrypt', await devices.timestampColumnMasks());
    });

    // --- column configuration (right-click a header -> "Table configuration" dialog) -----------
    // TableHeaderParamComp.qml (opened via CollectionViewBase.qml's headerRightClickEnabled, default
    // true). Lets a user hide/show columns and reorder them; both persist server-side per user
    // (TableViewParams), same class of shared-session state as tabs/filters, so each test either
    // restores what it changed or is immediately followed by a test that resets to defaults. Reuses
    // the same shared page/devices as the rest of 'interactions' (see file header).
    test.describe.serial('column configuration (header right-click)', () => {
      test('opens via header right-click', async () => {
        const dialog = await devices.openColumnConfig('status');
        await gui.checkScreenshot(page, 'devices-column-config-dialog');
        await dialog.cancel();
      });

      test('hide a column via Apply, then restore it', async () => {
        const dialog = await devices.openColumnConfig('status');
        // "Last Modified" (timeStamp) is appended last in DevicesPage.acc's header list, so it's
        // always the dialog's last row regardless of any prior reordering.
        const lastRow = (await dialog.rowCount()) - 1;
        await dialog.toggleColumn(lastRow);
        await dialog.apply();
        await gui.expectHidden(page, ['TableHeaders', 'timeStamp'], '"Last Modified" header should be hidden after unchecking + Apply');
        await gui.checkScreenshot(page, 'devices-column-hidden', await devices.timestampColumnMasks());

        // Restore immediately so later tests/users sharing this workspace see the default columns.
        const dialog2 = await devices.openColumnConfig('status');
        const lastRow2 = (await dialog2.rowCount()) - 1;
        await dialog2.toggleColumn(lastRow2);
        await dialog2.apply();
        await gui.expectVisible(page, ['TableHeaders', 'timeStamp'], '"Last Modified" header should be visible again after re-checking + Apply');
      });

      test('toggling a column then Cancel discards the change', async () => {
        const dialog = await devices.openColumnConfig('status');
        const lastRow = (await dialog.rowCount()) - 1;
        await dialog.toggleColumn(lastRow);
        await dialog.cancel();
        await gui.expectVisible(page, ['TableHeaders', 'timeStamp'], 'Cancel must not apply the unchecked column');
      });

      test('reorder columns with Up/Down', async () => {
        const beforeBox = await gui.dom.byPath(page, ['TableHeaders', 'timeStamp']).boundingBox();

        const dialog = await devices.openColumnConfig('status');
        const lastRow = (await dialog.rowCount()) - 1;
        await dialog.selectColumn(lastRow);
        await dialog.moveUp();
        await dialog.apply();

        const afterBox = await gui.dom.byPath(page, ['TableHeaders', 'timeStamp']).boundingBox();
        expect(afterBox.x, '"Last Modified" should have moved left (earlier) after Move Up + Apply').toBeLessThan(beforeBox.x);
        await gui.checkScreenshot(page, 'devices-column-reordered', await devices.timestampColumnMasks());
      });

      // Runs immediately after the reorder test above and unconditionally restores the default
      // column order/visibility, regardless of whether that test passed - this is the workspace's
      // self-healing step for this describe block, not just a Reset-feature test.
      test('reset restores default column layout', async () => {
        const dialog = await devices.openColumnConfig('status');
        await dialog.reset();
        await dialog.confirmReset();
        await gui.checkScreenshot(page, 'devices-column-reset', await devices.timestampColumnMasks());
      });
    });
  });
});
