// Control wrappers: thin, stateless factories over lib/actions, each keyed by an objectName path.
// They give page objects a readable vocabulary (combo.select('QUISS'), commandBar.run('Save'),
// table.sortBy('Name')) instead of raw path arrays, and they inherit the hard-fail-on-missing
// behaviour from lib/actions - a control method never silently does nothing.

const gui = require('../lib/gui');

/** A generic clickable button addressed by an objectName path (or single name). */
class Button {
  constructor(page, pathOrName) {
    this.page = page;
    this.path = Array.isArray(pathOrName) ? pathOrName : [pathOrName];
  }
  click() {
    return gui.clickButton(this.page, this.path);
  }
  expectVisible() {
    return gui.expectVisible(this.page, this.path);
  }
  expectHidden() {
    return gui.expectHidden(this.page, this.path);
  }
}

/** The top command bar (CommandsView). Commands are addressed by their element id -> <id>Button. */
class CommandBar {
  constructor(page) {
    this.page = page;
  }
  run(commandId) {
    return gui.clickCommand(this.page, commandId);
  }
  expectHasCommand(commandId) {
    return gui.expectVisible(this.page, ['CommandsView', `${commandId}Button`], `command "${commandId}" should be visible`);
  }
  expectNoCommand(commandId) {
    return gui.expectHidden(this.page, ['CommandsView', `${commandId}Button`], `command "${commandId}" should be hidden`);
  }
}

/** The left navigation menu (MenuPanel). Pages are addressed by PageId -> <PageId>Button. */
class MenuPanel {
  constructor(page) {
    this.page = page;
  }
  open(pageId) {
    return gui.openPage(this.page, pageId);
  }
  expectHasPage(pageId) {
    return gui.expectVisible(this.page, ['MenuPanel', `${pageId}Button`], `page "${pageId}" should be visible`);
  }
  expectNoPage(pageId) {
    return gui.expectHidden(this.page, ['MenuPanel', `${pageId}Button`], `page "${pageId}" should be hidden`);
  }
}

/** A ComboBox / dropdown. */
class ComboBox {
  constructor(page, path) {
    this.page = page;
    this.path = path;
  }
  open() {
    return gui.click(this.page, this.path, { what: 'combo box' });
  }
  select(itemText) {
    return gui.select(this.page, this.path, itemText);
  }
  clear() {
    return gui.clickButton(this.page, [...this.path, 'ClearButton']);
  }
}

/** A text input (wraps an inner [objectName="TextInput"]). */
class TextInput {
  constructor(page, path) {
    this.page = page;
    this.path = path;
  }
  fill(text, opts) {
    return gui.fill(this.page, this.path, text, opts);
  }
  clear() {
    return gui.clickButton(this.page, [...this.path, 'ClearText']);
  }
}

/** The collection filter panel (FilterPanel). */
class FilterPanel {
  constructor(page) {
    this.page = page;
  }
  search(text) {
    return gui.fill(this.page, ['FilterPanel', 'SearchTextInput'], text);
  }
  clearSearch() {
    return gui.clickButton(this.page, ['SearchTextInput', 'ClearText']);
  }
  combo(filterObjectName) {
    return new ComboBox(this.page, ['FilterPanel', filterObjectName]);
  }
  clearFilter(filterObjectName) {
    return gui.clickButton(this.page, ['FilterPanel', filterObjectName, 'ClearButton']);
  }
  /** Open a date-range filter and pick a preset (e.g. 'Month_Current', 'Year_Last'). */
  async dateFilter(dateFilterObjectName, preset) {
    await gui.clickButton(this.page, ['FilterPanel', dateFilterObjectName]);
    await gui.clickButton(this.page, ['TimeFilterParamView', preset]);
  }
}

/**
 * A collection table. Rows are addressable by index thanks to the objectName "TableRow_<i>" added to
 * TableRowDelegateBase.qml; columns are addressable by their header id (TableHeaderDelegate sets
 * objectName: headerId). "Table" / "TableHeaders" wrap the whole table.
 */
class Table {
  constructor(page) {
    this.page = page;
  }
  /** Click a row by zero-based index (selects it). */
  selectRow(index) {
    return gui.click(this.page, [`TableRow_${index}`], { what: `table row ${index}` });
  }
  /**
   * Sort by a column, addressed by its header field id - this is the page's HeaderIds entry, NOT the
   * visible HeaderNames caption (the two lists are independently ordered per *Page.acc, e.g. Devices/
   * SoftwareProducts's "Name" caption maps to id "licenseName", not "name"). Confirm the real id in
   * the collection's *Page.acc before using a new one here.
   * @example sortBy('status') // Devices/Orders/SoftwareProducts "Status" column
   * @example sortBy('macAddress') // Devices "MAC Address" column
   */
  sortBy(headerId) {
    return gui.click(this.page, ['TableHeaders', headerId], { what: `column "${headerId}"` });
  }
  /** Assert the table has at least `n` rows (row 0..n-1 visible). */
  async expectAtLeastRows(n) {
    await gui.expectVisible(this.page, [`TableRow_${n - 1}`], `expected at least ${n} table rows`);
  }
  expectRow(index) {
    return gui.expectVisible(this.page, [`TableRow_${index}`]);
  }
}

/** The collection pagination bar (Pagination.qml). */
class Pagination {
  constructor(page) {
    this.page = page;
  }
  /** Choose page size from the per-page combo (values: 25/50/100/250/500). */
  setPageSize(size) {
    return gui.select(this.page, ['Pagination', 'PageSizeCombo'], String(size));
  }
  /** Go to a page by its 1-based number (page buttons are auto-named "<n>Button"). */
  goToPage(pageNumber) {
    return gui.click(this.page, ['Pagination', `${pageNumber}Button`], { what: `page ${pageNumber}` });
  }
  next() {
    return gui.click(this.page, ['Pagination', 'NextPageButton'], { what: 'next page' });
  }
  prev() {
    return gui.click(this.page, ['Pagination', 'PrevPageButton'], { what: 'prev page' });
  }
}

/** A labelled on/off switch (SwitchElementView -> inner SwitchButton). */
class Switch {
  constructor(page, path) {
    this.page = page;
    this.path = path;
  }
  toggle() {
    return gui.click(this.page, [...this.path, 'SwitchButton'], { what: 'switch' });
  }
}

/** A modal dialog with confirm/cancel style buttons. */
class Dialog {
  constructor(page) {
    this.page = page;
  }
  clickButtonByText(text) {
    // imtcontrols Button auto-names as "<text w/o spaces>Button".
    return gui.clickButton(this.page, [`${text.replace(/\s/g, '')}Button`]);
  }
  confirm() {
    return this.clickButtonByText('Yes');
  }
  cancel() {
    return this.clickButtonByText('No');
  }
  ok() {
    return this.clickButtonByText('OK');
  }
  close() {
    return gui.clickButton(this.page, ['CloseButton']);
  }
}

module.exports = { Button, CommandBar, MenuPanel, ComboBox, TextInput, FilterPanel, Table, Pagination, Switch, Dialog };
