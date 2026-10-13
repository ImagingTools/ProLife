// AccountEditorPage - the editor for Accounts (New/Edit).
//
// The editor is a MultiPageView: each field lives on one of its sub-pages, and a field on a sub-page
// that is not open is present-but-invisible (or absent). Ids come from AccountEditor.qml's addPage
// calls and are NOT the visible captions - "AccountInformation" is labelled "Account".

const { BasePage } = require('imtcore-gui-testkit/pages/BasePage');
const { TextInput, Table } = require('imtcore-gui-testkit/controls');
const gui = require('imtcore-gui-testkit/lib/gui');

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
    // GroupsTable (AccountEditor.qml's "groupsElement") - a checkable Table listing every group this
    // account can belong to. Checking/unchecking a row applies to the in-memory model immediately
    // (AccountEditor.qml's onCheckedItemsChanged -> doUpdateModel()), Save still persists it.
    this.groups = new Table(page, ['GroupsTable']);
  }

  undo() { return this.runCommand('Undo'); }
  redo() { return this.runCommand('Redo'); }

  /** Switch to a MultiPageView sub-page by its addPage id (see this file's header). */
  async openEditorPage(pageId) {
    await gui.clickButton(this.page, [`Page_${pageId}`]);
    return this;
  }

  async setCustomerId(text) { await this.openEditorPage('CustomerInformation'); await this.customerId.fill(text); return this; }
  async setAccountName(text) { await this.openEditorPage('AccountInformation'); await this.accountName.fill(text); return this; }
  async setAccountDescription(text) { await this.openEditorPage('AccountInformation'); await this.accountDescription.fill(text); return this; }
  async setEmail(text) { await this.openEditorPage('AccountInformation'); await this.email.fill(text); return this; }
  async setCountry(text) { await this.openEditorPage('CompanyAddress'); await this.country.fill(text); return this; }
  async setCity(text) { await this.openEditorPage('CompanyAddress'); await this.city.fill(text); return this; }
  async setPostalCode(text) { await this.openEditorPage('CompanyAddress'); await this.postalCode.fill(text); return this; }
  async setStreet(text) { await this.openEditorPage('CompanyAddress'); await this.street.fill(text); return this; }

  /** The groups table lives on its own sub-page. */
  async openGroups() {
    await this.openEditorPage('Groups');
    return this;
  }

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
}

module.exports = { AccountEditorPage };
