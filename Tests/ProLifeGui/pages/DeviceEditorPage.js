// DeviceEditorPage - the multi-tab document editor opened from the Hardware page (New / Edit).
//
// Document Service flow (ProLife/Qml/Include/prolifeqml/DeviceCollectionViewCommandsDelegate.qml):
//   * "New"  -> MainDocumentService opens a fresh document + a new tab bound to an empty DeviceData
//              (documentIsNew = true).
//   * "Edit" -> opens the selected row's document in a tab; DocumentRepresentationController.
//              updateRepresentationFromDocument() sends the GetDeviceRepresentation GraphQL request
//              and fills the editor's DeviceData model.
//   * Editing any field -> doUpdateModel() mutates DeviceData -> the document becomes "dirty".
//   * "Save" -> updateDocumentFromRepresentation() sends UpdateDeviceFromRepresentation(documentId,
//              deviceData); on status "Success" the document is committed.
//   * "Undo"/"Redo" -> walk the document undo stack.
//   * Closing a dirty tab -> a confirm dialog (Save/Discard/Cancel).
// Editor commands Bind / CreateLicenseFile / TransferLicenses require the document to be saved first
// (the delegate shows "Please save the document first" otherwise).
//
// All field objectNames below were added to ProLife/Qml/Include/prolifeqml/DeviceEditor.qml.

const { BasePage } = require('imtcore-gui-testkit/pages/BasePage');
const { ComboBox, TextInput, Switch } = require('imtcore-gui-testkit/controls');
const gui = require('imtcore-gui-testkit/lib/gui');

class DeviceEditorPage extends BasePage {
  constructor(page) {
    // No MenuPanel pageId - the editor is a document tab, not a page. Reuse BasePage for the command
    // bar / tab handling only.
    super(page, 'Devices');

    // Device Information group
    this.deviceType = new ComboBox(page, ['DeviceTypeCombo']);
    this.hardwareConfiguration = new ComboBox(page, ['HardwareConfigurationCombo']);
    this.article = new TextInput(page, ['ArticleInput']); // read-only
    this.description = new TextInput(page, ['DescriptionInput']);
    this.serialNumber = new TextInput(page, ['SerialNumberInput']);
    this.macAddress = new TextInput(page, ['MacAddressInput']);

    // Additional Information group
    this.order = new ComboBox(page, ['OrderCombo']);
    this.productionStatus = new ComboBox(page, ['ProductionStatusCombo']);
    this.project = new TextInput(page, ['ProjectInput']);
    this.internalUse = new Switch(page, ['InternalUseSwitch']);
  }

  // --- editor commands --------------------------------------------------------------------------

  save() {
    return this.runCommand('Save');
  }
  undo() {
    return this.runCommand('Undo');
  }
  redo() {
    return this.runCommand('Redo');
  }
  bind() {
    return this.runCommand('Bind');
  }
  createLicenseFile() {
    return this.runCommand('CreateLicenseFile');
  }
  transferLicenses() {
    return this.runCommand('TransferLicenses');
  }
  support() {
    return this.runCommand('Support');
  }

  // --- field helpers ----------------------------------------------------------------------------

  // DeviceTypeCombo/HardwareConfigurationCombo are populated from the live product catalogue
  // (DeviceEditor.qml: model: CachedProductCollection.hardwareProductsModel) - seeded/DB-driven, not a
  // source-level enum, so there is no stable caption to hardcode here. Select by position instead.
  async setDeviceTypeByIndex(index = 0) {
    await this.deviceType.selectIndex(index);
    return this;
  }
  async setHardwareConfigurationByIndex(index = 0) {
    await this.hardwareConfiguration.selectIndex(index);
    return this;
  }
  async setDescription(text) {
    await this.description.fill(text);
    return this;
  }
  async setSerialNumber(text) {
    await this.serialNumber.fill(text, { verify: false });
    return this;
  }
  async setMacAddress(text) {
    // verify:false because MacAddressElementView reformats/validates input as you type.
    await this.macAddress.fill(text, { verify: false });
    return this;
  }
  async setProject(text) {
    await this.project.fill(text);
    return this;
  }
  // ProductionStatusCombo is a static enum (DeviceProductionStatus.qml): None/Accepted/InProgress(2)/
  // Canceled/OnHold/Finished(5)/Defected/InRepair/Decommissioned, indices 0-8.
  async setProductionStatusByIndex(index) {
    await this.productionStatus.selectIndex(index);
    return this;
  }
  async toggleInternalUse() {
    await this.internalUse.toggle();
    return this;
  }

  // --- group collapse/expand --------------------------------------------------------------------

  async toggleGroup(which) {
    const header = {
      device: 'DeviceInformationHeader',
      additional: 'AdditionalInformationHeader',
      license: 'LicenseInformationHeader',
    }[which];
    if (!header) throw new Error(`Unknown editor group "${which}"`);
    await gui.clickButton(this.page, [header]);
    return this;
  }

  // --- structural expectations (used by permission tests) ---------------------------------------

  expectFieldVisible(objectName) {
    return gui.expectVisible(this.page, [objectName]);
  }
}

module.exports = { DeviceEditorPage };
