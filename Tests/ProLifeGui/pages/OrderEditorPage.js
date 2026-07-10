// OrderEditorPage - multi-tab document editor for Orders (New / Edit).
//
// Covers main fields + groups. Products section uses separate ProductEditorDialog (high-level coverage via screenshots).

const { BasePage } = require('imtcore-gui-testkit/pages/BasePage');
const { ComboBox, TextInput } = require('imtcore-gui-testkit/controls');
const gui = require('imtcore-gui-testkit/lib/gui');

class OrderEditorPage extends BasePage {
  constructor(page) {
    super(page, 'Orders');

    this.deliveryId = new TextInput(page, ['DeliveryIdInput']);
    this.purchaseOrderId = new TextInput(page, ['PurchaseOrderIdInput']);
    this.description = new TextInput(page, ['DescriptionInput']);
    this.customer = new ComboBox(page, ['CustomerCombo']);
    this.orderStatus = new ComboBox(page, ['OrderStatusCombo']);
  }

  save() { return this.runCommand('Save'); }
  undo() { return this.runCommand('Undo'); }
  redo() { return this.runCommand('Redo'); }

  async setDeliveryId(text) {
    await this.deliveryId.fill(text);
    return this;
  }
  async setPurchaseOrderId(text) {
    await this.purchaseOrderId.fill(text);
    return this;
  }
  async setDescription(text) {
    await this.description.fill(text);
    return this;
  }
  async setCustomer(text) {
    await this.customer.select(text);
    return this;
  }
  async setOrderStatus(text) {
    await this.orderStatus.select(text);
    return this;
  }

  async toggleGroup(which) {
    const header = {
      order: 'OrderInformationHeader',
    }[which];
    if (!header) throw new Error(`Unknown editor group "${which}"`);
    await gui.clickButton(this.page, [header]);
    return this;
  }

  expectFieldVisible(objectName) {
    return gui.expectVisible(this.page, [objectName]);
  }

  // --- Products (ProductEditorDialog + OrderProductDelegate rows) -------------------------------
  //
  // ProductEditorDialog reuses the generic "Dialog" objectName (no dialog-specific one) and its
  // fields: category toggle SoftwareButton/HardwareButton, catalogue picker "ComboBox" (only one
  // combo in this dialog), confirm OKButton, cancel CancelButton. Removing a row raises a Yes/No
  // MessageDialog (YesButton/NoButton) - callers handle that confirm step themselves so they can
  // screenshot the confirm state before deciding.

  /**
   * Row locator for the Nth product line (OrderProductDelegate). Rows are NOT index-addressable by
   * objectName - each carries the product's OWN name as its objectName (e.g.
   * "WidgetLicenseElementView"), which collides whenever two rows share a product name - so this
   * locates by DOM position, scoped to the products list (OrderEditor.qml's OrderProductsListView),
   * instead.
   */
  productRow(index) {
    return this.page.locator('[objectName="OrderProductsListView"] [objectName$="ElementView"][visible]').nth(index);
  }

  /** Number of product rows currently in the order. */
  productCount() {
    return this.page.locator('[objectName="OrderProductsListView"] [objectName$="ElementView"][visible]').count();
  }

  /**
   * Add a new product: opens ProductEditorDialog, picks the Software or Hardware category, selects
   * the Nth catalogue entry, and confirms. Position-based selection (not by caption): the combo is
   * populated from live seeded catalogue data with no stable text to assert on - same convention as
   * DeviceEditorPage's setDeviceTypeByIndex.
   * @param {'software'|'hardware'} category
   * @param {number} index
   */
  async addProduct(category, index) {
    await gui.clickButton(this.page, ['AddProductButton']);
    await gui.clickButton(this.page, [category === 'hardware' ? 'HardwareButton' : 'SoftwareButton']);
    await gui.selectIndex(this.page, ['ComboBox'], index);
    await gui.clickButton(this.page, ['OKButton']);
    return this;
  }

  /** Open the Nth product row's editor (ProductEditorDialog, pre-filled) via its row Edit command. */
  async editProductRow(index) {
    await gui.clickWithin(this.page, this.productRow(index), 'EditButton');
    return this;
  }

  /** Confirm the currently open ProductEditorDialog (OK). */
  async confirmProductDialog() {
    await gui.clickButton(this.page, ['OKButton']);
    return this;
  }

  /** Cancel out of the currently open ProductEditorDialog without applying changes. */
  async cancelProductDialog() {
    await gui.clickButton(this.page, ['CancelButton']);
    return this;
  }

  /**
   * Remove the Nth product row - opens the Yes/No confirm MessageDialog; does NOT confirm it (call
   * gui.clickButton(page, ['YesButton']) / ['NoButton'] separately so a test can screenshot the
   * confirm prompt before deciding).
   */
  async removeProductRow(index) {
    await gui.clickWithin(this.page, this.productRow(index), 'RemoveButton');
    return this;
  }

  /**
   * Toggle the products list between compact and detailed (expanded) card view. This is a single,
   * GLOBAL toolbar toggle (OrderEditor.qml's expandButton) affecting every row at once - there is no
   * per-row expand control.
   */
  async toggleProductsExpanded() {
    await gui.clickButton(this.page, ['ExpandProductsButton']);
    return this;
  }
}

module.exports = { OrderEditorPage };
