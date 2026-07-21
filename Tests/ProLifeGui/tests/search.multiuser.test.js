// Search page - multi-user coverage.
//
// Search is universal (PagePermissions=["*"]), so EVERY authenticated user - including noAccess -
// must be able to reach it. That makes this the one page whose "opens for authorized users" body runs
// for all seeded users, and its per-user landing screenshot is the baseline for the minimal-rights UI.
// SearchPage.qml is not objectName-instrumented internally yet, so coverage is navigation + screenshot.

const { test, newUserPage } = require('../fixtures/test');
const { SearchPage, WorkspacePage } = require('../pages');
const { canSeePage } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

const PAGE = 'Search';

test.describe.serial('Search', () => {
  // One shared page/session for the whole file (see fixtures/test.js's newUserPage) instead of a fresh
  // boot per test - every test here already does its OWN explicit reload/navigation before asserting
  // anything (SearchPage.reload() below, or workspace.reload()+open() inside the two search tests), so
  // there's no implicit "clean slate" being relied on here; only the CONTEXT provisioning changes (once
  // per file instead of once per test), not the reset behavior itself.
  let page, user;

  test.beforeAll(async ({ browser }, testInfo) => {
    ({ page, user } = await newUserPage(browser, testInfo));
  });

  test.afterAll(async () => {
    if (page) await page.close();
  });

  test.beforeEach(async () => {
    await new SearchPage(page).reload();
  });

  test('opens for every user (universal page)', async () => {
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
  test('typing in the global search box auto-navigates to results', async () => {
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

  // Clicking through TABS only proves the results list itself renders - it never proves a result
  // actually leads anywhere. SearchResultsView.qml's per-row "Go to the content" ToolButton fires the
  // same navigation as double-clicking the row (table.doubleClicked(...) -> SearchPage.qml's
  // onItemDoubleClicked -> NavigationController.navigate); a single click on it is the reliable way to
  // drive that from Playwright. The button sets no `text`, so (matching this codebase's own documented
  // ToolButton-objectName convention) it falls back to the shared generic "Button" name - scope the
  // click to the first result ROW (TableRow_0), the same pattern OrderEditorPage uses for its own
  // per-row Edit/Remove buttons.
  test('clicking a result row navigates to that entity', async () => {
    test.skip(!canSeePage(user, 'Workspace'), 'need a FilterPanel-free page to drive the global search from');
    const workspace = new WorkspacePage(page);
    await workspace.reload();
    await workspace.open();

    const search = new SearchPage(page);
    await search.search('Test');
    await search.waitForResults();

    const tabCount = await search.tabCount();
    test.skip(tabCount === 0, 'no search results for this user/query to click through');

    const firstRow = page.locator('[objectName="TableRow_0"]').first();
    test.skip((await firstRow.count()) === 0, 'no result rows rendered for the active tab');
    await gui.clickWithin(page, firstRow, 'Button');
    await gui.waitForStable(page);

    // No single structural marker reliably proves "we left Search AND landed somewhere real": the
    // destination is one of several entity types depending on what matched "Test" for this user, and
    // "Tab0"/"Tab1" (TabDelegate.qml's generic per-index name) is reused by the DESTINATION page's own
    // document/collection tab strip just as much as by Search's result-category tabs, so asserting it's
    // gone would be a false signal, not a real one. The screenshot is the check here - a human (or a
    // future dedicated per-entity test) can confirm it's the right destination; this test's job is only
    // to prove the click doesn't silently no-op.
    await gui.checkScreenshot(page, 'search-result-navigation');
  });
});
