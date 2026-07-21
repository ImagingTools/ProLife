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
  /**
   * Position-based Customer selection - CustomerCombo is populated from live seeded Accounts data
   * with no stable caption to assert on, same convention as the product catalogue combos below.
   */
  async setCustomerByIndex(index) {
    await gui.selectIndex(this.page, ['CustomerCombo'], index);
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
  // fields: category toggle SoftwareButton/HardwareButton, catalogue picker "ProductCatalogueCombo"
  // (NOT the bare "ComboBox" fallback every combo box's inner control carries by default - see
  // ProductEditor.qml's own comment), confirm OKButton, cancel CancelButton. Removing a row raises a
  // Yes/No MessageDialog (YesButton/NoButton) - callers handle that confirm step themselves so they
  // can screenshot the confirm state before deciding.

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
   *
   * Selecting a Software product loads a nested sub-form (SoftwareProductEditor.qml) requiring a
   * SECOND selection - an existing license to link (ProductLicenseCombo) - before OK enables; this is
   * unchecked ("link an existing license", not "create a new one") by default for a freshly-added
   * product (confirmed live: OK stays disabled and a "Please select a license" error shows otherwise).
   * Selecting a Hardware product is the exact same shape via HardwareProductEditor.qml's own
   * "ProductDeviceCombo" (link an EXISTING device; switchNewSensor unchecked by default) - previously
   * UNHANDLED here (this method used to skip straight to OK for 'hardware', which would have silently
   * no-opped on a disabled OK button the same way the software gap once did, just never exercised by
   * any test). Not every catalogue product has a linkable license/device in the seeded test data
   * (confirmed live for software: the combo simply has nothing to open) - same data-adaptive reasoning
   * as DeviceCollectionPage's filterFinishedSensorsWithLicense. `index` can also simply exceed the
   * catalogue's real size (it's live seeded data with no stable count) - confirmed live: addAnyProduct's
   * retry loop can walk past the last real entry, throwing "combo item not found ... at index N" from
   * the CATALOGUE combo itself rather than the license/device one. All these cases return `false`
   * (dialog cancelled, nothing added) rather than throwing, so the caller can `test.skip()` instead of
   * failing on a data state.
   * @param {'software'|'hardware'} category
   * @param {number} index
   * @returns {Promise<boolean>} true if a product was actually added
   */
  async addProduct(category, index) {
    await gui.clickButton(this.page, ['AddProductButton']);
    await gui.clickButton(this.page, [category === 'hardware' ? 'HardwareButton' : 'SoftwareButton']);
    try {
      await gui.selectIndex(this.page, ['ProductCatalogueCombo'], index);
      await gui.selectIndex(this.page, [category === 'hardware' ? 'ProductDeviceCombo' : 'ProductLicenseCombo'], 0);
    } catch (_) {
      await gui.clickButton(this.page, ['CancelButton']);
      return false;
    }
    await gui.clickButton(this.page, ['OKButton']);
    return true;
  }

  /**
   * Like addProduct(), but tries catalogue entries 0, 1, 2, ... in turn until one actually has a
   * linkable license (see addProduct's own comment on why not every entry does), instead of
   * potentially skipping on the very first one tried. Later tests in the same describe.serial chain
   * (e.g. "open existing product row") depend on a row genuinely having been added, so silently giving
   * up after one attempt would cascade into failures there too.
   * @param {'software'|'hardware'} category
   * @param {number} [maxAttempts]
   * @returns {Promise<boolean>} true if a product was actually added
   */
  async addAnyProduct(category, maxAttempts = 5) {
    for (let index = 0; index < maxAttempts; index++) {
      // eslint-disable-next-line no-await-in-loop
      if (await this.addProduct(category, index)) return true;
    }
    return false;
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
