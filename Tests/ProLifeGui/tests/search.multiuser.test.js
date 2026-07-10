// Search page - multi-user coverage.
//
// Search is universal (PagePermissions=["*"]), so EVERY authenticated user - including noAccess -
// must be able to reach it. That makes this the one page whose "opens for authorized users" body runs
// for all seeded users, and its per-user landing screenshot is the baseline for the minimal-rights UI.
// SearchPage.qml is not objectName-instrumented internally yet, so coverage is navigation + screenshot.

const { test } = require('../fixtures/test');
const { SearchPage, WorkspacePage } = require('../pages');
const { canSeePage } = require('../matrix/permissions');

const PAGE = 'Search';

test.describe('Search', () => {
  test.beforeEach(async ({ page }) => {
    await new SearchPage(page).reload();
  });

  test('opens for every user (universal page)', async ({ page, gui, user }) => {
    // Sanity-check the matrix itself: Search must be visible to everyone.
    test.skip(!canSeePage(user, PAGE), 'BUG: Search should be universal but matrix says hidden');
    await new SearchPage(page).open(); // open() hard-fails if the SearchButton is missing
    await gui.checkScreenshot(page, 'search-view');
  });

  // Real users mostly reach Search by TYPING, not by clicking the menu button: the global search box
  // (TopCenterPanelDecorator.qml, top-of-window, present on every page) auto-navigates here 500ms after
  // the last keystroke (SearchTextInput.qml's debounce -> "GlobalSearchActivated" - see SearchPage.js's
  // search()). Driven from Workspace specifically because it has no FilterPanel of its own - a
  // collection page's OWN search box shares the exact same objectName, so searching from one of those
  // would be ambiguous about which box actually got the text.
  test('typing in the global search box auto-navigates to results', async ({ page, gui, user }) => {
    test.skip(!canSeePage(user, 'Workspace'), 'need a FilterPanel-free page to drive the global search from');
    const workspace = new WorkspacePage(page);
    await workspace.reload();
    await workspace.open();

    const search = new SearchPage(page);
    await search.search('Test');
    await search.waitForResults();
    await gui.checkScreenshot(page, 'search-global-autonav-results');

    // Click through every result tab (one per matched entity type) - this user's permission set decides
    // how many exist, so don't assume a fixed count. A screenshot per tab would multiply with that
    // data-dependent count across every user; one at the end (after the LAST tab) is enough to prove
    // tabs genuinely switch content, while every click still hard-fails loudly if a tab is broken.
    const tabCount = await search.tabCount();
    for (let i = 0; i < tabCount; i++) {
      await search.clickTab(i);
    }
    if (tabCount > 0) {
      await gui.checkScreenshot(page, 'search-global-last-tab');
    }
  });
});
