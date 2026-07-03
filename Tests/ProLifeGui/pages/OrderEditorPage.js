// OrderEditorPage - multi-tab document editor for Orders (New / Edit).
//
// Covers main fields + groups. Products section uses separate ProductEditorDialog (high-level coverage via screenshots).

const { BasePage } = require('./BasePage');
const { ComboBox, TextInput } = require('../controls');
const gui = require('../lib/gui');

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
}

module.exports = { OrderEditorPage };
