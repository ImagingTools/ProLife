// Administration page - multi-user coverage.
//
// The Admin area hosts three MultiPageView subpages (Roles/Users/Groups), each a standard
// RemoteCollectionView + CommandsView (New/Edit/Remove) - see MultiPageView.qml's "Page_<id>" nav
// objectName and each *View.qml's per-field objectNames added for this coverage. Visibility of each
// subpage is its OWN ViewRoles/ViewUsers/ViewGroups permission (independent of the others and of the
// aggregate canSeePage('Administration') check, which is true if the user holds ANY one of them).

const { test, newUserPage } = require('../fixtures/test');
const {
  AdministrationPage,
  RoleCollectionPage,
  UserCollectionPage,
  GroupCollectionPage,
} = require('../pages');
const {
  canSeePage,
  canRunRoleCommand,
  canRunUserCommand,
  canRunGroupCommand,
} = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

const PAGE = 'Administration';

test.describe('Administration', () => {
  test.beforeEach(async ({ page }) => {
    await new AdministrationPage(page).reload();
  });

  // Landing screenshot for every user (users without admin rights land elsewhere - that difference is
  // the permission check).
  test('landing', async ({ page, gui, user }) => {
    if (canSeePage(user, PAGE)) await new AdministrationPage(page).open();
    await gui.checkScreenshot(page, 'administration-landing');
  });

  test('opens for authorized users', async ({ page, gui, user }) => {
    test.skip(!canSeePage(user, PAGE), 'user cannot see Administration');
    const admin = new AdministrationPage(page);
    await admin.open();
    await admin.expectLoaded();
    await gui.checkScreenshot(page, 'administration-view');
  });

  // --- Subpages: navigate to each + command bar gating -------------------------------------------
  // One shared page/session for the whole block (see fixtures/test.js's newUserPage) instead of a
  // fresh boot per test - every test here is pure navigation (openSubPage) with no filter/tab state
  // that could leak between tests, so there's nothing to reset beyond landing on Administration once
  // in beforeAll; each test explicitly navigates to its OWN target subpage first thing anyway. Same
  // pattern already used by devices.collection.multiuser.test.js's 'interactions' block.
  test.describe.serial('subpages', () => {
    let page, user;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      // newUserPage() only opens a blank page - unlike the page fixture, nothing has navigated to the
      // app yet, so load it once here before the very first interaction (see the identical note in
      // devices.collection.multiuser.test.js's own 'interactions' beforeAll).
      await new AdministrationPage(page).reload();
      if (canSeePage(user, PAGE)) {
        await new AdministrationPage(page).open();
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(() => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Administration');
    });

    test('Roles subpage opens', async () => {
      test.skip(!user.can('ViewRoles'), 'no ViewRoles permission');
      await new AdministrationPage(page).openSubPage('Roles');
      await gui.checkScreenshot(page, 'administration-roles-subpage');
    });

    test('Roles command bar reflects permissions', async () => {
      test.skip(!user.can('ViewRoles'), 'no ViewRoles permission');
      await new AdministrationPage(page).openSubPage('Roles');
      const roles = new RoleCollectionPage(page);
      for (const cmd of ['New', 'Edit', 'Remove']) {
        if (canRunRoleCommand(user, cmd)) await roles.commands.expectHasCommand(cmd);
        else await roles.commands.expectNoCommand(cmd);
      }
    });

    test('Users subpage opens', async () => {
      test.skip(!user.can('ViewUsers'), 'no ViewUsers permission');
      await new AdministrationPage(page).openSubPage('Users');
      await gui.checkScreenshot(page, 'administration-users-subpage');
    });

    test('Users command bar reflects permissions', async () => {
      test.skip(!user.can('ViewUsers'), 'no ViewUsers permission');
      await new AdministrationPage(page).openSubPage('Users');
      const users = new UserCollectionPage(page);
      for (const cmd of ['New', 'Edit', 'Remove']) {
        if (canRunUserCommand(user, cmd)) await users.commands.expectHasCommand(cmd);
        else await users.commands.expectNoCommand(cmd);
      }
    });

    test('Groups subpage opens', async () => {
      test.skip(!user.can('ViewGroups'), 'no ViewGroups permission');
      await new AdministrationPage(page).openSubPage('Groups');
      await gui.checkScreenshot(page, 'administration-groups-subpage');
    });

    test('Groups command bar reflects permissions', async () => {
      test.skip(!user.can('ViewGroups'), 'no ViewGroups permission');
      await new AdministrationPage(page).openSubPage('Groups');
      const groups = new GroupCollectionPage(page);
      for (const cmd of ['New', 'Edit', 'Remove']) {
        if (canRunGroupCommand(user, cmd)) await groups.commands.expectHasCommand(cmd);
        else await groups.commands.expectNoCommand(cmd);
      }
    });
  });

  // Role/User/Group EDITOR coverage ("New document" flows) lives in its own isolated spec -
  // administration.editor.multiuser.test.js, pinned to the dedicated `admEditor` user - because opening
  // a document tab here shares the same server-side per-user Collection Document Service workspace that
  // this file's own collection-navigation tests (run under su/fullAccess/adminManager) would otherwise
  // race against. See fixtures/users.js's admEditor comment.
});
