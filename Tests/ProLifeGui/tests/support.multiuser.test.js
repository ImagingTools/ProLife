// Support (Tickets) page - GENERIC ImtCore feature (imtdeskgui/TicketCollectionView.qml +
// TicketEditor.qml), previously completely untested. Universal page (PAGE_PERMISSIONS: ['*']), same
// as Search - every authenticated user can reach it, no permission gate needed for basic access.
//
// The MenuPanel entry is labelled "Support" in the UI, but its actual registered PageId (and MenuPanel
// button objectName) is "Tickets" - confirmed by dumping the live MenuPanel DOM, not "Support" as its
// on-screen label would suggest. DeskPage.qml (the page behind this menu entry) hosts a Tickets/
// Conversations toggle of its own; it lands on Tickets by default (index 0, toggled true on
// Component.onCompleted), so no extra navigation is needed to reach the ticket list.

const fs = require('fs');
const os = require('os');
const path = require('path');
const { test, newUserPage } = require('../fixtures/test');
const { SupportCollectionPage, SupportTicketEditorPage } = require('../pages');
const { canSeePage } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

const PAGE = 'Tickets';

test.describe('Support', () => {
  test.beforeEach(async ({ page }) => {
    await new SupportCollectionPage(page).reload();
  });

  test('landing', async ({ page, gui: guiFixture, user }) => {
    if (canSeePage(user, PAGE)) await new SupportCollectionPage(page).open();
    await guiFixture.checkScreenshot(page, 'support-landing');
  });

  // New ticket: Title/Description/Type/Priority (Status only appears once the ticket already exists -
  // TicketEditor.qml's own `visible: !root.isNewIssue`), then Save (commandsPanelVisible: isNewIssue -
  // the command bar itself disappears once saved, since further edits apply immediately with no
  // separate Save step - see SupportTicketEditorPage's own header comment).
  test.describe.serial('ticket lifecycle', () => {
    let page, user, support, editor;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      support = new SupportCollectionPage(page);
      await support.reload();
      if (canSeePage(user, PAGE)) {
        await support.open();
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(() => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Support');
    });

    test('create a new ticket and save', { tag: '@mutating' }, async () => {
      await support.newItem();
      editor = new SupportTicketEditorPage(page);

      const title = `GUI test ticket ${Date.now()}`;
      await editor.setTitle(title);
      await editor.setDescription('Created by the GUI test suite.');
      await editor.setType('BugReport');
      await editor.setPriority('High');
      await gui.checkScreenshot(page, 'support-new-ticket-filled');

      await editor.save();
      await gui.waitForStable(page);
      // The command bar hides once the ticket is no longer new (commandsPanelVisible: isNewIssue), and
      // the title switches from its edit field to a read-only "#<number> <title>" display - the
      // screenshot is the check here rather than a structural assertion on that transition.
      await gui.checkScreenshot(page, 'support-new-ticket-saved');
    });

    // Adding a comment applies immediately (no Save step for an existing ticket) - the comment's own
    // text is the check here (no objectName exists on individual chat-message rows). The QML DOM bridge
    // renders each Text element as an outer layout wrapper (present but not actually visible) plus an
    // inner ".impl" node that holds the real rendered content - the SAME pattern seen on the PAT token
    // field - so the check targets ".impl" directly rather than a bare getByText().
    test('add a comment to the ticket', { tag: '@mutating' }, async () => {
      test.skip(!editor, 'no ticket was created above to comment on');
      const commentText = `Comment from GUI test ${Date.now()}`;
      await editor.addComment(commentText);
      await page.locator('.impl').filter({ hasText: commentText }).first().waitFor({ state: 'visible', timeout: 10000 });
      await gui.checkScreenshot(page, 'support-ticket-comment-added');
    });

    // Status only appears once the ticket is no longer new - closing it here proves the combo is live
    // on an EXISTING ticket, applying immediately (no Save step).
    test('change ticket status', { tag: '@mutating' }, async () => {
      test.skip(!editor, 'no ticket was created above to change status on');
      await gui.expectVisible(page, ['TicketStatusCombo'], 'Status should be visible on an existing ticket');
      await editor.setStatus('Closed');
      await gui.checkScreenshot(page, 'support-ticket-status-closed');
    });

    // Assignee - editor-UI coverage only (open the picker, pick a row, see the chip, remove it via the
    // chip's own RemoveButton). Not a cross-user visibility check - see SupportTicketEditorPage.
    // addAssignee's own comment for why (ProLifeGui fixture users don't show up in this picker's
    // results at all, root cause not fully pinned down without Puma's own source).
    test('add assignee, then remove it', { tag: '@mutating' }, async () => {
      test.skip(!editor, 'no ticket was created above to assign');
      await editor.addAssignee('');
      await gui.expectVisible(page, ['AssigneeChip_0'], 'the picked user should appear as an assignee chip');
      await gui.checkScreenshot(page, 'support-ticket-assignee-added');
      await editor.removeAssignee(0);
      await gui.expectHidden(page, ['AssigneeChip_0'], 'the chip should be gone after Remove');
    });

    // Context / entity references - a real search + select against the ComboBox (entity type) and
    // FilterableSelectPopup (per-entity picker), then remove it via the chip's own RemoveButton -
    // same picker/chip pattern already covered for Roles/Groups in administration.editor.multiuser.test.js.
    test('add context, then remove it', { tag: '@mutating' }, async () => {
      test.skip(!editor, 'no ticket was created above to add context to');
      await editor.addContext(0, '');
      await gui.expectVisible(page, ['ContextChip_0'], 'the picked entity should appear as a context chip');
      await gui.checkScreenshot(page, 'support-ticket-context-added');
      await editor.removeContext(0);
      await gui.expectHidden(page, ['ContextChip_0'], 'the chip should be gone after Remove');
    });

    // Attachments - a real file goes through AttachButton's native file dialog (Playwright's
    // filechooser event, not raw DOM access - see SupportTicketEditorPage.attachFile), uploads via the
    // app's own XHR endpoint, previews in the composer, then is sent along with the comment text.
    test('attach an image to a comment', { tag: '@mutating' }, async () => {
      test.skip(!editor, 'no ticket was created above to attach a file to');
      // A minimal valid 1x1 PNG - generated on the fly rather than committing a binary fixture.
      const pngBytes = Buffer.from(
        'iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNk+A8AAQUBAScY42YAAAAASUVORK5CYII=',
        'base64'
      );
      const filePath = path.join(os.tmpdir(), `support-attachment-${Date.now()}.png`);
      fs.writeFileSync(filePath, pngBytes);

      await editor.attachFile(filePath);
      await gui.expectVisible(page, ['CommentInput'], 'composer should still be visible while the attachment uploads');
      await page.getByText('support-attachment-', { exact: false }).first().waitFor({ state: 'visible', timeout: 15000 });
      await gui.checkScreenshot(page, 'support-ticket-attachment-pending');

      const commentText = `Comment with attachment ${Date.now()}`;
      await editor.addComment(commentText);
      await page.locator('.impl').filter({ hasText: commentText }).first().waitFor({ state: 'visible', timeout: 10000 });
      await gui.checkScreenshot(page, 'support-ticket-attachment-sent');

      fs.unlinkSync(filePath);
    });

    // Lock Issue - hides the comment box even for the reporter/admin who locked it (canComment stays
    // true, but the section's own visibility binding also checks !m_locked - see TicketEditor.qml).
    // Locking is placed LAST in this serial block since nothing after it can comment on this ticket again.
    test('locking the issue hides the comment box', { tag: '@mutating' }, async () => {
      test.skip(!editor, 'no ticket was created above to lock');
      await gui.expectVisible(page, ['CommentInput'], 'comment box should be visible before locking');
      await editor.lockIssue('Locked by the GUI test suite');
      await gui.expectHidden(page, ['CommentInput'], 'comment box should hide once the issue is locked');
      await gui.checkScreenshot(page, 'support-ticket-locked');
    });
  });
});
