// Hardware (Devices) EDITOR - full functional coverage, multi-user.
//
// The editor is a document tab opened by the "New" or "Edit" command (see DeviceEditorPage.js for the
// Document Service flow). Only users who can open it (ViewSensors, plus AddSensor for New) run the
// interaction bodies; field editability is permission-driven (DeviceEditor.checkPermissions), so the
// per-user screenshots + the structural "editable fields" test encode the permission matrix.
//
// Both describe blocks below are `.serial` and share ONE page/document across all their steps (see
// fixtures/test.js's newUserPage) instead of a fresh page+reload per test: "fill a field, then another,
// then save" is already a deliberate narrative sequence, and ProLife keeps the document open server-side
// between actions anyway (MultiDocumentCollectionView.qml), so this also better matches how the app is
// actually used. Trade-off: a failure partway through a block skips the remaining steps in that block.

const { test, newUserPage } = require('../fixtures/test');
const { DeviceCollectionPage, DeviceEditorPage } = require('../pages');
const gui = require('imtcore-gui-testkit/lib/gui');

// Returns null when this user cannot get to a new-sensor editor at all - the page is not in their
// menu, or the collection offers no New command. The caller test.skip()s on that rather than failing.
async function openNewEditor(page) {
  const devices = new DeviceCollectionPage(page);
  await devices.reload();
  if (!(await devices.isAvailable())) return null;
  await devices.open();
  if (!(await devices.commands.isAvailable('New'))) return null;
  await devices.newItem(); // "New" -> fresh document tab
  return new DeviceEditorPage(page);
}

async function openEditEditor(page) {
  const devices = new DeviceCollectionPage(page);
  await devices.reload();
  if (!(await devices.isAvailable())) return null;
  await devices.open();
  // Sort by "added" (creation date, header id "added" - DevicesPage.acc) before picking row 0: the
  // default (unsorted) view's row 0 is whichever device the server currently orders first, and that
  // ordering shifts whenever ANOTHER test mutates a device's Last Modified timestamp - which several
  // @mutating devices.collection tests (bind, create-license-file, reset-transfer-counter) do, and this
  // phase runs serially after them. Without a stable sort, row 0 here can silently become a DIFFERENT
  // device than whichever one the baseline screenshots were captured against, showing as every single
  // field mismatching (confirmed live). "added" is immutable creation metadata none of those mutations
  // touch, so sorting by it pins row 0 to the same device regardless of what ran earlier in this phase.
  await devices.table.sortBy('added');
  await devices.selectRow(0);
  await devices.editItem(); // "Edit" -> loads selected doc via GetDeviceRepresentation
  return new DeviceEditorPage(page);
}

test.describe('Hardware / editor', () => {
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
      test.skip(!editor, 'creating a sensor is not available to this user');
    });

    test('empty new editor', async () => {
      await gui.checkScreenshot(page, 'device-editor-new-empty');
    });

    // DeviceValidator.qml blocks Save until Device Type AND Configuration are both selected, and it
    // now surfaces that INLINE - a red message under each unset combo, with Save disabled - rather than
    // as a modal error dialog on click (same shape the Orders editor already uses for Delivery-ID).
    // The message Text carries no objectName, so this asserts on its visible text.
    test('save blocked - missing required fields', async () => {
      // A truly untouched document isn't dirty yet, so touch a field the validator does NOT check
      // (Description) first, leaving Device Type/Configuration unset.
      await editor.setDescription('Touch to dirty');
      // clickSave, not save(): the point here is that Save is REFUSED, and save() drives to a committed
      // document and fails if it never gets one.
      await editor.clickSave();
      await page.getByText('Please select a device type').first().waitFor({ state: 'visible', timeout: 10000 });
      await gui.checkScreenshot(page, 'device-editor-save-blocked-empty');
    });

    test('fill device information group', async () => {
      // Both combos are populated from the live product catalogue (seeded/DB data, not a source-level
      // enum), so there's no stable caption to assert on - pick the first entry by position instead.
      await editor.setDeviceTypeByIndex(0);
      await gui.checkScreenshot(page, 'device-editor-new-device-type');
      await editor.setHardwareConfigurationByIndex(0); // dependent combo populated by device type
      await gui.checkScreenshot(page, 'device-editor-new-configuration');
      await editor.setDescription('ProLifeGui test device');
      await gui.checkScreenshot(page, 'device-editor-new-description');
    });

    test('mac address validation - invalid then valid', async () => {
      await editor.setMacAddress('12345');
      await gui.checkScreenshot(page, 'device-editor-mac-invalid');
      await editor.setMacAddress('11:11:11:11:11:11');
      await gui.checkScreenshot(page, 'device-editor-mac-valid');
    });

    test('serial number + mac triggers Finished-status confirm dialog', async () => {
      await editor.setSerialNumber('SN-PROLIFEGUI-1');
      await editor.setMacAddress('22:22:22:22:22:22');
      // Entering a valid serial + mac raises the "set production status to Finished?" MessageDialog.
      await gui.checkScreenshot(page, 'device-editor-finished-confirm');
      // Decline - the next step sets a different status explicitly via the combo. Must dismiss here
      // (rather than leave it for the next test to deal with): this document stays open for the rest
      // of the chain, so a lingering modal would block every subsequent step's clicks.
      await gui.clickButton(page, ['NoButton']);
    });

    test('production page - order / status / project / internal use', async () => {
      await editor.setProductionStatusByIndex(2); // "In Progress" (DeviceProductionStatus.qml)
      await gui.checkScreenshot(page, 'device-editor-status');
      await editor.setProject('ProLifeGui Project');
      await gui.checkScreenshot(page, 'device-editor-project');
      await editor.toggleInternalUse();
      await gui.checkScreenshot(page, 'device-editor-internal-use');
    });

    test('collapse / expand groups', async () => {
      await editor.toggleGroup('device');
      await gui.checkScreenshot(page, 'device-editor-device-group-collapsed');
      await editor.toggleGroup('device');
      await editor.toggleGroup('production');
      await gui.checkScreenshot(page, 'device-editor-additional-group-collapsed');
      await editor.toggleGroup('production'); // leave groups expanded for the remaining steps
    });

    // DocumentHistoryPanel.qml is embedded identically in every document editor (only rendered at all
    // when the user may see revisions - PermissionsController.checkPermission at Component.onCompleted) -
    // a generic mechanic with no per-entity logic, covered once here for the whole suite (same reasoning
    // as group-collapse/undo-redo/dirty-close-confirm above).
    test('document history panel toggles open and closed', async () => {
      test.skip(!(await gui.dom.isVisible(page, ['HistoryPanelToggle'])), 'history panel is not available to this user');
      await gui.clickButton(page, ['HistoryPanelToggle']);
      await gui.checkScreenshot(page, 'device-editor-history-panel-open');
      await gui.clickButton(page, ['HistoryPanelToggle']);
      await gui.checkScreenshot(page, 'device-editor-history-panel-closed');
    });

    test('undo / redo', async () => {
      // Undo steps back ONE edit, and this block shares one document with the steps above - so the
      // field reverts to whatever they left in it, not to empty. Read it first rather than assuming.
      await editor.openEditorPage('Production');
      const before = await editor.project.value();
      await editor.setProject('Undo me');
      await editor.undo();
      // Undo's field-value revert can lag its own command-settle under concurrent-worker load (a
      // screenshot taken right after can stably still show the pre-undo text) - wait for the actual
      // reverted value rather than trusting generic DOM-quiet. See TextInput.waitForValue's comment.
      await editor.project.waitForValue(before);
      await gui.checkScreenshot(page, 'device-editor-after-undo');
      await editor.redo();
      await editor.project.waitForValue('Undo me');
      await gui.checkScreenshot(page, 'device-editor-after-redo');
    });

    test('editor commands require save first', async () => {
      test.skip(!(await editor.commands.isAvailable('Bind')), 'Bind is not available to this user');
      // Bind on an unsaved document -> "Please save the document first" info dialog.
      await editor.bind();
      await gui.checkScreenshot(page, 'device-editor-bind-needs-save');
      // This document stays open for the rest of the chain (shared page) - dismiss so the info
      // dialog doesn't block the next step's clicks.
      await gui.dismissDialog(page);
    });

    // Support is a sub-page of the editor now, not a command-bar command opening a dialog. On a document
    // that has never been saved it has no ticket panel to show - a ticket hangs off the saved device
    // record - so what it shows instead is the reason why, which is what this captures.
    test('support sub-page explains that tickets need a saved device', async () => {
      await editor.openSupport();
      await page.getByText('Tickets are available after saving').first().waitFor({ state: 'visible', timeout: 10000 });
      await gui.checkScreenshot(page, 'device-editor-support-page-unsaved');
    });

    // Last step: closes the document tab, ending this chain.
    test('close dirty document -> confirm dialog', async () => {
      await editor.setProject('dirty');
      await editor.closeDocument();
      await gui.checkScreenshot(page, 'device-editor-close-dirty');
    });
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
      test.skip(!editor, 'Hardware is not available to this user');
    });

    test('open existing sensor editor', async () => {
      await gui.checkScreenshot(page, 'device-editor-edit-loaded');
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
      await gui.checkScreenshot(page, 'device-editor-edit-changed');

      // (Previously also spied on the "UpdateDeviceFromRepresentation" GraphQL call here to confirm
      // the server accepted the mutation - removed: it never matched a real request within the
      // timeout, so either the operation name assumption or the request shape was wrong. The reopen
      // check below is the reliable persistence signal already - it fails loudly on its own if Save
      // didn't really commit.)
      await editor.save();
      await gui.checkScreenshot(page, 'device-editor-edit-saved');

      // Persistence check: a Save that only LOOKS successful in the client's own state (but never
      // actually round-tripped/committed server-side) would still pass every assertion above - closing
      // and reopening the SAME document from a clean collection reload proves the new value was really
      // written, not just held in this still-open document's in-memory representation.
      await editor.closeDocument();
      editor = await openEditEditor(page);
      // Project lives on the Production sub-page and a reopened editor lands on Device, so without
      // this the field was invisible - and waitForValue used to treat "not there" as nothing to wait
      // for, passing a persistence check that had read nothing at all.
      await editor.openEditorPage("Production");
      await editor.project.waitForValue(edited);
      await gui.checkScreenshot(page, 'device-editor-edit-reopened');
    });
  });
});
