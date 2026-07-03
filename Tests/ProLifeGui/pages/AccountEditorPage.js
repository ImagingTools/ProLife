// AccountEditorPage - the editor for Accounts (New/Edit).
// Fields for customer info, account info, company address, groups table.

const { BasePage } = require('./BasePage');
const { TextInput } = require('../controls');
const gui = require('../lib/gui');

class AccountEditorPage extends BasePage {
  constructor(page) {
    super(page, 'Accounts');

    this.customerId = new TextInput(page, ['CustomerIdInput']);
    this.accountName = new TextInput(page, ['AccountNameInput']);
    this.accountDescription = new TextInput(page, ['AccountDescriptionInput']);
    this.email = new TextInput(page, ['EmailInput']);
    this.country = new TextInput(page, ['CountryInput']);
    this.city = new TextInput(page, ['CityInput']);
    this.postalCode = new TextInput(page, ['PostalCodeInput']);
    this.street = new TextInput(page, ['StreetInput']);
    // Groups table addressed by objectName for visibility
  }

  save() { return this.runCommand('Save'); }
  undo() { return this.runCommand('Undo'); }
  redo() { return this.runCommand('Redo'); }

  async setCustomerId(text) { await this.customerId.fill(text); return this; }
  async setAccountName(text) { await this.accountName.fill(text); return this; }
  async setAccountDescription(text) { await this.accountDescription.fill(text); return this; }
  async setEmail(text) { await this.email.fill(text); return this; }
  async setCountry(text) { await this.country.fill(text); return this; }
  async setCity(text) { await this.city.fill(text); return this; }
  async setPostalCode(text) { await this.postalCode.fill(text); return this; }
  async setStreet(text) { await this.street.fill(text); return this; }

  async toggleGroup(which) {
    const header = {
      customer: 'CustomerInformationHeader',
      account: 'AccountInformationHeader',
      address: 'CompanyAddressHeader',
    }[which];
    if (!header) throw new Error(`Unknown editor group "${which}"`);
    await gui.clickButton(this.page, [header]);
    return this;
  }

  expectFieldVisible(objectName) {
    return gui.expectVisible(this.page, [objectName]);
  }
}

module.exports = { AccountEditorPage };
