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
    this.contextEntityType = new ComboBox(page, ['ContextEntityTypeCombo']);
  }

  /**
   * Screenshot masks for everything in this editor that differs run to run: each comment carries the
   * clock time it was posted at (CommentTimestamp_<i>, named in TicketEditor.qml for exactly this).
   * formatTimestamp renders "d MMM yyyy HH:mm" - the day-of-month has no leading zero, so its width
   * shifts by a digit depending on which day the test runs. Pin the mask to a fixed width comfortably
   * wider than the longest realistic value instead of the element's own (variable) rendered width, or
   * a run on a different day than the baseline leaves a mismatched sliver at the mask's edge.
   */
  masks() {
    return gui.masksForPrefix(this.page, 'CommentTimestamp_', 3, 200);
  }

  async setTitle(text) {
    await this.title.fill(text);
    return this;
  }
  // The description commits to the document on editingFinished AND on its own 1s idle timer
  // (TicketEditor.qml's descriptionAutoSaveTimer) - Tab covers the first, the wait covers the second.
  // Without it the document could still be unedited when Save was clicked, and Save does nothing then.
  async setDescription(text) {
    await this.description.fill(text);
    await this.page.keyboard.press('Tab');
    await this.page.waitForTimeout(1100);
    await gui.waitForStable(this.page);
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

  /** Removing an assignee asks first (TicketEditor.qml's confirmRemoveAssigneeDialogComp). */
  async removeAssignee(index = 0) {
    await gui.click(this.page, [`AssigneeChip_${index}`, 'RemoveButton'], { what: 'remove assignee chip' });
    await gui.clickButton(this.page, ['YesButton']);
    return this;
  }

  /** Lock Issue - reporter/admin only (FullAccess); hides the comment box for EVERYONE once set. */
  /**
   * Lock the ticket. Ticking the box asks for confirmation first (TicketEditor.qml's
   * confirmLockTicketDialogComp), and the reason field only appears once the box is actually checked -
   * so filling it straight after the click looked for a field the confirmation was still holding back.
   */
  async lockIssue(reason) {
    await gui.click(this.page, ['LockIssueCheckBox']);
    await gui.clickButton(this.page, ['YesButton']);
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
  async addContext(searchText) {
    await gui.click(this.page, ['AddContextButton'], { what: '"Add context"' });
    // The per-entity-type FilterableSelectPopup is created by a Loader (recreated on entity-type
    // change, but also mounted for the first time asynchronously when the dialog opens) - give it a
    // beat to settle before addressing anything inside it.
    await gui.waitForStable(this.page);
    const typeCount = await this.contextEntityType.optionCount();
    for (let index = 0; index < typeCount; index++) {
      await this.contextEntityType.selectIndex(index);
      await gui.waitForStable(this.page);
      await gui.fill(this.page, ['FilterableSelectPopup'], searchText);
      // FilterableSelectPopup debounces filtering before its GraphQL round-trip.
      await this.page.waitForTimeout(700);
      await gui.waitForStable(this.page);
      if ((await gui.countVisible(this.page, ['FilterableSelectItem_0'])) > 0) {
        await gui.click(this.page, ['FilterableSelectItem_0'], { what: 'first context search result' });
        await this.page.keyboard.press('Escape');
        return true;
      }
    }
    await gui.dismissDialog(this.page);
    return false;
  }

  /** Removing a context reference asks first, like removing an assignee does. */
  async removeContext(index = 0) {
    await gui.click(this.page, [`ContextChip_${index}`, 'RemoveButton'], { what: 'remove context chip' });
    await gui.clickButton(this.page, ['YesButton']);
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
