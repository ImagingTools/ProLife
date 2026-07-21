// SupportTicketEditorPage - the Ticket document editor (imtdeskgui/TicketEditor.qml), opened as a
// document tab from SupportCollectionPage the same way Devices/Orders/etc.'s own editors are
// (DocCollectionViewDelegate + registerDocumentType/addDocumentView).
//
// Status only shows for an EXISTING ticket (TicketEditor.qml: `visible: !root.isNewIssue`) - a brand
// new ticket has no Status field yet. The command bar (Save/Undo/Redo) is ONLY visible while the
// ticket is new (`commandsPanelVisible: isNewIssue`) - once saved, further edits (comment, status,
// title, description) apply immediately via each field's own onEditingFinished/onCurrentIndexChanged
// (doUpdateModel()), with no separate Save step needed.

const { BasePage } = require('imtcore-gui-testkit/pages/BasePage');
const { ComboBox, TextInput } = require('imtcore-gui-testkit/controls');
const gui = require('imtcore-gui-testkit/lib/gui');

class SupportTicketEditorPage extends BasePage {
  constructor(page) {
    super(page, 'Tickets');

    this.title = new TextInput(page, ['TitleInput']);
    this.description = new TextInput(page, ['DescriptionInput']);
    this.type = new ComboBox(page, ['TicketTypeCombo']);
    this.priority = new ComboBox(page, ['TicketPriorityCombo']);
    this.status = new ComboBox(page, ['TicketStatusCombo']);
  }

  save() { return this.runCommand('Save'); }

  async setTitle(text) {
    await this.title.fill(text);
    return this;
  }
  async setDescription(text) {
    await this.description.fill(text);
    return this;
  }
  async setType(text) {
    await this.type.select(text);
    return this;
  }
  async setPriority(text) {
    await this.priority.select(text);
    return this;
  }
  /** Only available once the ticket is no longer new (see class header). */
  async setStatus(text) {
    await this.status.select(text);
    return this;
  }

  /** Type a comment and click Send (CommentInput/SendCommentButton - both bare, hand-instrumented). */
  async addComment(text) {
    await gui.fill(this.page, ['CommentInput'], text);
    await gui.clickButton(this.page, ['SendCommentButton']);
    return this;
  }

  /**
   * Assign a user (multi-select FilterableSelectPopup over the Users collection, same picker
   * component AddGroups/AddRoles already use in administration.editor.multiuser.test.js). Editor-UI
   * coverage only (open the picker, pick whichever row search resolves to, see the chip, remove it) -
   * NOT a cross-user visibility check. Which specific user gets picked is not asserted; the ProLifeGui
   * fixture users are excluded from this particular picker's results (confirmed live: a real,
   * loggable-in fixture user like "fullAccess" returns "No items found" even for an exact-name search
   * - root cause not fully pinned down, likely a Puma-side filter this repo doesn't have source for),
   * so leave searchText empty to just pick row 0 of whatever real users the picker does return.
   */
  async addAssignee(searchText) {
    await gui.click(this.page, ['AddAssigneeButton'], { what: '"Add assignee"' });
    await gui.fill(this.page, ['FilterableSelectPopup'], searchText);
    // FilterableSelectPopup debounces the filter text (500ms - FilterableSelectPopup.qml's
    // debounceInterval) before re-querying, so the row list right after fill() still reflects the
    // PRE-filter results; clicking row 0 immediately can select an unrelated item. Outwait the
    // debounce, then let the resulting GraphQL round-trip settle before addressing row 0.
    await this.page.waitForTimeout(700);
    await gui.waitForStable(this.page);
    await gui.click(this.page, ['FilterableSelectItem_0'], { what: 'first assignee search result' });
    await this.page.keyboard.press('Escape');
    return this;
  }

  async removeAssignee(index = 0) {
    await gui.click(this.page, [`AssigneeChip_${index}`, 'RemoveButton'], { what: 'remove assignee chip' });
    return this;
  }

  /** Lock Issue - reporter/admin only (FullAccess); hides the comment box for EVERYONE once set. */
  async lockIssue(reason) {
    await gui.click(this.page, ['LockIssueCheckBox']);
    if (reason) await gui.fill(this.page, ['LockReasonInput'], reason);
    return this;
  }

  async unlockIssue() {
    await gui.click(this.page, ['LockIssueCheckBox']);
    return this;
  }

  /**
   * Context / entity reference - ComboBox (entity type) + the same FilterableSelectPopup pattern.
   * The entity type list is server-supplied per ticket (ticketData.m_entityTypes - see
   * TicketEditor.qml), so it's selected BY POSITION (selectIndex) rather than a hardcoded caption
   * that would silently drift out of sync with the catalogue - same reasoning selectIndex's own doc
   * comment gives for device-type/hardware-configuration combos elsewhere in this suite.
   */
  async addContext(entityTypeIndex, searchText) {
    await gui.click(this.page, ['AddContextButton'], { what: '"Add context"' });
    // The per-entity-type FilterableSelectPopup is created by a Loader (recreated on entity-type
    // change, but also mounted for the first time asynchronously when the dialog opens) - give it a
    // beat to settle before addressing anything inside it.
    await gui.waitForStable(this.page);
    await gui.selectIndex(this.page, ['ContextEntityTypeCombo'], entityTypeIndex);
    await gui.waitForStable(this.page);
    await gui.fill(this.page, ['FilterableSelectPopup'], searchText);
    // FilterableSelectPopup debounces the filter text (500ms - FilterableSelectPopup.qml's
    // debounceInterval) before re-querying, so the row list right after fill() still reflects the
    // PRE-filter results; clicking row 0 immediately can select an unrelated item. Outwait the
    // debounce, then let the resulting GraphQL round-trip settle before addressing row 0.
    await this.page.waitForTimeout(700);
    await gui.waitForStable(this.page);
    await gui.click(this.page, ['FilterableSelectItem_0'], { what: 'first context search result' });
    await this.page.keyboard.press('Escape');
    return this;
  }

  async removeContext(index = 0) {
    await gui.click(this.page, [`ContextChip_${index}`, 'RemoveButton'], { what: 'remove context chip' });
    return this;
  }

  /**
   * Attach a file to the comment being composed (AttachButton opens a native QLP.FileDialog - on the
   * web build this is backed by a real browser file input, so Playwright's own filechooser event
   * intercepts it regardless of the QML-side implementation detail).
   */
  async attachFile(filePath) {
    const chooserPromise = this.page.waitForEvent('filechooser');
    await gui.clickButton(this.page, ['AttachButton']);
    const chooser = await chooserPromise;
    await chooser.setFiles(filePath);
    return this;
  }
}

module.exports = { SupportTicketEditorPage };
