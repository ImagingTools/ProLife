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

  /**
   * Switch to a MultiPageView sub-page by its addPage id (OrderEditor.qml: "General", "Products",
   * "History" - ids, not the visible captions, which carry a row count).
   */
  async openEditorPage(pageId) {
    await gui.clickButton(this.page, [`Page_${pageId}`]);
    return this;
  }

  // --- Products (ProductEditorDialog + OrderProductDelegate rows) -------------------------------
  //
  // ProductEditorDialog reuses the generic "Dialog" objectName (no dialog-specific one). Its parts:
  // category toggle SoftwareButton/HardwareButton, source toggle ProductSourceSegmented (Link existing
  // by default), the instance picker ProductLinkSelectButton -> FilterableSelectItem_<i>, and a primary
  // button named for what it does ("Add" / "Save") next to CancelButton. Removing a row raises a
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

  /**
   * Number of product rows currently in the order. Switches to the Products sub-page first, but only
   * when it isn't already showing - this is called from an expect.poll loop, which would otherwise
   * re-click the nav item on every iteration.
   */
  async productCount() {
    if (!(await gui.dom.isVisible(this.page, ['OrderProductsListView'], 500))) {
      await this.openEditorPage('Products');
    }
    return this.page.locator('[objectName="OrderProductsListView"] [objectName$="ElementView"][visible]').count();
  }

  /**
   * Add a product line by LINKING an existing instance.
   *
   * ProductEditorDialog was rebuilt: the old "pick a catalogue entry, then pick a license/device to
   * link" pair of combos (ProductCatalogueCombo + ProductLicenseCombo/ProductDeviceCombo) is gone. The
   * dialog now has a [Software|Hardware] segment, a [Link existing|Create new] segment (ProductEditor.qml
   * defaults to Link existing, `isCreateMode: false`), and ONE picker in the top bar -
   * ProductLinkSelectButton, which opens a FilterableSelectPopup of the linkable instances. This method
   * went on driving the combos that no longer exist, so every attempt threw, cancelled, and the retry
   * loop below simply repeated that until the test timed out.
   *
   * Selection is positional: the list is live seeded data with no stable caption - same convention as
   * DeviceEditorPage.setDeviceTypeByIndex. `index` may simply exceed what this catalogue currently
   * offers, which is a data state and not a fault: that returns false (dialog cancelled, nothing added)
   * so the caller can test.skip() rather than fail.
   * @param {'software'|'hardware'} category
   * @param {number} index
   * @returns {Promise<boolean>} true if a product line was actually added
   */
  /**
   * Screenshot masks for the product rows. What is IN them - the linked licence's Software-ID and
   * expiry - depends on which instance was still free when the test ran, i.e. on what earlier mutating
   * tests consumed; these shots document the dialog and the flow, not that particular licence.
   */
  productMasks() {
    return gui.masksForPrefix(this.page, 'OrderProductsListView');
  }

  async addProduct(category, index) {
    await this.openEditorPage('Products');
    await gui.clickButton(this.page, ['AddProductButton']);
    await gui.clickButton(this.page, [category === 'hardware' ? 'HardwareButton' : 'SoftwareButton']);
    try {
      // The dialog opens in Create-new mode, so switch to Link existing first - that is what reveals
      // the picker (ProductEditor.qml: showLinkPicker requires !isCreateMode).
      await gui.clickButton(this.page, ['LinkexistingButton']);
      await gui.clickButton(this.page, ['ProductLinkSelectButton']);
      // clickSelf: this popup's rows carry a MouseArea that the bridge never marks visible, so the
      // usual nested-MouseArea click waits for something that never comes.
      await gui.clickSelf(this.page, [`FilterableSelectItem_${index}`], { what: `linkable ${category} instance ${index}` });
    } catch (_) {
      // The popup may or may not have opened; close whichever is on top, then leave the dialog.
      await this.page.keyboard.press('Escape').catch(() => {});
      await gui.clickButton(this.page, ['CancelButton']).catch(() => {});
      return false;
    }
    await this.confirmProductDialog();
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
    await this.openEditorPage('Products');
    await gui.clickWithin(this.page, this.productRow(index), 'EditButton');
    return this;
  }

  /** Confirm the currently open ProductEditorDialog (OK). */
  async confirmProductDialog() {
    // The primary button is "Add" for a new line and "Save" when editing one (ProductEditorDialog.qml
    // renames it on activeProductIndexChanged), and imtcontrols derives the objectName from that text -
    // so it is not one fixed name, and the old "OKButton" matches neither.
    const isAdd = (await gui.countVisible(this.page, ['AddButton'])) > 0;
    await gui.clickButton(this.page, [isAdd ? 'AddButton' : 'SaveButton']);
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
