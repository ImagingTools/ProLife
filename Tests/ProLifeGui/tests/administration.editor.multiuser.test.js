// Administration EDITORS (Role/User/Group "New document") - isolated coverage.
//
// Split out from administration.multiuser.test.js: these open document tabs via the same server-side
// Collection Document Service used by Devices/Orders/Accounts/Software editors (workspace keyed by
// userId only - see fixtures/users.js's admEditor comment and
// ImtCore/Include/imtservergql/CCollectionDocumentServiceControllerComp.cpp). Running them under a
// matrix user (su/fullAccess/adminManager) that ALSO runs administration.multiuser.test.js's own
// collection-navigation tests concurrently risks the same cross-file tab-closing collision fixed for
// the domain editors - so this spec is pinned to its own dedicated `admEditor` user (see
// fixtures/users.js's isolatedSpec) instead.

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
const { canRunRoleCommand, canRunUserCommand, canRunGroupCommand } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

// search()'s own settle wait (gui.fill's internal waitForStable, 400ms DOM-quiet) can resolve DURING
// the search box's debounce delay - i.e. before the debounced filter request even fires - not after
// the filtered results actually replace the table. Left unguarded, selectRow(0)/editItem() then race
// ahead of the real filter and click against the stale (pre-filter) table. Wait for the searched text
// to actually appear in a visible row first, so selectRow(0) always acts on the real result.
async function waitForSearchResult(page, text) {
  await page
    .locator('[objectName^="TableRow_"][visible]')
    .filter({ hasText: text })
    .first()
    .waitFor({ state: 'visible', timeout: 15000 });
}

// Even AFTER the row is confirmed visible (waitForSearchResult above), selectRow(0)'s click can still
// silently miss: confirmed live via screenshot (a row showing the right text, but unhighlighted, with
// Edit/Remove greyed out and "Please select an item..." on the right) - the click landed but never
// registered a selection, most likely the exact same class of issue gui.openComboPopup's own doc
// comment already describes for combo boxes ("a click that lands right after an unrelated DOM update
// ... can occasionally miss/no-op on WASM canvas coordinates without throwing"), here caused by the
// search results table settling into place right as the click fires. Every following step then
// silently no-ops too - editor.<field>.waitForValue() returns without waiting at all when its path
// doesn't resolve to a readable TextInput (see waitForTextInputValue's own "not a readable TextInput
// at this path" comment) - so the real failure only surfaces much later as an opaque 60s test timeout
// on an unrelated line. Retry the select+edit pair (not just the click) up to 3 times, verifying
// navigation actually happened via gui.requireVisible (which DOES throw on a missing field, unlike
// waitForValue) before trusting it.
async function openSearchResultEditor(page, collectionPage, identifyingFieldPath) {
  const ATTEMPTS = 3;
  for (let attempt = 1; attempt <= ATTEMPTS; attempt++) {
    await collectionPage.selectRow(0);
    await collectionPage.editItem();
    try {
      await gui.requireVisible(page, identifyingFieldPath, { timeout: 3000 });
      return;
    } catch (_) {
      if (attempt === ATTEMPTS) {
        throw new Error(`Edit never navigated into the editor after ${attempt} attempts (selectRow/editItem click likely missed)`);
      }
    }
  }
}

// Same race as openSearchResultEditor above, confirmed live for Remove too (a run under heavier host
// load hit "YesButton not found" - removeItem()'s click landed but the confirm Dialog never opened,
// because the preceding selectRow(0) never actually registered a selection). Retries the select+remove
// pair, verifying the Yes/No confirm Dialog actually appeared before trusting it.
async function removeSearchResult(page, collectionPage) {
  const ATTEMPTS = 3;
  for (let attempt = 1; attempt <= ATTEMPTS; attempt++) {
    await collectionPage.selectRow(0);
    await collectionPage.removeItem();
    try {
      await gui.requireVisible(page, ['YesButton'], { timeout: 3000 });
      return;
    } catch (_) {
      if (attempt === ATTEMPTS) {
        throw new Error(`Remove never opened its confirm dialog after ${attempt} attempts (selectRow/removeItem click likely missed)`);
      }
    }
  }
}

test.describe('Administration / editors', () => {
  // --- Role editor: one continuous new document ---------------------------------------------------
  test.describe.serial('Role editor', () => {
    let page, user, editor, createdRoleName;

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
    // both, matching the same-mechanism consolidation used across the other editor suites. Name carries
    // a timestamp so the Edit/Remove tests below can reliably relocate THIS run's own record (search by
    // name) instead of an ambiguous duplicate left by a previous run.
    test('fill fields and save', { tag: '@mutating' }, async () => {
      createdRoleName = `ProLifeGui Test Role ${Date.now()}`;
      await editor.setRoleName(createdRoleName);
      await editor.setDescription('Test role description');
      await gui.checkScreenshot(page, 'role-editor-new-filled');
      await editor.save();
      await gui.checkScreenshot(page, 'role-editor-new-saved');
    });

    // Edit/Remove exercise the OTHER two document-service entry points (GetRepresentation via the
    // collection's own Edit command, and the Remove confirm flow) against the just-created record -
    // "New" only proves creation works, not that the record can be found and reopened afterwards.
    test('edit existing role and save', { tag: '@mutating' }, async () => {
      test.skip(!createdRoleName, 'no role was created above to edit');
      // Role/User/Group editors are a single-page view with breadcrumb back-navigation (NOT the
      // tabbed MultiDocumentCollectionView Devices/Orders/etc. use - confirmed live: no CloseButton
      // exists here at all). admin.reload() below is a full page.goto, which already discards this
      // view without needing an explicit "close" step first.
      const roles = new RoleCollectionPage(page);
      const admin = new AdministrationPage(page);
      await admin.reload();
      await admin.open();
      await admin.openSubPage('Roles');
      await roles.search(createdRoleName);
      await waitForSearchResult(page, createdRoleName);
      await openSearchResultEditor(page, roles, ['RoleNameInput']);
      editor = new RoleEditorPage(page);
      await editor.roleName.waitForValue(createdRoleName);
      const editedDescription = `Edited description ${Date.now()}`;
      await editor.setDescription(editedDescription);
      await gui.checkScreenshot(page, 'role-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'role-editor-edit-saved');
    });

    test('remove confirmation dialog', { tag: '@mutating' }, async () => {
      test.skip(!createdRoleName, 'no role was created above to remove');
      test.skip(!canRunRoleCommand(user, 'Remove'), 'no Remove permission');
      const roles = new RoleCollectionPage(page);
      const admin = new AdministrationPage(page);
      await admin.reload();
      await admin.open();
      await admin.openSubPage('Roles');
      await roles.search(createdRoleName);
      await waitForSearchResult(page, createdRoleName);
      await removeSearchResult(page, roles);
      await gui.checkScreenshot(page, 'role-editor-remove-confirm');
      await gui.clickButton(page, ['YesButton']);
      await gui.checkScreenshot(page, 'role-editor-removed');
    });
  });

  // --- User editor: one continuous new document ---------------------------------------------------
  test.describe.serial('User editor', () => {
    let page, user, editor, createdUsername;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      if (canRunUserCommand(user, 'New')) {
        const admin = new AdministrationPage(page);
        await admin.reload();
        await admin.open();
        await admin.openSubPage('Users');
        await new UserCollectionPage(page).newItem();
        editor = new UserEditorPage(page);
        // Computed here (not inside 'fill general information') because that test has no @mutating
        // tag and never runs in the @mutating-only phase - leaving createdUsername unset there would
        // silently skip the Edit/Remove tests below in that phase (test.skip(!createdUsername, ...)),
        // exactly the way it did before this was hoisted out.
        createdUsername = `prolifegui_test_user_${Date.now()}`;
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
    // distinct, worth its own visual check. Username carries a timestamp - it must be unique server-side
    // AND the Edit/Remove tests below need it to reliably relocate THIS run's own record.
    //
    // Tagged @mutating (despite not saving anything itself) so it still runs in phase 2's @mutating-only
    // invocation, immediately before 'set password and confirm, then save' - without this tag it only
    // ran in phase 1, so in phase 2 the Username/Name/Email fields were NEVER FILLED before Save,
    // silently saving a user with an empty username; the Edit/Remove tests further down then searched
    // for `createdUsername` and correctly found nothing (confirmed live via screenshot: "(1-0/0)").
    // Matches Role/Group's own convention in this same file, where fill+save is already one single
    // @mutating test rather than split in two.
    test('fill general information', { tag: '@mutating' }, async () => {
      await editor.setUsername(createdUsername);
      await editor.setName('ProLifeGui Test User');
      await editor.setEmail(`${createdUsername}@example.com`);
      await gui.checkScreenshot(page, 'user-editor-new-general-filled');
    });

    // UserView.qml's "Roles" and "Groups" sections (both GqlBasedItemSelectElementView) let a user be
    // assigned via a filterable picker popup (FilterableSelectPopup.qml). A single click on a row
    // toggles its selection immediately - there's no separate confirm/OK button, closing the popup
    // (Escape - FilterableSelectPopup's own Shortcut) is what "confirms" the picker session. Position-
    // based selection (not by name): the picker's option list is live seeded data with no stable text
    // to assert on - same convention as DeviceEditorPage's setDeviceTypeByIndex. The assigned item then
    // shows up as a removable chip (AssignedItem_<index>, RemoveButton within it).
    async function addFirstOption(addButtonName) {
      await gui.clickButton(page, [addButtonName]);
      await gui.expectVisible(page, ['FilterableSelectPopup'], 'picker should open');
      await gui.click(page, ['FilterableSelectItem_0'], { what: 'first picker result' });
      await gui.dismissDialog(page);
    }

    test('add group: pick the first result, it appears as a chip, then remove it', async () => {
      await addFirstOption('AddGroups');
      await gui.expectVisible(page, ['AssignedItem_0'], 'the picked group should appear as an assigned chip');
      await gui.checkScreenshot(page, 'user-editor-group-assigned');

      await gui.click(page, ['AssignedItem_0', 'RemoveButton'], { what: 'remove the assigned group chip' });
      await gui.expectHidden(page, ['AssignedItem_0'], 'the chip should be gone after Remove');
    });

    test('add role: pick the first result, it appears as a chip, then remove it', async () => {
      await addFirstOption('AddRoles');
      await gui.expectVisible(page, ['AssignedItem_0'], 'the picked role should appear as an assigned chip');
      await gui.checkScreenshot(page, 'user-editor-role-assigned');

      await gui.click(page, ['AssignedItem_0', 'RemoveButton'], { what: 'remove the assigned role chip' });
      await gui.expectHidden(page, ['AssignedItem_0'], 'the chip should be gone after Remove');
    });

    test('set password and confirm, then save', { tag: '@mutating' }, async () => {
      await editor.setPassword('ProLifeGuiTest_2026!');
      await editor.setConfirmPassword('ProLifeGuiTest_2026!');
      await gui.checkScreenshot(page, 'user-editor-new-password-set');
      await editor.save();
      await gui.checkScreenshot(page, 'user-editor-new-saved');
    });

    // Edit/Remove exercise the OTHER two document-service entry points (GetRepresentation via the
    // collection's own Edit command, and the Remove confirm flow) against the just-created record -
    // "New" only proves creation works, not that the record can be found and reopened afterwards.
    test('edit existing user and save', { tag: '@mutating' }, async () => {
      test.skip(!createdUsername, 'no user was created above to edit');
      // Single-page view with breadcrumb back-navigation, no CloseButton - see the identical note in
      // the Role editor block above.
      const users = new UserCollectionPage(page);
      const admin = new AdministrationPage(page);
      await admin.reload();
      await admin.open();
      await admin.openSubPage('Users');
      await users.search(createdUsername);
      await waitForSearchResult(page, createdUsername);
      await openSearchResultEditor(page, users, ['UsernameInput']);
      editor = new UserEditorPage(page);
      await editor.username.waitForValue(createdUsername);
      const editedName = `Edited Name ${Date.now()}`;
      await editor.setName(editedName);
      await gui.checkScreenshot(page, 'user-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'user-editor-edit-saved');
    });

    test('remove confirmation dialog', { tag: '@mutating' }, async () => {
      test.skip(!createdUsername, 'no user was created above to remove');
      test.skip(!canRunUserCommand(user, 'Remove'), 'no Remove permission');
      const users = new UserCollectionPage(page);
      const admin = new AdministrationPage(page);
      await admin.reload();
      await admin.open();
      await admin.openSubPage('Users');
      await users.search(createdUsername);
      await waitForSearchResult(page, createdUsername);
      await removeSearchResult(page, users);
      await gui.checkScreenshot(page, 'user-editor-remove-confirm');
      await gui.clickButton(page, ['YesButton']);
      await gui.checkScreenshot(page, 'user-editor-removed');
    });
  });

  // --- Group editor: one continuous new document --------------------------------------------------
  test.describe.serial('Group editor', () => {
    let page, user, editor, createdGroupName;

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

    // Name carries a timestamp so the Edit/Remove tests below can reliably relocate THIS run's own
    // record (search by name) instead of an ambiguous duplicate left by a previous run.
    test('fill fields and save', { tag: '@mutating' }, async () => {
      createdGroupName = `ProLifeGui Test Group ${Date.now()}`;
      await editor.setName(createdGroupName);
      await editor.setDescription('Test group description');
      await gui.checkScreenshot(page, 'group-editor-new-filled');
      await editor.save();
      await gui.checkScreenshot(page, 'group-editor-new-saved');
    });

    // Edit/Remove exercise the OTHER two document-service entry points (GetRepresentation via the
    // collection's own Edit command, and the Remove confirm flow) against the just-created record -
    // "New" only proves creation works, not that the record can be found and reopened afterwards.
    test('edit existing group and save', { tag: '@mutating' }, async () => {
      test.skip(!createdGroupName, 'no group was created above to edit');
      // Single-page view with breadcrumb back-navigation, no CloseButton - see the identical note in
      // the Role editor block above.
      const groups = new GroupCollectionPage(page);
      const admin = new AdministrationPage(page);
      await admin.reload();
      await admin.open();
      await admin.openSubPage('Groups');
      await groups.search(createdGroupName);
      await waitForSearchResult(page, createdGroupName);
      await openSearchResultEditor(page, groups, ['GroupNameInput']);
      editor = new GroupEditorPage(page);
      await editor.name.waitForValue(createdGroupName);
      const editedDescription = `Edited description ${Date.now()}`;
      await editor.setDescription(editedDescription);
      await gui.checkScreenshot(page, 'group-editor-edit-changed');
      await editor.save();
      await gui.checkScreenshot(page, 'group-editor-edit-saved');
    });

    test('remove confirmation dialog', { tag: '@mutating' }, async () => {
      test.skip(!createdGroupName, 'no group was created above to remove');
      test.skip(!canRunGroupCommand(user, 'Remove'), 'no Remove permission');
      const groups = new GroupCollectionPage(page);
      const admin = new AdministrationPage(page);
      await admin.reload();
      await admin.open();
      await admin.openSubPage('Groups');
      await groups.search(createdGroupName);
      await waitForSearchResult(page, createdGroupName);
      await removeSearchResult(page, groups);
      await gui.checkScreenshot(page, 'group-editor-remove-confirm');
      await gui.clickButton(page, ['YesButton']);
      await gui.checkScreenshot(page, 'group-editor-removed');
    });
  });
});
