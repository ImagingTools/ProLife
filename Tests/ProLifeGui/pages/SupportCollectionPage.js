// SupportCollectionPage - the "Support" menu entry (actual PageId/MenuPanel button: "Tickets"), a
// generic ImtCore feature
// (imtdeskgui/TicketCollectionView.qml) hosted inside DeskPage.qml, which also has its own
// Tickets/Conversations toggle (Support lands on Tickets by default - index 0, toggled true on
// Component.onCompleted). Universal page (PAGE_PERMISSIONS: ['*']), same as Search - every
// authenticated user can reach it.
//
// Standard RemoteCollectionView conventions apply (Table/FilterPanel/CommandsView, TableRow_<index>),
// same base every other collection page in this suite already uses - no extra instrumentation needed
// for the list itself.

const { CollectionPage } = require('imtcore-gui-testkit/pages/CollectionPage');

class SupportCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'Tickets');
  }

  async clearAllFilters() {
    await this.filters.clearAllFilters();
    return this;
  }
}

module.exports = { SupportCollectionPage };
