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

  /**
   * Explicitly switch to the Dashboard tab (the default landing tab). NOT a no-op when already on
   * Dashboard - safe to call unconditionally as a reset. Confirmed live: re-navigating to Workspace via
   * the menu while ALREADY on Workspace (e.g. tests/workspace.multiuser.test.js's shared-session
   * 'interactions' block, where most tests never actually leave Workspace) does NOT reset the active
   * tab back to Dashboard - WorkspacePage.qml's NavigableItem.onParentActivated (which calls
   * checkCurrentPage("Dashboard")) only fires when the PARENT segment is freshly activated, i.e.
   * navigating in from a DIFFERENT page, not on a redundant click while already there. A test that
   * assumed the tab reset itself silently ran against whatever tab a PRIOR test left active instead.
   */
  async openDashboard() {
    await this.runCommand('Dashboard');
    return this;
  }

  async openUserActions() {
    await this.runCommand('UserActions');
    return this;
  }

  async openAnalytics() {
    await this.runCommand('Analytics');
    return this;
  }

  // --- Analytics tab's OWN toggle (Software/Hardware vs Orders) -------------------------------
  // StatisticsPage.qml is a separate ViewBase with its OWN commandsController (typeId "Analytics",
  // Software_Hardware/Orders) distinct from WorkspacePage's own top-level one (typeId "Workspace",
  // Dashboard/UserActions/Analytics) - both render a generic "CommandsView" at the same time while
  // Analytics is active, but runCommand()'s `['CommandsView', '<id>Button']` addressing isn't ambiguous
  // between them since the two command sets don't share any button names.

  /** Default sub-view of the Analytics tab (Software/Hardware charts). */
  async openAnalyticsSoftwareHardware() {
    await this.runCommand('Software_Hardware');
    return this;
  }

  /** The Orders sub-view of the Analytics tab (OrderCreationLineChart etc.) - previously untested. */
  async openAnalyticsOrders() {
    await this.runCommand('Orders');
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
