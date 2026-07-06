// SearchPage - the universal Search page (pageId "Search", PagePermissions=["*"], visible to everyone).
//
// Grounded in Partitura/ImtGraphQlVoce.arp/PagesController.acc (SearchPage, PageId=Search) and
// ImtCore/Qml/imtgui/View/SearchPage.qml. The search view is not objectName-instrumented internally
// yet, so this page object navigates via the menu button (SearchButton) - open() hard-fails if that
// button is missing - and coverage is the per-user landing screenshot.

const { BasePage } = require('./BasePage');

class SearchPage extends BasePage {
  constructor(page) {
    super(page, 'Search');
  }
}

module.exports = { SearchPage };
