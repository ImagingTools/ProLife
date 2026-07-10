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
  RoleEditorPage,
  UserCollectionPage,
  UserEditorPage,
  GroupCollectionPage,
  GroupEditorPage,
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
  test.describe('subpages', () => {
    test.beforeEach(async ({ page, user }) => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Administration');
      await new AdministrationPage(page).open();
    });

    test('Roles subpage opens', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewRoles'), 'no ViewRoles permission');
      await new AdministrationPage(page).openSubPage('Roles');
      await gui.checkScreenshot(page, 'administration-roles-subpage');
    });

    test('Roles command bar reflects permissions', async ({ page, user }) => {
      test.skip(!user.can('ViewRoles'), 'no ViewRoles permission');
      await new AdministrationPage(page).openSubPage('Roles');
      const roles = new RoleCollectionPage(page);
      for (const cmd of ['New', 'Edit', 'Remove']) {
        if (canRunRoleCommand(user, cmd)) await roles.commands.expectHasCommand(cmd);
        else await roles.commands.expectNoCommand(cmd);
      }
    });

    test('Users subpage opens', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewUsers'), 'no ViewUsers permission');
      await new AdministrationPage(page).openSubPage('Users');
      await gui.checkScreenshot(page, 'administration-users-subpage');
    });

    test('Users command bar reflects permissions', async ({ page, user }) => {
      test.skip(!user.can('ViewUsers'), 'no ViewUsers permission');
      await new AdministrationPage(page).openSubPage('Users');
      const users = new UserCollectionPage(page);
      for (const cmd of ['New', 'Edit', 'Remove']) {
        if (canRunUserCommand(user, cmd)) await users.commands.expectHasCommand(cmd);
        else await users.commands.expectNoCommand(cmd);
      }
    });

    test('Groups subpage opens', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewGroups'), 'no ViewGroups permission');
      await new AdministrationPage(page).openSubPage('Groups');
      await gui.checkScreenshot(page, 'administration-groups-subpage');
    });

    test('Groups command bar reflects permissions', async ({ page, user }) => {
      test.skip(!user.can('ViewGroups'), 'no ViewGroups permission');
      await new AdministrationPage(page).openSubPage('Groups');
      const groups = new GroupCollectionPage(page);
      for (const cmd of ['New', 'Edit', 'Remove']) {
        if (canRunGroupCommand(user, cmd)) await groups.commands.expectHasCommand(cmd);
        else await groups.commands.expectNoCommand(cmd);
      }
    });
  });

  // --- Role editor: one continuous new document ---------------------------------------------------
  test.describe.serial('Role editor', () => {
    let page, user, editor;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      if (canRunRoleCommand(user, 'New')) {
        const admin = new AdministrationPage(page);
        await admin.reload();
        await admin.open();
        await admin.openSubPage('Roles');
        await new RoleCollectionPage(page).newItem();
        editor = new RoleEditorPage(page);
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(() => {
      test.skip(!canRunRoleCommand(user, 'New'), 'cannot create a role (ChangeRole)');
    });

    test('empty new editor', async () => {
      await gui.checkScreenshot(page, 'role-editor-new-empty');
    });

    // Role Name and Description are both plain TextInput fields - one combined screenshot documents
    // both, matching the same-mechanism consolidation used across the other editor suites.
    test('fill fields and save', { tag: '@mutating' }, async () => {
      await editor.setRoleName('ProLifeGui Test Role');
      await editor.setDescription('Test role description');
      await gui.checkScreenshot(page, 'role-editor-new-filled');
      await editor.save();
      await gui.checkScreenshot(page, 'role-editor-new-saved');
    });
  });

  // --- User editor: one continuous new document ---------------------------------------------------
  test.describe.serial('User editor', () => {
    let page, user, editor;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      if (canRunUserCommand(user, 'New')) {
        const admin = new AdministrationPage(page);
        await admin.reload();
        await admin.open();
        await admin.openSubPage('Users');
        await new UserCollectionPage(page).newItem();
        editor = new UserEditorPage(page);
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(() => {
      test.skip(!canRunUserCommand(user, 'New'), 'cannot create a user (ChangeUser)');
    });

    test('empty new editor', async () => {
      await gui.checkScreenshot(page, 'user-editor-new-empty');
    });

    // Username/Name/Email are all plain TextInput fields (same fill/verify mechanism) - one combined
    // screenshot documents the general block. Password/Confirm are their own step: a NEW user requires
    // a password (UserCollectionView.qml's documentValidator), and mismatched confirm is genuinely
    // distinct, worth its own visual check.
    test('fill general information', async () => {
      await editor.setUsername('prolifegui_test_user');
      await editor.setName('ProLifeGui Test User');
      await editor.setEmail('prolifegui_test_user@example.com');
      await gui.checkScreenshot(page, 'user-editor-new-general-filled');
    });

    test('set password and confirm, then save', { tag: '@mutating' }, async () => {
      await editor.setPassword('ProLifeGuiTest_2026!');
      await editor.setConfirmPassword('ProLifeGuiTest_2026!');
      await gui.checkScreenshot(page, 'user-editor-new-password-set');
      await editor.save();
      await gui.checkScreenshot(page, 'user-editor-new-saved');
    });
  });

  // --- Group editor: one continuous new document --------------------------------------------------
  test.describe.serial('Group editor', () => {
    let page, user, editor;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      if (canRunGroupCommand(user, 'New')) {
        const admin = new AdministrationPage(page);
        await admin.reload();
        await admin.open();
        await admin.openSubPage('Groups');
        await new GroupCollectionPage(page).newItem();
        editor = new GroupEditorPage(page);
      }
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(() => {
      test.skip(!canRunGroupCommand(user, 'New'), 'cannot create a group (ChangeGroups)');
    });

    test('empty new editor', async () => {
      await gui.checkScreenshot(page, 'group-editor-new-empty');
    });

    test('fill fields and save', { tag: '@mutating' }, async () => {
      await editor.setName('ProLifeGui Test Group');
      await editor.setDescription('Test group description');
      await gui.checkScreenshot(page, 'group-editor-new-filled');
      await editor.save();
      await gui.checkScreenshot(page, 'group-editor-new-saved');
    });
  });
});
