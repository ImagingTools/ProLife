// User Profile panel + Logout - GENERIC coverage (ImtCore/Qml/imtauthgui/UserPanel.qml, ProfileView.qml).
//
// The top-right account button (UserPanelButton) is present on every page, opens a PopupMenuDialog
// (Profile / per-tenant Organization switcher / Logout), and Profile itself is a modal Dialog hosting a
// MultiPageView (General/Organizations/AccessTokens/Access - Roles & Permissions) - the SAME MultiPageView
// component Administration uses, so its subpages are already addressable via the established
// "Page_<id>" objectName convention (see AdministrationPage.js) with no extra instrumentation needed.
// None of this opens a document tab (it's a plain modal dialog), so - unlike the editor specs - it
// doesn't need an isolated user; the default per-test `page` fixture (its own fresh context) is enough,
// which also means a Logout in one test can't affect any other test's page.

const { test, expect } = require('../fixtures/test');
const { WorkspacePage } = require('../pages');

test.describe('User profile', () => {
  test.beforeEach(async ({ page }) => {
    const workspace = new WorkspacePage(page);
    await workspace.reload();
    if (await workspace.isAvailable()) await workspace.open();
  });

  // UserPanel.qml's account menu is a plain PopupMenuDialog bound to a ListModel, NOT a ComboBox - its
  // default delegate (unlike ComboBox.qml's own inline delegate) gives items no text-based objectName
  // (confirmed live: "Profile"/"Logout" not found by text), so items are addressed by position instead
  // (same convention as CreateLicenseFile's Encrypt/Unencrypt popup). "Profile" is always the menu's
  // first row; "Logout" is always its last, regardless of how many organization rows sit between them.

  test('opens and switches between profile tabs', async ({ page, gui }) => {
    await gui.openComboPopup(page, ['UserPanelButton']);
    await gui.clickPopupItemByIndex(page, 0); // Profile - always first
    await gui.expectVisible(page, ['Dialog'], 'Profile must open in a modal dialog');
    await gui.checkScreenshot(page, 'user-profile-general');

    for (const pageId of ['Organizations', 'AccessTokens', 'Access', 'General']) {
      await gui.click(page, [`Page_${pageId}`], { what: `Profile subpage "${pageId}"` });
    }
    await gui.checkScreenshot(page, 'user-profile-after-tab-switches');

    await gui.dismissDialog(page);
  });

  // The "General" page's password-change card is expanded for real (Change button), then collapsed
  // via Cancel WITHOUT ever submitting - same "open it for real, never complete the destructive
  // action" reasoning already used elsewhere in this suite for Register/Forgot-password (see
  // login.guest.test.js) - a real password change here would break every LATER test's own login.
  test('profile General "Change" expands the password card without submitting', async ({ page, gui }) => {
    await gui.openComboPopup(page, ['UserPanelButton']);
    await gui.clickPopupItemByIndex(page, 0); // Profile
    await gui.expectVisible(page, ['Dialog'], 'Profile must open in a modal dialog');

    await gui.clickButton(page, ['ChangeButton']);
    await gui.expectVisible(page, ['NewPasswordInput'], 'password card should expand');
    await gui.checkScreenshot(page, 'user-profile-general-password-card');

    await gui.clickButton(page, ['CancelButton']);
    await gui.expectHidden(page, ['NewPasswordInput'], 'password card should collapse after Cancel');
    await gui.dismissDialog(page);
  });

  // Full Personal Access Token (PAT) lifecycle: create it (with a real permissions-tree selection and
  // a non-default expiration), read the generated secret, copy it, confirm it lands in the table, then
  // revoke and delete it (cleanup, so repeat runs don't accumulate orphaned tokens).
  test('access tokens: generate, copy, appears in the list, revoke, then delete', async ({ page, gui }) => {
    // Keyed to the user, not the clock: the name is typed into a field several screenshots capture, so
    // a per-run number means they never match. Per-user because this spec runs for EVERY user against
    // one database, in parallel - the names still must not collide.
    const tokenName = `gui-test-token-${test.info().project.name}`;

    await gui.openComboPopup(page, ['UserPanelButton']);
    await gui.clickPopupItemByIndex(page, 0); // Profile
    await gui.expectVisible(page, ['Dialog'], 'Profile must open in a modal dialog');
    await gui.click(page, ['Page_AccessTokens'], { what: 'Profile subpage "AccessTokens"' });

    await gui.clickButton(page, ['NewTokenButton']);
    await gui.expectVisible(page, ['TokenNameInput'], '"New Personal Access Token" dialog should open');

    // A token has to be granted at least one permission, and the tree offers only the permissions its
    // owner actually holds - so a user granted none gets "No permissions available to assign to this
    // token" and ProfileTokensPage.qml hides the whole PermissionsTableView (scopesEmpty), toolbar
    // included. There is no token for them to create; that is the product working, not a failure.
    //
    // The scope list loads asynchronously, so this waits for the toolbar rather than counting rows in
    // the same tick - and it asks about the TOOLBAR, not about "TreeRow_0" anywhere on the page, which
    // also matched a row of the profile's own permissions tree behind the dialog and so never skipped.
    test.skip(
      !(await gui.dom.isVisible(page, ['ExpandAllButton'], 10000)),
      'this user holds no permissions, so a scoped token cannot be created'
    );

    await gui.fill(page, ['TokenNameInput'], tokenName);
    await gui.fill(page, ['TokenDescriptionInput'], 'Created by the GUI test suite');
    // Blur the description field (its value only commits onEditingFinished) by interacting with the
    // expiration combo next - also proves that combo is a real, working control, not just a default.
    await gui.select(page, ['ComboBox'], '30Days');

    // Click through the permissions tree for real (not just "Check All") - expand it and check the top
    // row, which is a GROUP and therefore selects its children with it.
    //
    // It used to check rows 0 AND 1, assuming both are always present and independent. They are for su,
    // whose tree is large. A user holding ONE permission has a tree of exactly two rows - the group and
    // its single child - so the second click unchecked the child and with it the group, leaving nothing
    // selected. "Select at least one permission" stayed up, Generate Token stayed disabled, and the
    // failure surfaced much later as "the dialog should open". One group row is what this needs, and it
    // holds for any user.
    await gui.clickButton(page, ['ExpandAllButton']);
    await gui.checkScreenshot(page, 'access-tokens-new-dialog-tree-expanded');
    await gui.click(page, ['TreeRow_0', 'RowCheckBox'], { what: 'first permission group checkbox' });
    await gui.checkScreenshot(page, 'access-tokens-new-dialog-filled');

    // Generate Token is disabled until name + expiration + at least one scope are all set - clicking
    // it while still disabled would silently no-op, so a "Token Created" dialog actually appearing is
    // itself proof the button was truly enabled at this point.
    await gui.clickButton(page, ['GenerateTokenButton']);
    await gui.expectVisible(page, ['TokenValueField'], '"Token Created Successfully" dialog should open');

    // CustomTextField (readOnly) renders its value as the text CONTENT of a nested contenteditable div
    // (no "text" attribute, no real <input> - confirmed by dumping the live DOM), so textContent is the
    // only way to read it back.
    const tokenValueInput = gui.dom.byPath(page, ['TokenValueField', 'TextInput']);
    const tokenValue = await tokenValueInput.evaluate((el) => el.textContent.trim());
    expect(tokenValue, 'the generated token secret should be non-empty').toBeTruthy();
    await gui.checkScreenshot(page, 'access-tokens-token-created', { path: ['TokenValueField'] });

    // Copy: no structural "was it copied" signal exposed (the icon just swaps to a checkmark and the
    // button disables itself), so the click succeeding without error plus a screenshot of the changed
    // icon state is the check here - same reasoning as this suite's own RememberMeCheckBox convention.
    // The generated secret is random per run - masked out of both shots of it, or the comparison is
    // against a value that can never match.
    await gui.clickButton(page, ['CopyTokenButton']);
    await gui.checkScreenshot(page, 'access-tokens-token-copied', { path: ['TokenValueField'] });

    await gui.clickButton(page, ['OKButton']);
    await gui.expectHidden(page, ['TokenValueField'], '"Token Created" dialog should close after OK');

    // The new token now shows up in the table - locate its row by the NAME we just gave it, not by
    // position (a fresh token's row position depends on the list's own sort order).
    const tokenRow = page.locator('[objectName^="TableRow_"][visible]').filter({ hasText: tokenName }).first();
    await tokenRow.waitFor({ state: 'visible', timeout: 10000 });
    await gui.checkScreenshot(page, 'access-tokens-list-with-new-token');

    // Revoke, then delete (cleanup) - both scoped to THIS token's own row so a same-named leftover
    // from a previous failed run can't be affected instead.
    await gui.clickWithin(page, tokenRow, 'RevokeTokenButton');
    await gui.waitForStable(page);
    await gui.checkScreenshot(page, 'access-tokens-token-revoked');

    await gui.clickWithin(page, tokenRow, 'DeleteTokenButton');
    await gui.expectVisible(page, ['YesButton'], 'delete should ask for confirmation');
    await gui.clickButton(page, ['YesButton']);

    await gui.dismissDialog(page);
  });

  test('logout returns to the login screen', async ({ page, gui }) => {
    await gui.openComboPopup(page, ['UserPanelButton']);
    await gui.clickPopupItemLast(page); // Logout - always last
    await gui.expectVisible(page, ['LoginInput'], 'Logout must return to the login screen');
    await gui.checkScreenshot(page, 'user-profile-after-logout');
  });
});
