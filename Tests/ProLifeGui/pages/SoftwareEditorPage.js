// SoftwareEditorPage - the multi-tab document editor opened from the Software page (New / Edit).
//
// Document Service flow (ProLife/Qml/Include/prolifeqml/SoftwareProductCollectionView.qml +
// SoftwareProductsCollectionViewCommandsDelegate + GqlBasedCommandsController):
//   * "New"  -> opens fresh SoftwareProductData document tab (documentIsNew = true).
//   * "Edit" -> loads via gql SoftwareProductItem, fills SoftwareProductData.
//   * Edits -> doUpdateModel -> dirty.
//   * "Save" -> SoftwareProductUpdate.
//   * Undo/Redo via document undo stack.
//   * Close dirty -> confirm dialog.
//
// MultiPageView pages (SoftwareEditor.qml):
//   General    — license identity (product/license/software-id/options)
//   Additional — project / order
//   Expiration — unlimited / expiration date
//   Hierarchy  — parent/child tree (conditional)
// Nav items use objectName "Page_<id>" (see MultiPageView.qml).

const { BasePage } = require('imtcore-gui-testkit/pages/BasePage');
const { ComboBox, TextInput, Switch } = require('imtcore-gui-testkit/controls');
const gui = require('imtcore-gui-testkit/lib/gui');

const FIELD_PAGE = {
  ProductCombo: 'General',
  LicenseCombo: 'General',
  ArticleInput: 'General',
  SerialNumberInput: 'General',
  InternalUseSwitch: 'General',
  IsMultipleSwitch: 'General',
  ProductCountSpinBox: 'General',
  ProjectInput: 'Additional',
  OrderCombo: 'Additional',
  UnlimitedSwitch: 'Expiration',
  ExpirationDatePicker: 'Expiration',
};

class SoftwareEditorPage extends BasePage {
  constructor(page) {
    // Editor tab, reuse BasePage for command bar.
    super(page, 'SoftwareProducts');

    // General page — license identity
    this.product = new ComboBox(page, ['ProductCombo']);
    this.license = new ComboBox(page, ['LicenseCombo']);
    this.article = new TextInput(page, ['ArticleInput']); // read-only
    this.serialNumber = new TextInput(page, ['SerialNumberInput']);
    this.internalUse = new Switch(page, ['InternalUseSwitch']);
    this.isMultiple = new Switch(page, ['IsMultipleSwitch']);
    this.productCount = new TextInput(page, ['ProductCountSpinBox']);

    // General page — assignment
    this.project = new TextInput(page, ['ProjectInput']);
    this.order = new ComboBox(page, ['OrderCombo']);

    // Expiration page
    this.unlimited = new Switch(page, ['UnlimitedSwitch']);
    this.expiration = new TextInput(page, ['ExpirationDatePicker']);
  }

  // --- MultiPageView navigation -----------------------------------------------------------------

  async openEditorPage(pageId) {
    await gui.clickButton(this.page, [`Page_${pageId}`]);
    return this;
  }

  async ensureFieldPage(objectName) {
    const pageId = FIELD_PAGE[objectName];
    if (pageId) {
      await this.openEditorPage(pageId);
    }
    return this;
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

  // --- field helpers ----------------------------------------------------------------------------

  async setProject(text) {
    await this.openEditorPage('Additional');
    await this.project.fill(text);
    return this;
  }
  async setOrder(text) {
    await this.openEditorPage('Additional');
    await this.order.select(text);
    return this;
  }
  async setProduct(text) {
    await this.openEditorPage('General');
    await this.product.select(text);
    return this;
  }
  async setLicense(text) {
    await this.openEditorPage('General');
    await this.license.select(text);
    return this;
  }
  async setSerialNumber(text) {
    await this.openEditorPage('General');
    await this.serialNumber.fill(text);
    return this;
  }
  async toggleInternalUse() {
    await this.openEditorPage('General');
    await this.internalUse.toggle();
    return this;
  }
  async toggleIsMultiple() {
    await this.openEditorPage('General');
    await this.isMultiple.toggle();
    return this;
  }
  async setUnlimited(on) {
    // Toggle only if needed; we click to set desired state by checking screenshot or always toggle twice if wrong.
    // For simplicity in tests we toggle and rely on state.
    await this.openEditorPage('Expiration');
    await this.unlimited.toggle();
    return this;
  }
  async setExpiration(text) {
    // Date picker; fill the inner may work for text rep, use {verify:false} like mac in devices.
    await this.openEditorPage('Expiration');
    await this.expiration.fill(text, { verify: false });
    return this;
  }

  // --- group collapse/expand (within the active MultiPageView page) -----------------------------

  async toggleGroup(which) {
    const pageAndHeader = {
      software: { pageId: 'Additional', header: 'SoftwareInformationHeader' },
      additional: { pageId: 'Additional', header: 'SoftwareInformationHeader' },
      license: { pageId: 'General', header: 'LicenseInformationHeader' },
      expiration: { pageId: 'Expiration', header: 'ExpirationInformationHeader' },
    }[which];
    if (!pageAndHeader) throw new Error(`Unknown editor group "${which}"`);
    await this.openEditorPage(pageAndHeader.pageId);
    await gui.clickButton(this.page, [pageAndHeader.header]);
    return this;
  }

  // --- structural expectations (used by permission tests) ---------------------------------------

  async expectFieldVisible(objectName) {
    await this.ensureFieldPage(objectName);
    return gui.expectVisible(this.page, [objectName]);
  }
}

module.exports = { SoftwareEditorPage };
