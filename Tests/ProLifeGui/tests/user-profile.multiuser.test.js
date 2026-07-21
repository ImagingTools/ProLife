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
const { canSeePage } = require('../matrix/permissions');

test.describe('User profile', () => {
  test.beforeEach(async ({ page, user }) => {
    const workspace = new WorkspacePage(page);
    await workspace.reload();
    if (canSeePage(user, 'Workspace')) await workspace.open();
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
    const tokenName = `gui-test-token-${Date.now()}`;

    await gui.openComboPopup(page, ['UserPanelButton']);
    await gui.clickPopupItemByIndex(page, 0); // Profile
    await gui.expectVisible(page, ['Dialog'], 'Profile must open in a modal dialog');
    await gui.click(page, ['Page_AccessTokens'], { what: 'Profile subpage "AccessTokens"' });

    await gui.clickButton(page, ['NewTokenButton']);
    await gui.expectVisible(page, ['TokenNameInput'], '"New Personal Access Token" dialog should open');

    await gui.fill(page, ['TokenNameInput'], tokenName);
    await gui.fill(page, ['TokenDescriptionInput'], 'Created by the GUI test suite');
    // Blur the description field (its value only commits onEditingFinished) by interacting with the
    // expiration combo next - also proves that combo is a real, working control, not just a default.
    await gui.select(page, ['ComboBox'], '30Days');

    // Click through the permissions tree for real (not just "Check All") - expand it, then check two
    // individual rows. Row content is data-dependent per user, but su/fullAccess always has a large
    // permission set, so rows 0 and 1 are reliably present.
    await gui.clickButton(page, ['ExpandAllButton']);
    await gui.checkScreenshot(page, 'access-tokens-new-dialog-tree-expanded');
    await gui.click(page, ['TreeRow_0', 'RowCheckBox'], { what: 'first permission row checkbox' });
    await gui.click(page, ['TreeRow_1', 'RowCheckBox'], { what: 'second permission row checkbox' });
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
    await gui.checkScreenshot(page, 'access-tokens-token-created');

    // Copy: no structural "was it copied" signal exposed (the icon just swaps to a checkmark and the
    // button disables itself), so the click succeeding without error plus a screenshot of the changed
    // icon state is the check here - same reasoning as this suite's own RememberMeCheckBox convention.
    await gui.clickButton(page, ['CopyTokenButton']);
    await gui.checkScreenshot(page, 'access-tokens-token-copied');

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
