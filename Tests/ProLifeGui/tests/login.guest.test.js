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
});
