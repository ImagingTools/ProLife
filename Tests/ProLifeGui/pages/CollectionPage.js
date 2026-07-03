// Base for the table+filter+CRUD collection pages (Customers, Devices/Hardware, Orders, Licenses).
// Concrete pages subclass this and add page-specific filters/editors.

const { BasePage } = require('./BasePage');
const { FilterPanel, Table, Pagination } = require('../controls');

class CollectionPage extends BasePage {
  /**
   * @param {import('@playwright/test').Page} page
   * @param {string} pageId   MenuPanel PageId (e.g. 'Accounts', 'Devices')
   */
  constructor(page, pageId) {
    super(page, pageId);
    this.filters = new FilterPanel(page);
    this.table = new Table(page);
    this.pagination = new Pagination(page);
  }

  /** Text-search the collection. */
  async search(text) {
    await this.filters.search(text);
    return this;
  }

  /** Select the Nth row (0-based). */
  async selectRow(index = 0) {
    await this.table.selectRow(index);
    return this;
  }

  // New / Edit / Remove / Revision via the command bar (present on every collection page).
  newItem() {
    return this.runCommand('New');
  }
  editItem() {
    return this.runCommand('Edit');
  }
  removeItem() {
    return this.runCommand('Remove');
  }
  revision() {
    return this.runCommand('Revision');
  }
}

module.exports = { CollectionPage };
