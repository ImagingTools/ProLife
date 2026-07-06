// Administration page - multi-user coverage.
//
// The Admin area (Users / Roles / Groups) is not objectName-instrumented internally yet, so coverage
// here is: (1) a per-user landing screenshot documenting exactly what each permission level lands on,
// and (2) for users who may see it, opening the page and asserting the AdministrationView root
// rendered. `adminManager` is the user that exercises this page without being a superuser.
//
// When the Admin sub-views are instrumented (like DeviceEditor was), extend this with command-bar
// gating (Add/Change/Remove User/Role/Group) the same way the collection specs do.

const { test } = require('../fixtures/test');
const { AdministrationPage } = require('../pages');
const { canSeePage } = require('../matrix/permissions');

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
});
