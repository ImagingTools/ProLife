// Hardware (Devices) COLLECTION view - declared where it is standard, written where it is not.
//
// The eleven scenarios below are the shape every collection in this product has (filter, sort, paginate,
// open a row's dialog), so they are declared and the kit generates them: the shared page, the per-user
// skip, the filter reset between tests and the timestamp masking all come from there.
//
// `extra` is the rest - the flows only Hardware has (license binding, license files, transfer counters,
// column configuration). It registers inside the SAME shared-page block, so all of it still costs one
// app boot per user rather than two.
//
// Every skip is decided by a permission the server granted this user at login, named right here in the
// declaration. Nothing waits to see whether a button appears and reads its absence as "not permitted".

const fixtures = require('../fixtures/test');
const { defineCollectionSpec } = require('imtcore-gui-testkit/specs/collectionSpec');
const { DeviceCollectionPage } = require('../pages');

const { test, expect } = fixtures;

defineCollectionSpec({ ...fixtures, defineTest: (...args) => fixtures.test(...args) }, {
  title: 'Hardware / collection',
  pageId: 'Devices',
  requires: 'ViewSensors',
  prefix: 'devices',
  createPage: (page) => new DeviceCollectionPage(page),
  scenarios: [
    { name: 'filter-text', title: 'filter - text search', search: 'ias' },
    // By index, not by text: these option lists come from a source-defined enum, not from data.
    // status 0 = "None", usage 1 = "For Internal Purposes", license 1 = "Sensors with license"
    // (DeviceCollectionView.qml / LicenseFilterDelegate.qml).
    { name: 'filter-status-none', title: 'filter - sensor status', filter: 'status', optionIndex: 0 },
    { name: 'filter-usage-internal', title: 'filter - usage (internal use)', filter: 'usage', optionIndex: 1 },
    { name: 'filter-license-with', title: 'filter - license status', filter: 'license', optionIndex: 1 },
    // The Customers list is populated from the customers this user's org can see, so an org-scoped user
    // legitimately has no QUISS entry - `optional` skips on that data state rather than failing.
    { name: 'filter-customer-quiss', title: 'filter - customers', filter: 'customers', option: 'QUISS', optional: true },
    { name: 'filter-creation-date', title: 'filter - creation date preset', dateFilter: 'creationDate', preset: 'Year_Last' },
    {
      name: 'filter-cleared',
      title: 'filter - clear all',
      clearAll: true,
      apply: [{ search: 'ias' }, { filter: 'status', optionIndex: 0 }],
    },
    // sortBy() addresses a column by its header id, not by the visible caption - DevicesPage.acc orders
    // HeaderIds and HeaderNames independently ("Status" -> "status", "Name" -> "licenseName").
    { name: 'sort-status', title: 'sort by status column', sort: 'status' },
    { name: 'pagination', title: 'pagination - page size and navigation', pagination: { size: 50, page: 2 } },
    { name: 'revision-dialog', title: 'revision dialog', command: 'Revision', requires: 'ViewRevisions' },
    { name: 'remove-dialog', title: 'remove confirmation dialog', command: 'Remove', requires: 'RemoveSensor' },
  ],

  extra: (ctx) => {
    // Hardware's context-sensitive commands only DO anything on a row that satisfies a server-side data
    // precondition (DeviceCollectionViewCommandsDelegate.qml). A bare selectRow(0) takes whatever the
    // default sort put first, which may not qualify - the command then silently no-ops instead of being
    // exercised. Each flow below filters for a qualifying row first.

    ctx.test('bind dialog', async () => {
      ctx.requires('BindSensor');
      const devices = ctx.collection;
      await devices.filterFinishedSensors();
      const selected = await devices.selectSensorByMac('70:b3:d5:e3:0b:17');
      test.skip(!selected, 'the seeded binding-dialog sensor is not available');
      await devices.bind();
      const dialog = ctx.page.locator('[objectName="Dialog"][visible]').first();
      await expect(dialog.getByText('RTV Software', { exact: true }).first()).toBeVisible();
      await ctx.gui.checkElementScreenshot(ctx.page, ['Dialog'], 'devices-bind-dialog', { path: ['BoundLicensesTable'] });
      await ctx.gui.dismissDialog(ctx.page);
    });

    // The dialog above only documents its landing state. The rest of its functionality - the nested
    // "Available Licenses" editor, binding/unbinding, and Save -> "Apply changes" project prompt ->
    // a real updateDeviceBindingRequest - is HardwareProductBindingDialog.qml / ...Editor.qml.
    test.describe('bind dialog - full functionality', () => {
      test.beforeEach(() => {
        ctx.requires('BindSensor');
      });

      ctx.test('open "Bind New Licenses", then Cancel discards', async () => {
        const devices = ctx.collection;
        // "Bind New Licenses" only enables once a Product is selected, and the combo auto-selects one
        // only when the row already has a bound license - filter for such a row rather than hoping.
        await devices.filterFinishedSensorsWithLicense();
        await devices.selectRow(0);
        await devices.bind();
        await devices.openBindNewLicenses();
        await ctx.gui.checkScreenshot(ctx.page, 'devices-bind-available-licenses-dialog', [...(await devices.masks()), { path: ['BoundLicensesTable'] }]);
        await devices.cancelBindNewLicenses();
        // Asked geometrically, not by visibility: both pages of this dialog stay in the scene (see
        // isAvailableLicensesShowing), so the picker's rows never stop being `visible` and an
        // expectHidden on them could not pass however well Cancel worked.
        expect(
          await devices.isAvailableLicensesShowing(),
          'the licence picker should no longer be showing after going back'
        ).toBe(false);
        await ctx.gui.dismissDialog(ctx.page);
      });

      ctx.test('bind a new license, then Save applies it via the project prompt', { tag: '@mutating' }, async () => {
        const devices = ctx.collection;
        await devices.filterFinishedSensorsWithLicense();
        await devices.selectRow(0);
        await devices.bind();
        await devices.openBindNewLicenses();
        // "Available Licenses" lists the product's licenses NOT already bound to a device; for some
        // sensors - and once earlier binds in this phase consumed the spares - it is legitimately empty.
        // Dismiss both dialogs before skipping: this is a shared page and a modal blocks the next test.
        if ((await ctx.gui.countAny(ctx.page, ['AvailableLicensesCollection', 'TableRow_0', 'RowCheckBox'])) === 0) {
          await devices.cancelBindNewLicenses();
          await ctx.gui.dismissDialog(ctx.page);
          test.skip(true, 'no available (unbound) licenses to bind for this sensor');
        }
        await devices.checkAvailableLicense(0);
        await devices.confirmBindNewLicenses();
        await ctx.gui.checkScreenshot(ctx.page, 'devices-bind-license-added', [...(await devices.masks()), { path: ['BoundLicensesTable'] }]);
        await devices.saveBinding();
        await ctx.gui.checkScreenshot(ctx.page, 'devices-bind-apply-changes-prompt');
        await devices.confirmProjectPrompt('GUI test project');
        await ctx.gui.waitForStable(ctx.page);
        // A successful updateDeviceBindingRequest closes the dialog.
        await ctx.gui.expectHidden(ctx.page, ['Dialog'], 'Bind dialog should close after a successful save');
      });

      ctx.test('unbind an existing license', async () => {
        ctx.requires('UnbindSensor');
        const devices = ctx.collection;
        await devices.filterFinishedSensorsWithLicense();
        await devices.selectRow(0);
        await devices.bind();
        // Which sensor lands at row 0 is data-dependent per user, and prior binds/unbinds in the run can
        // change it, so a given sensor can open with no used-license row. Nothing to unbind is a data
        // state, not a failure - dismiss and skip, keeping the shared page clean.
        if ((await ctx.gui.countAny(ctx.page, ['BoundLicensesTable', 'TableRow_0'])) === 0) {
          await ctx.gui.dismissDialog(ctx.page);
          test.skip(true, 'sensor has no used license to unbind');
        }
        await devices.unbindLicense(0);
        await ctx.gui.checkScreenshot(ctx.page, 'devices-bind-license-unbound', [...(await devices.masks()), { path: ['BoundLicensesTable'] }]);
        // Discard rather than Save - keeps this independent of the bind test above instead of
        // compounding mutations to the same device across the run.
        await ctx.gui.dismissDialog(ctx.page);
      });
    });

    ctx.test('create license file - validation (non-superuser)', { tag: '@mutating' }, async () => {
      ctx.requires('CreateLicenseFile');
      const devices = ctx.collection;
      // CreateLicenseFile requires inUse, a non-empty MAC and serial, and status "Finished".
      await devices.filterFinishedSensorsWithLicense();
      await devices.selectRow(0);
      await devices.createLicenseFile();
      await ctx.gui.checkScreenshot(ctx.page, 'devices-create-license-file', await devices.masks());
      await ctx.gui.dismissDialog(ctx.page);
    });

    ctx.test('create license file - "No license is linked" error (non-superuser)', async () => {
      ctx.requires('CreateLicenseFile');
      const devices = ctx.collection;
      // Deliberately a row that fails the FIRST validation check (inUse === false), so the error path
      // stays covered instead of only ever being hit by accident.
      await devices.filterSensorsWithoutLicense();
      await devices.selectRow(0);
      await devices.createLicenseFile();
      await ctx.gui.checkScreenshot(ctx.page, 'devices-create-license-file-no-license-error', await devices.masks());
      await ctx.gui.dismissDialog(ctx.page);
    });

    // The superuser gets an Encrypt/Unencrypt popup instead of the two outcomes above - that flow is
    // devices.license-file.test.js, pinned to `su`.

    ctx.test('transfer licenses dialog', async () => {
      ctx.requires('TransferLicenses');
      const devices = ctx.collection;
      // Requires inUse - without a bound license this opens the "No license is linked" error instead.
      await devices.filterSensorsWithLicense();
      await devices.selectRow(0);
      await devices.transferLicenses();
      await ctx.gui.checkScreenshot(ctx.page, 'devices-transfer-licenses-dialog', await devices.masks());
      await ctx.gui.dismissDialog(ctx.page);
    });

    ctx.test('reset transfer counter', { tag: '@mutating' }, async () => {
      ctx.requires('ResetTransferCounter');
      const devices = ctx.collection;
      // No hard precondition, but a device with a license/transfer history makes the reset meaningful
      // rather than a no-op on an untouched sensor.
      await devices.filterSensorsWithLicense();
      await devices.selectRow(0);
      await devices.resetTransferCounter();
      // Sends the request immediately (no confirm step) and shows an info dialog - dismiss it.
      await ctx.gui.checkScreenshot(ctx.page, 'devices-reset-transfer-counter', await devices.masks());
      await ctx.gui.dismissDialog(ctx.page);
    });

    ctx.test('decrypt file dialog', async () => {
      // DecryptFile opens a native file picker, outside Playwright's DOM - this only records the state
      // the command is invoked from.
      await ctx.gui.checkScreenshot(ctx.page, 'devices-before-decrypt', await ctx.collection.masks());
    });

    // Column configuration: right-click a header -> TableHeaderParamComp.qml. Visibility and order both
    // persist server-side per user (TableViewParams), the same shared-session state as tabs and filters,
    // so each test restores what it changed and the last one resets to defaults unconditionally.
    test.describe.serial('column configuration (header right-click)', () => {
      ctx.test('opens via header right-click', async () => {
        const dialog = await ctx.collection.openColumnConfig('status');
        await ctx.gui.checkScreenshot(ctx.page, 'devices-column-config-dialog');
        await dialog.cancel();
      });

      ctx.test('hide a column via Apply, then restore it', async () => {
        const devices = ctx.collection;
        const dialog = await devices.openColumnConfig('status');
        // "Last Modified" (timeStamp) is appended last in DevicesPage.acc's header list, so it is always
        // the dialog's last row regardless of any prior reordering.
        await dialog.toggleColumn((await dialog.rowCount()) - 1);
        await dialog.apply();
        await ctx.gui.expectHidden(ctx.page, ['TableHeaders', 'timeStamp'], '"Last Modified" should be hidden after Apply');
        await ctx.gui.checkScreenshot(ctx.page, 'devices-column-hidden', await devices.masks());

        const restore = await devices.openColumnConfig('status');
        await restore.toggleColumn((await restore.rowCount()) - 1);
        await restore.apply();
        await ctx.gui.expectVisible(ctx.page, ['TableHeaders', 'timeStamp'], '"Last Modified" should be back after re-checking');
      });

      ctx.test('toggling a column then Cancel discards the change', async () => {
        const dialog = await ctx.collection.openColumnConfig('status');
        await dialog.toggleColumn((await dialog.rowCount()) - 1);
        await dialog.cancel();
        await ctx.gui.expectVisible(ctx.page, ['TableHeaders', 'timeStamp'], 'Cancel must not apply the unchecked column');
      });

      // Asserts the header ORDER, not the x of one header. The pixel version measured "Last Modified",
      // the rightmost column - and by the time this block runs an earlier test has selected a row,
      // which opens the licences panel beside the table and narrows it enough to push that column out
      // of view. The measurement then waited for a header that was never going to appear and burned
      // the whole 60s test timeout on a table that was working correctly.
      ctx.test('reorder columns with Up/Down', async () => {
        const devices = ctx.collection;
        const before = await devices.table.headerOrder();

        const dialog = await devices.openColumnConfig('status');
        // Row 1 is the second column, so Move Up makes it the first. Chosen over the last row because
        // the leftmost columns stay on screen whatever else is open beside the table.
        await dialog.selectColumn(1);
        await dialog.moveUp();
        await dialog.apply();

        const after = await devices.table.headerOrder();
        expect(after[0], 'Move Up + Apply should put the second column first').toBe(before[1]);
        expect(after[1], '... and push the first one after it').toBe(before[0]);
        await ctx.gui.checkScreenshot(ctx.page, 'devices-column-reordered', await devices.masks());
      });

      // Runs right after the reorder above and restores the default layout whether or not that passed -
      // this is the block's self-healing step, not only a test of the Reset feature.
      ctx.test('reset restores default column layout', async () => {
        const dialog = await ctx.collection.openColumnConfig('status');
        await dialog.reset();
        await dialog.confirmReset();
        await ctx.gui.checkScreenshot(ctx.page, 'devices-column-reset', await ctx.collection.masks());
      });
    });
  },
});
