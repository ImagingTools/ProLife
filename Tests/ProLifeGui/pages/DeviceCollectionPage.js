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
// ClearAllFilters and a built-in CreationDateFilter + DocumentState + SearchTextInput):
//   SensorStatusFilter, UsageFilter, LicenseFilter, LicenseCreationDateFilter,
//   CustomersFilter, ProductsFilter, LicensesFilter, DateFilter, DocumentState
//
// Commands are permission-gated server-side (New<-AddSensor, Remove<-RemoveSensor,
// Bind<-BindSensor, CreateLicenseFile<-CreateLicenseFile, TransferLicenses<-TransferLicenses, ...);
// see matrix/permissions.js.

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
  documentState: 'DocumentState',
};

class DeviceCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'Devices');
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
  // Opened by bind(). Its own footer is Save/Close ("SaveButton"/"CloseButton", Enums.ok/cancel).
  // "Bind New Licenses" opens a SECOND Dialog (HardwareProductBindingEditor.qml, "Available
  // Licenses") on top of it - NOT nested inside it in the DOM (confirmed live: both dialogs' roots
  // carry objectName "Dialog" as PARALLEL/sibling overlays, each with exactly one "Dialog" ancestor in
  // its own controls' chain, not two), so a single ['Dialog', ...] scope already reaches whichever
  // dialog's own control it names - it just needs to not collide with an IDENTICALLY-named control
  // elsewhere. The nested editor's footer is Bind/Cancel ("BindButton"/"CancelButton"); the command
  // bar's own "Bind" command is ALSO "BindButton" but has NO Dialog ancestor at all, so scoping to
  // ['Dialog', 'BindButton'] unambiguously reaches the nested editor's confirm button, never the
  // command bar's. The outer dialog's own footer (Save/Close) never collides with the nested editor's
  // (Bind/Cancel), since the two dialogs are only ever open one-at-a-time from each other's own
  // controls' perspective in the flows below.

  /** Row index in the (outer dialog's) "Used Licenses" table. */
  selectUsedLicenseRow(index) {
    return gui.click(this.page, ['Dialog', `TableRow_${index}`], { what: `used license row ${index}` });
  }
  unbindLicense() {
    return gui.clickButton(this.page, ['Dialog', 'UnbindButton']);
  }
  /** Opens the nested "Available Licenses" (Bind New Licenses) dialog. */
  openBindNewLicenses() {
    return gui.clickButton(this.page, ['Dialog', 'BindNewLicensesButton']);
  }
  /** Toggle a license's checkbox by row index in the nested "Available Licenses" dialog's table. */
  checkAvailableLicense(index) {
    return gui.click(this.page, ['Dialog', `TableRow_${index}`, 'RowCheckBox'], {
      what: `available license row ${index} checkbox`,
    });
  }
  /** Confirms the nested dialog (Enums.ok, "Bind") - binds the checked license(s) and closes it. */
  confirmBindNewLicenses() {
    return gui.clickButton(this.page, ['Dialog', 'BindButton']);
  }
  /** Cancels the nested dialog (Enums.cancel, "Cancel") without binding anything. */
  cancelBindNewLicenses() {
    return gui.clickButton(this.page, ['Dialog', 'CancelButton']);
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

  /** Pick an option in a registered field filter by its option text (e.g. status "None"). */
  async selectFilterOption(filterKey, optionText) {
    const filterId = FILTERS[filterKey] || filterKey;
    await this.filters.combo(filterId).select(optionText);
    return this;
  }

  /**
   * Pick an option in a registered field filter BY POSITION (see DeviceCollectionView.qml for the
   * source-defined option order backing each filterKey - e.g. 'status' is
   * None/Accepted/InProgress/Canceled/OnHold/Finished/Defect/InRepair/Decommissioned, indices 0-8;
   * 'license' is WithoutLicense/WithLicense, indices 0-1, per LicenseFilterDelegate.qml).
   */
  async selectFilterOptionByIndex(filterKey, index) {
    const filterId = FILTERS[filterKey] || filterKey;
    await this.filters.combo(filterId).selectIndex(index);
    return this;
  }

  /** Open the built-in creation-date filter and pick a preset ('Month_Current', 'Year_Last', ...). */
  async setCreationDate(preset) {
    await this.filters.dateFilter(FILTERS.creationDate, preset);
    return this;
  }

  /** Clear one registered filter. */
  async clearFilter(filterKey) {
    await this.filters.clearFilter(FILTERS[filterKey] || filterKey);
    return this;
  }

  /** Clear every active filter. */
  async clearAllFilters() {
    await this.filters.clearAllFilters();
    return this;
  }

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
