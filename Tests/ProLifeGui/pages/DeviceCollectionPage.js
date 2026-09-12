// DeviceCollectionPage - the Hardware page (pageId "Devices") collection view.
//
// Grounded in:
//   ProLife/Qml/Include/prolifeqml/DeviceCollectionView.qml            (filters registered)
//   ProLife/Qml/Include/prolifeqml/DeviceCollectionViewCommandsDelegate.qml (commands + dialogs)
//   ImtCore RemoteCollectionView/CollectionView/TableBase/Pagination/FilterPanelDecorator (base UI)
//
// Command bar ids (button objectName = "<id>Button", under CommandsView):
//   New, Edit, Remove, Revision                          (base collection commands)
//   Bind, CreateLicenseFile, TransferLicenses,           (custom, enabled when exactly 1 row selected;
//   ResetTransferCounter, DecryptFile, Support            Bind also needs a non-empty MAC)
//
// Filters (FilterPanel -> "<filterId>", options are text-keyed, each has a ClearButton; the panel has
// ClearAllFilters and a built-in CreationDateFilter + SearchTextInput):
//   SensorStatusFilter, UsageFilter, LicenseFilter, LicenseCreationDateFilter,
//   CustomersFilter, ProductsFilter, LicensesFilter
//
// Commands are permission-gated server-side (New<-AddSensor, Remove<-RemoveSensor,
// Bind<-BindSensor, CreateLicenseFile<-CreateLicenseFile, TransferLicenses<-TransferLicenses, ...),
// so a test asks commands.isAvailable(id) rather than predicting who holds what.

const { CollectionPage } = require('imtcore-gui-testkit/pages/CollectionPage');
const { TableConfigDialog } = require('imtcore-gui-testkit/controls');
const gui = require('imtcore-gui-testkit/lib/gui');

// "Added"/"Last Modified" column header ids (DevicesPage.acc's CollectionHeadersProvider /
// DeviceItemTypeMetaInfo.s_added / s_timeStamp) - their values change over time and after edits, so
// they're masked in screenshots rather than asserted on.
const TIMESTAMP_HEADER_IDS = ['added', 'timeStamp'];

// Filter ids as registered in DeviceCollectionView.qml / FilterPanelDecorator.qml.
const FILTERS = {
  status: 'SensorStatusFilter',
  usage: 'UsageFilter',
  license: 'LicenseFilter',
  licenseCreationDate: 'LicenseCreationDateFilter',
  customers: 'CustomersFilter',
  products: 'ProductsFilter',
  licenses: 'LicensesFilter',
  creationDate: 'CreationDateFilter',
};

class DeviceCollectionPage extends CollectionPage {
  constructor(page) {
    // Declared to the base class rather than reimplemented here: CollectionPage.filterId() turns a
    // short key ('status') into the real objectName ('SensorStatusFilter'), and masks() covers the
    // timestamp columns. Without these every filter key falls through as its own objectName and
    // nothing resolves - which is exactly what a live hardwareManager run caught.
    super(page, 'Devices', { filters: FILTERS, maskColumns: TIMESTAMP_HEADER_IDS });
  }

  // --- custom commands (each throws if the command button is missing/hidden) --------------------

  bind() {
    return this.runCommand('Bind');
  }
  createLicenseFile() {
    return this.runCommand('CreateLicenseFile');
  }
  transferLicenses() {
    return this.runCommand('TransferLicenses');
  }
  resetTransferCounter() {
    return this.runCommand('ResetTransferCounter');
  }
  decryptFile() {
    return this.runCommand('DecryptFile');
  }
  support() {
    return this.runCommand('Support');
  }

  // --- CreateLicenseFile's Encrypt/Unencrypt choice (superuser only) -----------------------------
  // DeviceCollectionViewCommandsDelegate.qml's onCreateLicenseFile() only shows this popup for
  // AuthorizationController.loggedUserIsSuperuser() - every other user's CreateLicenseFile call
  // skips straight to sending the request (or a validation error dialog). The popup is opened
  // directly (not via a ComboBox), and its items aren't text-addressable (PopupMenuDialog's own
  // default delegate doesn't override objectName like ComboBox.qml's does) - only position works:
  // index 0 = "Encrypted" (Encrypt), index 1 = "Unencrypted" (NotEncrypt), per the addItem() order
  // in encryptPopupMenuModel.
  chooseEncrypted() {
    return gui.clickPopupItemByIndex(this.page, 0);
  }
  chooseNotEncrypted() {
    return gui.clickPopupItemByIndex(this.page, 1);
  }

  // --- Bind dialog (HardwareProductBindingDialog.qml) --------------------------------------------
  // Opened by bind(). ONE dialog with two pages that slide: the bound-licence list, and a "Select
  // Licenses" picker. It used to be two separate dialogs (the picker lived in its own
  // HardwareProductBindingEditor.qml, since removed), which is why the flows below read as if they
  // opened and closed a nested window - they now just move between pages.
  //
  // Both pages are in the scene at once, so a bare ['Dialog', 'TableRow_0'] is ambiguous between the
  // two lists: each is addressed through its own name instead. The controls carry explicit objectNames
  // added for these tests - the captions they would otherwise be named after either change with the
  // selection ("Bind" -> "Bind (2)") or do not exist at all on the icon-only buttons.
  // Its own footer is still Save/Close ("SaveButton"/"CloseButton", Enums.ok/cancel).

  /** Row index in the bound-licences table (the dialog's first page). */
  selectUsedLicenseRow(index) {
    return gui.click(this.page, ['BoundLicensesTable', `TableRow_${index}`], { what: `bound licence row ${index}` });
  }
  /** Unbind is a per-ROW button in that table, not a dialog-level command. */
  unbindLicense(index = 0) {
    return gui.clickButton(this.page, ['BoundLicensesTable', `TableRow_${index}`, 'UnbindLicenseButton']);
  }
  /** Slides to the licence picker ("Select Licenses"). */
  openBindNewLicenses() {
    return gui.clickButton(this.page, ['Dialog', 'OpenAvailableLicensesButton']);
  }
  /** Toggle a licence's checkbox by row index in the picker's own list. */
  checkAvailableLicense(index) {
    return gui.click(this.page, ['AvailableLicensesCollection', `TableRow_${index}`, 'RowCheckBox'], {
      what: `available licence row ${index} checkbox`,
    });
  }
  /** Binds the checked licences and slides back to the bound list. */
  confirmBindNewLicenses() {
    return gui.clickButton(this.page, ['Dialog', 'ConfirmBindLicensesButton']);
  }
  /** Returns to the bound list without binding anything. */
  cancelBindNewLicenses() {
    return gui.clickButton(this.page, ['Dialog', 'BackToBoundLicensesButton']);
  }
  /** Save (Enums.ok) - opens the "Apply changes" project-name prompt (notClosingButtons keeps the
   * Bind dialog itself open underneath). Only enabled once a product is selected and the binding
   * model actually changed (HardwareProductBindingDialog.qml's onModelChanged handler). */
  saveBinding() {
    return gui.clickButton(this.page, ['Dialog', 'SaveButton']);
  }
  /**
   * Fill + confirm the "Apply changes" / "Please enter the project" InputDialog opened by Save.
   * Save's `notClosingButtons: Enums.ok` (HardwareProductBindingDialog.qml) keeps the Bind dialog
   * itself open underneath while this prompt is up, so BOTH share objectName "Dialog" at once - a
   * path-based ['Dialog', ...] lookup's `.first()` would land on the (earlier-opened) Bind dialog
   * behind it, not this prompt. It's opened later/on top, so `.last()` reaches the actual prompt.
   */
  async confirmProjectPrompt(project) {
    const prompt = this.page.locator('[objectName="Dialog"][visible]').last();
    // Not a real <input> (a synthetic QML/WASM text item), so type via mouse-click + keyboard like
    // actions.js's own fill() does, rather than Playwright's .fill() (which requires a real form field).
    const input = prompt.locator('[objectName="TextInput"] input, input[objectName="TextInput"], [objectName="TextInput"]').first();
    await input.waitFor({ state: 'visible', timeout: 5000 });
    const inputBox = await input.boundingBox();
    if (!inputBox) throw new Error('GUI fill target TextInput has no bounding box: Apply changes project field');
    await this.page.mouse.click(inputBox.x + inputBox.width / 2, inputBox.y + inputBox.height / 2);
    await this.page.keyboard.type(project);
    await gui.waitForStable(this.page);

    const okMouse = prompt.locator('[objectName="OKButton"] [objectName="MouseArea"][visible]').first();
    await okMouse.waitFor({ state: 'visible', timeout: 5000 });
    const box = await okMouse.boundingBox();
    if (!box) throw new Error('GUI click target has no bounding box: Apply changes OK button');
    await this.page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
    await gui.waitForStable(this.page);
  }

  // --- column configuration (right-click a header) -----------------------------------------------

  /** Open the "Table configuration" dialog by right-clicking a column header. */
  async openColumnConfig(headerId = 'status') {
    const dialog = new TableConfigDialog(this.page);
    await dialog.openViaHeader(headerId);
    return dialog;
  }

  /** Screenshot masks for the Added/Last Modified columns - see TIMESTAMP_HEADER_IDS above. */
  timestampColumnMasks() {
    return this.columnMasks(TIMESTAMP_HEADER_IDS);
  }

  // --- filters ----------------------------------------------------------------------------------

  // --- precondition filters for context-sensitive commands --------------------------------------
  // Bind/CreateLicenseFile/TransferLicenses/ResetTransferCounter only actually DO something on a row
  // that satisfies a server-side data precondition (DeviceCollectionViewCommandsDelegate.qml); a bare
  // selectRow(0) picks whatever the table's default sort happens to put first, which may not qualify
  // (e.g. no MAC address, no license), making the command silently no-op instead of exercising it.
  // These filter first so row 0 is guaranteed to be a row the command is actually meant to act on.

  /** Rows with SensorStatus "Finished" (index 5) - Finished sensors always carry a MAC address. */
  async filterFinishedSensors() {
    await this.clearAllFilters();
    await this.selectFilterOptionByIndex('status', 5);
    return this;
  }

  /** Rows with LicenseStatus "Sensors with license" (index 1) - satisfies s_inUse === true. */
  async filterSensorsWithLicense() {
    await this.clearAllFilters();
    await this.selectFilterOptionByIndex('license', 1);
    return this;
  }

  /** Rows with LicenseStatus "Sensors without a license" (index 0) - s_inUse === false, the first
   * precondition CreateLicenseFile's validation checks (DeviceCollectionViewCommandsDelegate.qml:423-427)
   * - guarantees the "No license is linked" error path instead of hoping row 0 happens to lack one. */
  async filterSensorsWithoutLicense() {
    await this.clearAllFilters();
    await this.selectFilterOptionByIndex('license', 0);
    return this;
  }

  /** Finished AND licensed - CreateLicenseFile's precondition (inUse + MAC + serial + status). */
  async filterFinishedSensorsWithLicense() {
    await this.clearAllFilters();
    await this.selectFilterOptionByIndex('status', 5);
    await this.selectFilterOptionByIndex('license', 1);
    return this;
  }

  static get FILTERS() {
    return FILTERS;
  }
}

module.exports = { DeviceCollectionPage, FILTERS };
