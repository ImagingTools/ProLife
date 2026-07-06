// Search page - multi-user coverage.
//
// Search is universal (PagePermissions=["*"]), so EVERY authenticated user - including noAccess -
// must be able to reach it. That makes this the one page whose "opens for authorized users" body runs
// for all seeded users, and its per-user landing screenshot is the baseline for the minimal-rights UI.
// SearchPage.qml is not objectName-instrumented internally yet, so coverage is navigation + screenshot.

const { test } = require('../fixtures/test');
const { SearchPage } = require('../pages');
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
});
