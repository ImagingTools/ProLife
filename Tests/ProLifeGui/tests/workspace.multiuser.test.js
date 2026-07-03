// Example spec: Workspace page, exercised under every user in the multi-user model.
//
// HOW THE MULTI-USER PART WORKS
// This single file is run once per Playwright *project*, and there is one project per test user
// (su / fullAccess / accountsViewer / noAccess - see playwright.config.js + fixtures/users.js). The
// `user` fixture tells the test which user it is running as. Screenshots are auto-separated per user
// by snapshotPathTemplate, so `workspace-start` produces one baseline per user:
//   tests/__screenshots__/su/workspace.multiuser.test.js/workspace-start-<platform>.png
//   tests/__screenshots__/noAccess/workspace.multiuser.test.js/workspace-start-<platform>.png
// A user who cannot see the Workspace simply has a different baseline - that difference is the check.

const { test } = require('../fixtures/test');
const { WorkspacePage } = require('../pages');
const { canSeePage, visiblePages } = require('../matrix/permissions');

test.describe('Workspace', () => {
  test.beforeEach(async ({ page }) => {
    const workspace = new WorkspacePage(page);
    await workspace.reload();
  });

  // 1. Landing state, per user. Always taken (even for users without ViewWorkspace) so the baseline
  //    documents exactly what each permission level lands on.
  test('workspace start', async ({ page, gui, user }) => {
    if (canSeePage(user, 'Workspace')) {
      await new WorkspacePage(page).open();
    }
    await gui.checkScreenshot(page, 'workspace-start');
  });

  // 2. Structural permission-matrix check on the navigation menu (cheap, exact, per user). This is
  //    the one place we assert visibility structurally so the intent is machine-checked, not only
  //    eyeballed via screenshot.
  test('menu reflects permissions', async ({ page, user }) => {
    const workspace = new WorkspacePage(page);
    const shouldSee = visiblePages(user);
    for (const pageId of Object.keys(require('../matrix/permissions').PAGE_PERMISSIONS)) {
      if (shouldSee.includes(pageId)) {
        await workspace.menu.expectHasPage(pageId);
      } else {
        await workspace.menu.expectNoPage(pageId);
      }
    }
  });

  // The remaining interactions only make sense for users who can actually open the Workspace.
  test.describe('interactions', () => {
    test.beforeEach(async ({ page, user }) => {
      test.skip(!canSeePage(user, 'Workspace'), 'user cannot see the Workspace page');
      await new WorkspacePage(page).open();
    });

    test('user actions tab', async ({ page, gui }) => {
      await new WorkspacePage(page).openUserActions();
      await gui.checkScreenshot(page, 'workspace-user-actions');
    });

    test('analytics tab', async ({ page, gui }) => {
      await new WorkspacePage(page).openAnalytics();
      await gui.checkScreenshot(page, 'workspace-analytics');
    });

    test('time filter - current month', async ({ page, gui }) => {
      await new WorkspacePage(page).setTimeFilter('Month_Current');
      await gui.checkScreenshot(page, 'workspace-time-filter-month');
    });

    test('customer filter', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewAccounts'), 'customer filter requires ViewAccounts');
      await new WorkspacePage(page).setCustomerFilter('QUISS');
      await gui.checkScreenshot(page, 'workspace-customer-filter');
    });

    test('software card - view all', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewLicenses'), 'software card requires ViewLicenses');
      await new WorkspacePage(page).viewAllIn('SoftwareProducts');
      await gui.checkScreenshot(page, 'workspace-software-view-all');
    });

    test('software card - create new', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewLicenses'), 'software card requires ViewLicenses');
      await new WorkspacePage(page).createNewIn('SoftwareProducts');
      await gui.checkScreenshot(page, 'workspace-software-create-new');
    });

    test('orders card - view all', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewOrders'), 'orders card requires ViewOrders');
      await new WorkspacePage(page).viewAllIn('Orders');
      await gui.checkScreenshot(page, 'workspace-orders-view-all');
    });
  });
});
