// WorkspacePage - the fully-worked example page object.
//
// Grounded in Qml/Include/prolifeqml/WorkspacePage.qml objectNames:
//   TimeFilterDelegate, CustomerFilterDelegate,
//   <collectionId>Info  (e.g. SoftwareProductsInfo, OrdersInfo) -> CreateNewButton / ViewAllButton,
//   command-bar tabs UserActions / Analytics (via CommandsView),
//   charts (SoftwareUsedPieChart, HardwareUsedBarChart, ...).
//
// Actions only; the test owns the screenshots. Every method returns `this` for chaining.

const { BasePage } = require('imtcore-gui-testkit/pages/BasePage');
const { ComboBox } = require('imtcore-gui-testkit/controls');
const gui = require('imtcore-gui-testkit/lib/gui');

class WorkspacePage extends BasePage {
  constructor(page) {
    super(page, 'Workspace');
    this.customerFilterCombo = new ComboBox(page, ['CustomerFilterDelegate']);
  }

  // --- command-bar tabs -------------------------------------------------------------------------

  async openUserActions() {
    await this.runCommand('UserActions');
    return this;
  }

  async openAnalytics() {
    await this.runCommand('Analytics');
    return this;
  }

  // --- top filters ------------------------------------------------------------------------------

  /** Open the date filter and choose a preset (e.g. 'Month_Current', 'Year_Last'). */
  async setTimeFilter(preset) {
    await gui.clickButton(this.page, ['TimeFilterDelegate']);
    await gui.clickButton(this.page, ['TimeFilterParamView', preset]);
    return this;
  }

  /** Pick a customer in the workspace customer filter. */
  async setCustomerFilter(customerName) {
    await this.customerFilterCombo.select(customerName);
    return this;
  }

  // --- collection info cards ("Create New" / "View All") ----------------------------------------

  /** e.g. collectionId 'SoftwareProducts' or 'Orders' -> the "<id>Info" card. */
  async createNewIn(collectionId) {
    await gui.clickButton(this.page, [`${collectionId}Info`, 'CreateNewButton']);
    return this;
  }

  async viewAllIn(collectionId) {
    await gui.clickButton(this.page, [`${collectionId}Info`, 'ViewAllButton']);
    return this;
  }
}

module.exports = { WorkspacePage };
