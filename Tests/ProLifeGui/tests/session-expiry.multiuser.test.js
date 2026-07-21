// Session expiry - GENERIC coverage.
//
// Doesn't simulate a live mid-session 401 (would need intercepting every GraphQL request, fragile and
// not what actually happens first for most users - they come back to a stale tab/reload). Instead
// clears the token the app itself checks on boot (same tokenStorageKey global-setup.js's own
// loginAndWaitForToken waits for: 'AuthorizationController/accessToken') and reloads - the realistic
// "token expired/was revoked while the tab was closed" path. No document tabs opened, so no isolated
// user needed (same reasoning as user-profile.multiuser.test.js).

const { test } = require('../fixtures/test');
const { WorkspacePage } = require('../pages');
const { canSeePage } = require('../matrix/permissions');

const TOKEN_STORAGE_KEY = 'AuthorizationController/accessToken';

test.describe('Session expiry', () => {
  test('missing/expired token redirects to the login screen on reload', async ({ page, gui, user }) => {
    const workspace = new WorkspacePage(page);
    await workspace.reload();
    if (canSeePage(user, 'Workspace')) await workspace.open();

    await page.evaluate((key) => localStorage.removeItem(key), TOKEN_STORAGE_KEY);
    await gui.reload(page);

    // A cold boot with nothing cached (we just cleared the token) can take noticeably longer than a
    // normal warm reload - same reasoning as login()'s own 20s allowance for the very first render.
    await gui.requireVisible(page, ['LoginInput'], { what: 'login field', timeout: 20000 });
    await gui.checkScreenshot(page, 'session-expired-login-screen');
  });
});
