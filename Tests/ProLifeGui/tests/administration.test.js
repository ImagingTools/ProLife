// Administration page.
//
// The Admin area hosts three MultiPageView subpages (Roles/Users/Groups), each a standard
// RemoteCollectionView + CommandsView (New/Edit/Remove) - see MultiPageView.qml's "Page_<id>" nav
// objectName and each *View.qml's per-field objectNames added for this coverage. Each subpage is
// gated independently of the others, so whether one is reachable is asked of the rendered sidebar.

const { test, newUserPage } = require('../fixtures/test');
const { AdministrationPage, UserCollectionPage } = require('../pages');
const gui = require('imtcore-gui-testkit/lib/gui');

test.describe('Administration', () => {
  // Scoped to its own describe so the reload does NOT also fire for the shared-page block(s)
  // below: an outer beforeEach runs for nested describes too, and requesting the `page` fixture
  // there created and booted a whole extra app instance per nested test that nothing then used.
  test.describe('cold load', () => {
    test.beforeEach(async ({ page }) => {
      await new AdministrationPage(page).reload();
    });

    test('landing', async ({ page, gui }) => {
      const admin = new AdministrationPage(page);
      test.skip(!(await admin.isAvailable()), 'Administration is not available to this user');
      await admin.open();
      await admin.expectLoaded();
      await gui.checkScreenshot(page, 'administration-landing');
    });
  });

  // --- Subpages -----------------------------------------------------------------------------------
  // One shared page/session for the whole block (see fixtures/test.js's newUserPage) instead of a
  // fresh boot per test - every test here is pure navigation (openSubPage) with no filter/tab state
  // that could leak between tests, so there's nothing to reset beyond landing on Administration once
  // in beforeAll; each test explicitly navigates to its OWN target subpage first thing anyway. Same
  // pattern already used by devices.collection.test.js's 'interactions' block.
  test.describe.serial('subpages', () => {
    let page, admin, available;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page } = await newUserPage(browser, testInfo));
      admin = new AdministrationPage(page);
      // newUserPage() only opens a blank page - unlike the page fixture, nothing has navigated to the
      // app yet, so load it once here before the very first interaction (see the identical note in
      // devices.collection.test.js's own 'interactions' beforeAll).
      await admin.reload();
      available = await admin.isAvailable();
      if (available) {
        await admin.open();
      }
    });

    test.afterAll(async () => {
      if (page) await page.context().close();
    });

    test.beforeEach(() => {
      test.skip(!available, 'Administration is not available to this user');
    });

    test('Roles subpage opens', async () => {
      test.skip(!(await admin.hasSubPage('Roles')), 'Roles is not available to this user');
      await admin.openSubPage('Roles');
      await gui.checkScreenshot(page, 'administration-roles-subpage');
    });

    test('Users subpage opens', async () => {
      test.skip(!(await admin.hasSubPage('Users')), 'Users is not available to this user');
      await admin.openSubPage('Users');
      // Last Connection is every fixture user's sign-in time from this very run.
      await gui.checkScreenshot(page, 'administration-users-subpage', await new UserCollectionPage(page).columnMasks(['lastConnection']));
    });

    test('Groups subpage opens', async () => {
      test.skip(!(await admin.hasSubPage('Groups')), 'Groups is not available to this user');
      await admin.openSubPage('Groups');
      await gui.checkScreenshot(page, 'administration-groups-subpage');
    });
  });

  // Role/User/Group EDITOR coverage ("New document" flows) lives in administration.editor.test.js,
  // which runs as its own user - see fixtures/users.js.
});
