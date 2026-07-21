// Guest (unauthenticated) login flow - runs ONLY under the `guest` Playwright project.
//
// playwright.config.js gives the guest project no storageState, and matches only *.guest.test.js, so
// this is the one spec that exercises the actual AuthorizationPage. Before this file existed the guest
// project ran zero tests. Field objectNames (LoginInput / PasswordInput / LoginButton) come from
// ImtCore/Qml/imtauthgui/AuthorizationPage.qml and are the same ones global-setup uses to mint each
// user's storageState, so they are known-present.

const { test } = require('../fixtures/test');
const { byKey } = require('../fixtures/users');

test.describe('Guest / login', () => {
  test.beforeEach(async ({ page, gui }) => {
    await gui.reload(page);
  });

  // 1. An unauthenticated visitor lands on the login form.
  test('login form is shown', async ({ page, gui }) => {
    await gui.expectVisible(page, ['LoginInput'], 'login field should be visible for a guest');
    await gui.expectVisible(page, ['PasswordInput'], 'password field should be visible for a guest');
    await gui.expectVisible(page, ['LoginButton'], 'sign-in button should be visible for a guest');
    await gui.checkScreenshot(page, 'login-form');
  });

  // 2. Wrong credentials must NOT authenticate: the login form is still present afterwards.
  test('invalid credentials keep the user on the login page', async ({ page, gui }) => {
    await gui.login(page, 'definitely-not-a-user', 'wrong-password');
    await gui.expectVisible(page, ['LoginInput'], 'still on the login page after a failed sign-in');
    await gui.checkScreenshot(page, 'login-invalid');
  });

  // 3. Happy path: the pre-existing superuser can sign in and reach the app shell (menu appears).
  test('superuser can sign in', async ({ page, gui }) => {
    const su = byKey('su');
    await gui.login(page, su.login, su.password);
    await gui.expectVisible(page, ['MenuPanel'], 'the app menu should appear after a successful sign-in');
    await gui.checkScreenshot(page, 'login-success');
  });

  // 4. The show/hide password toggle (EyeButton) is a harmless client-side UI toggle (no submission, no
  // navigation), so unlike Register/Password-recovery below it's exercised for real rather than just
  // checked for presence. PasswordInput's underlying TextInput renders as a plain canvas-backed `<div>`
  // (confirmed live - no real `<input>`, so there's no `type` attribute to assert on structurally), so
  // the before/after screenshots (masked dots vs plain text) ARE the check here, same reasoning as
  // "remember me" below.
  test('eye button toggles password visibility', async ({ page, gui }) => {
    await gui.fill(page, ['PasswordInput'], 'SomeSecret123');
    await gui.checkScreenshot(page, 'login-password-masked');

    await gui.clickButton(page, ['EyeButton']);
    await gui.checkScreenshot(page, 'login-password-visible');

    // Toggle back - leaves the form in its default (masked) state, matching every other test in this file.
    await gui.clickButton(page, ['EyeButton']);
    await gui.checkScreenshot(page, 'login-password-masked-again');
  });

  // 5. "Remember me" defaults to checked (AuthorizationPage.qml: `checkState: Qt.Checked`) and is a
  // harmless local UI toggle (only read at the moment Sign in is clicked - see the Binding to
  // AuthorizationController.rememberMe), so it's toggled for real. No structural "is this checked"
  // signal is exposed by the generic CheckBox control (only its click MouseArea has an objectName), so
  // the before/after screenshots ARE the check here, matching this suite's own screenshot-first
  // convention for this class of control.
  test('remember me is checked by default and can be toggled', async ({ page, gui }) => {
    await gui.checkScreenshot(page, 'login-remember-me-default');
    await gui.clickButton(page, ['RememberMeCheckBox']);
    await gui.checkScreenshot(page, 'login-remember-me-unchecked');
  });

  // 6. Forgot password: opens PasswordRecoveryDialog.qml's real dialog (screenshotted), then dismissed
  // via Cancel - NEVER via "Check the email" (Enums.yes), which is the button that actually fires
  // checkEmailRequestSender.send() (a real GraphQL request). Button objectNames are auto-derived from
  // their (localized) text with spaces stripped (imtcontrols/Buttons/Button.qml:
  // `objectName: text.replace(/\s/g, '') + "Button"`), so "Cancel" -> "CancelButton" - the same
  // objectName editor dialogs elsewhere in this suite already use for their own Cancel button.
  test('forgot password link opens the recovery dialog, Cancel closes it without sending a request', async ({
    page,
    gui,
  }) => {
    // PasswordRecoveryLink is a bare MouseArea carrying its own objectName directly (no wrapper with a
    // nested [objectName="MouseArea"] child), so clickButton()/click() throws "has no visible
    // MouseArea" here (confirmed live) - clickSelf() clicks the addressed node's own bounding box
    // instead, exactly for this control shape.
    await gui.clickSelf(page, ['PasswordRecoveryLink']);
    await gui.expectVisible(page, ['Dialog'], 'password recovery dialog should open');

    // Fill the email field to prove it actually accepts input - still never clicking "Check the email"
    // (Enums.yes), the button that would fire the real GraphQL request.
    await gui.fill(page, ['EmailInput'], 'guest-test@example.com');
    await gui.checkScreenshot(page, 'login-password-recovery-dialog');

    await gui.clickButton(page, ['CancelButton']);
    await gui.expectHidden(page, ['Dialog'], 'password recovery dialog should close after Cancel');
    await gui.expectVisible(page, ['LoginInput'], 'back on the login form after cancelling');
  });

  // 7. Sign up: opens the registration dialog (AuthorizationPage.qml's inline Dialog hosting
  // UserGeneralEditor, screenshotted), then dismissed via its "Close" button (Enums.close) - NEVER via
  // "Sign up" (Enums.save), which is the button whose onFinished actually emits registerUser(userData)
  // (a real account-creation request). Same auto-derived-objectName reasoning as Cancel above:
  // "Close" -> "CloseButton".
  test('sign up link opens the registration dialog, Close dismisses it without registering', async ({ page, gui }) => {
    // RegisterUser is also a bare, self-named MouseArea - see clickSelf's own comment above.
    await gui.clickSelf(page, ['RegisterUser']);
    await gui.expectVisible(page, ['Dialog'], 'registration dialog should open');

    // Fill every field to prove the form actually accepts input - still never clicking "Sign up"
    // (Enums.save), the button whose onFinished fires the real registerUser(userData) request.
    // Field objectNames come from UserGeneralEditor.qml (this dialog reuses that same component,
    // already instrumented for the Administration "New user" editor - see administration.editor
    // .multiuser.test.js). Scoped under ['Dialog', ...]: UserGeneralEditor's own "PasswordInput"
    // collides with AuthorizationPage's login-form field of the same name, which is still in the DOM
    // (just visually covered) behind this modal - the Dialog scope disambiguates which one gets filled.
    await gui.fill(page, ['Dialog', 'UsernameInput'], 'guest_test_signup');
    await gui.fill(page, ['Dialog', 'UserNameInput'], 'Guest Test Signup');
    await gui.fill(page, ['Dialog', 'MailInput'], 'guest-signup@example.com');
    await gui.fill(page, ['Dialog', 'PasswordInput'], 'GuestTest_2026!');
    await gui.fill(page, ['Dialog', 'ConfirmPasswordInput'], 'GuestTest_2026!');
    await gui.checkScreenshot(page, 'login-signup-dialog');

    await gui.clickButton(page, ['CloseButton']);
    await gui.expectHidden(page, ['Dialog'], 'registration dialog should close after Close');
    await gui.expectVisible(page, ['LoginInput'], 'back on the login form after closing');
  });
});
