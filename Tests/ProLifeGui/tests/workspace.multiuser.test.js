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

const { test, newUserPage } = require('../fixtures/test');
const { WorkspacePage } = require('../pages');
const { canSeePage, visiblePages } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

// TimeFilterDelegate/CustomerFilterDelegate are both built on ImtCore's FilterDelegateBase, whose
// "ClearButton" child is only VISIBLE while the filter is active (FilterDelegateBase.qml:
// `visible: filterDelegateBase.isActive`) - confirmed live in the QML source, not guessed. Checking
// with countVisible() first (a single cheap DOM read) instead of just clicking means a test that never
// touched a filter pays nothing here, rather than a full ASSERT_TIMEOUT wait for a button that only
// exists in the DOM once active (clicking blind would otherwise cost a real ~10s per test on the common
// "nothing to clear" path). Needed once the 'interactions' block below shares one page/session across
// tests instead of a fresh reload per test - timeFilter/customerId are plain properties with no reset
// hook of their own (unlike the active TAB, which openDashboard() resets explicitly - see its own
// comment), so a filter set by an earlier test would otherwise silently leak into a later one's
// screenshot.
async function resetWorkspaceFilters(page) {
  if (await gui.countVisible(page, ['TimeFilterDelegate', 'ClearButton'])) {
    await gui.clickButton(page, ['TimeFilterDelegate', 'ClearButton']);
  }
  if (await gui.countVisible(page, ['CustomerFilterDelegate', 'ClearButton'])) {
    await gui.clickButton(page, ['CustomerFilterDelegate', 'ClearButton']);
  }
}

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

  // The remaining interactions only make sense for users who can actually open the Workspace. One
  // shared page/session for the whole block (see fixtures/test.js's newUserPage) instead of a fresh
  // boot per test - reset per-test via workspace.open() (resets the active tab back to Dashboard - see
  // resetWorkspaceFilters's own comment) plus an explicit filter reset, cheaply, without navigation.
  test.describe.serial('interactions', () => {
    let page, user, workspace;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      workspace = new WorkspacePage(page);
      // newUserPage() only opens a blank page - unlike the page fixture, nothing has navigated to the
      // app yet, so load it once here before the very first interaction.
      await workspace.reload();
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(async () => {
      test.skip(!canSeePage(user, 'Workspace'), 'user cannot see the Workspace page');
      await workspace.open();
      // Explicit reset, NOT implicit via re-navigation - see openDashboard()'s own comment: confirmed
      // live that a prior test leaving the Analytics tab active (e.g. 'analytics tab reflects time
      // filter') otherwise leaks into this test, hiding Dashboard-only controls like
      // CustomerFilterDelegate entirely ("1 element(s) exist but none became visible").
      await workspace.openDashboard();
      await resetWorkspaceFilters(page);
    });

    test('user actions tab', async () => {
      await workspace.openUserActions();
      await gui.checkScreenshot(page, 'workspace-user-actions');
    });

    test('analytics tab', async () => {
      await workspace.openAnalytics();
      await gui.checkScreenshot(page, 'workspace-analytics');
    });

    // The Analytics tab has its OWN Software/Hardware vs Orders toggle (StatisticsPage.qml - a
    // separate ViewBase/commandsController from WorkspacePage's own Dashboard/UserActions/Analytics
    // tabs), previously untested. Orders' own chart (OrderCreationLineChart) only renders for a user
    // with ViewOrders.
    test('analytics tab - orders sub-view', async () => {
      test.skip(!user.can('ViewOrders'), 'orders analytics sub-view requires ViewOrders');
      await workspace.openAnalytics();
      await workspace.openAnalyticsOrders();
      await gui.checkScreenshot(page, 'workspace-analytics-orders');
    });

    // The Dashboard-tab "time filter" test above proves the filter control itself works, but never
    // confirms the Analytics tab's OWN charts (GqlBarchartView/GqlLinechartView, bound to the same
    // root.timeFilter - WorkspacePage.qml) actually re-render under it, since that test never leaves
    // the Dashboard tab. Apply it while already on Analytics instead.
    test('analytics tab reflects time filter', async () => {
      await workspace.openAnalytics();
      await workspace.setTimeFilter('Month_Current');
      await gui.checkScreenshot(page, 'workspace-analytics-time-filter');
    });

    test('time filter - current month', async () => {
      await workspace.setTimeFilter('Month_Current');
      await gui.checkScreenshot(page, 'workspace-time-filter-month');
    });

    test('customer filter', async () => {
      test.skip(!user.can('ViewAccounts'), 'customer filter requires ViewAccounts');
      await workspace.setCustomerFilter('QUISS');
      await gui.checkScreenshot(page, 'workspace-customer-filter');
    });

    test('software card - view all', async () => {
      test.skip(!user.can('ViewLicenses'), 'software card requires ViewLicenses');
      await workspace.viewAllIn('SoftwareProducts');
      await gui.checkScreenshot(page, 'workspace-software-view-all');
    });

    test('software card - create new', async () => {
      test.skip(!user.can('ViewLicenses'), 'software card requires ViewLicenses');
      await workspace.createNewIn('SoftwareProducts');
      await gui.checkScreenshot(page, 'workspace-software-create-new');
    });

    test('orders card - view all', async () => {
      test.skip(!user.can('ViewOrders'), 'orders card requires ViewOrders');
      await workspace.viewAllIn('Orders');
      await gui.checkScreenshot(page, 'workspace-orders-view-all');
    });
  });
});
