// AdministrationPage - the Admin page (pageId "Administration").
//
// Grounded in ImtCore/Qml/imtauthgui/AdministrationView.qml, whose root carries
// objectName: "AdministrationView". The view hosts Users / Roles / Groups sub-pages, but those inner
// controls are not individually objectName-instrumented yet, so this page object only navigates to the
// page and asserts the view root is present; deeper coverage is screenshot-based (per user) until the
// sub-views are instrumented the way DeviceEditor was.

const { BasePage } = require('./BasePage');
const gui = require('../lib/gui');

class AdministrationPage extends BasePage {
  constructor(page) {
    super(page, 'Administration');
  }

  /** Assert the Administration view actually rendered after open() (hard-fails otherwise). */
  async expectLoaded() {
    await gui.expectVisible(this.page, ['AdministrationView'], 'AdministrationView should be visible');
    return this;
  }
}

module.exports = { AdministrationPage };
