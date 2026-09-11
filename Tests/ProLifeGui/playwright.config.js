const { defineConfig } = require('@playwright/test');
const { buildProjects } = require('imtcore-gui-testkit/playwrightConfig/buildProjects');
const { GUEST, authFile, activeUsers } = require('./fixtures/users');
const { canSeePage } = require('./matrix/permissions');

const BASE_URL = process.env.PROLIFE_BASE_URL || 'http://localhost:17778';

// Which page each spec exercises, so a user who cannot open that page never has the spec scheduled at
// all (see buildProjects' specPages). Verified before enabling: a restricted user's "landing"
// baselines for pages it cannot see are byte-identical to one another - noAccess's seven were
// literally one PNG repeated - so nothing is lost by not producing them, while each one costs a full
// app boot to produce and compare.
//
// Specs mapped to null are deliberately universal and stay scheduled for everybody:
//   workspace  - owns "menu reflects permissions", the structural check that a restricted user sees
//                LESS; excluding it for those users would remove the very assertion they exist for
//   support    - the Tickets page is universal ('*' in PAGE_PERMISSIONS)
//   search / user-profile / session-expiry - no page permission gates them
// Editor and document-tab specs are absent because their isolatedSpec users already pin them.
const SPEC_PAGES = {
  'accounts.collection.multiuser.test.js': 'Accounts',
  'devices.collection.multiuser.test.js': 'Devices',
  'orders.collection.multiuser.test.js': 'Orders',
  // The one editor spec without an isolatedSpec user of its own (see fixtures/users.js's note on
  // ordEditor), so unlike its siblings it does need mapping here.
  'orders.editor.multiuser.test.js': 'Orders',
  'software.collection.multiuser.test.js': 'SoftwareProducts',
  'administration.multiuser.test.js': 'Administration',
  'organizations.multiuser.test.js': 'Tenants',
  'workspace.multiuser.test.js': null,
  'support.multiuser.test.js': null,
  'search.multiuser.test.js': null,
  'user-profile.multiuser.test.js': null,
  'session-expiry.multiuser.test.js': null,
};

// @mutating tests run serially against one shared database (Run-CiTests.ps1 phase 2, --workers=1), so
// their wall clock is linear in the number of users and no worker count helps. Re-running the same
// save as a seventh user re-proves the FLOW, not the permission - whether that user may save at all is
// already decided structurally by the command-bar checks. Keep the two broad users (su for the admin
// path, fullAccess for the real-granted-permission path); every isolatedSpec user keeps its own
// mutating tests automatically, since its spec runs under that user and nobody else.
const MUTATING_USER_KEYS = ['su', 'fullAccess'];

module.exports = defineConfig({
  testDir: './tests',
  // A per-test cap, not 0 (unlimited) - with an unlimited timeout, a test whose action is waiting on a
  // server that has crashed/died mid-run (confirmed live: ProLifeServerTest.exe went down under a
  // heavier concurrent-workers load) hangs forever instead of failing, stalling the whole run with no
  // further output. 60s is comfortably above the slowest observed real test (~20s, the Bind dialog's
  // multi-step flow) while still bounding a dead-server hang to a single minute instead of indefinitely.
  timeout: 60_000,
  expect: { timeout: 15_000 },
  forbidOnly: !!process.env.CI,
  // NOTE: workers > 1 has a known structural risk - every project is one fixture USER, and ProLife
  // keeps that user's open document tabs / view state in server-side workspace state
  // (MultiDocumentCollectionView.qml). Two tests running as the SAME user concurrently share - and can
  // corrupt - one workspace: worker A opens a "New" editor tab while worker B's navigation closes it,
  // so the editor/collection command bars flicker between present and absent and tests fail
  // nondeterministically (empirically: at 2 workers the editor specs fail, at 1 they pass). That risk
  // is scoped to tests sharing a PROJECT (user) - different projects/users are independent, so most of
  // the flakiness risk from raising this is confined to same-user test files running concurrently.
  // Set higher for throughput (e.g. full-suite/all-users runs where occasional reruns are acceptable);
  // drop back to 1 for a trustworthy single-pass verification run.
  workers: 10,
  // Every test runs exactly once, everywhere - no retries, so a flaky/broken test reports red
  // immediately instead of being masked by a re-run.
  retries: 0,
  // The HTML report bundles each failure's screenshot/diff/trace into one browsable page - reading
  // failures off the CI agent's disk (or the raw junit XML) means digging through
  // test-results-phase*/**/*.png by hand; `open: 'never'` keeps this from popping a browser window on
  // a headless CI agent (it's meant to be published as a build artifact and opened later, not shown
  // live). Each phase gets its own report dir via PLAYWRIGHT_HTML_OUTPUT_DIR (set by Run-CiTests.ps1,
  // same reasoning as PLAYWRIGHT_OUTPUT_DIR below - the second phase would otherwise silently overwrite
  // the first's).
  reporter: process.env.CI
    ? [
        ['list'],
        ['junit', { outputFile: process.env.PLAYWRIGHT_JUNIT_OUTPUT || 'junit-report.xml' }],
        ['html', { outputFolder: process.env.PLAYWRIGHT_HTML_OUTPUT_DIR || 'playwright-report', open: 'never' }],
      ]
    : 'list',
  globalSetup: require.resolve('./global-setup.js'),

  // Run-CiTests.ps1 invokes "npx playwright test" TWICE per run (a read-only phase at the configured
  // `workers`, then a serial @mutating phase - see its Invoke-PlaywrightSuite) against this ONE config.
  // Playwright clears outputDir (and truncates the junit file above) at the START of every invocation,
  // so without per-phase paths the second invocation silently wipes the first phase's screenshots/
  // diffs/traces before anyone can look at them - confirmed live: a run with real phase-1 failures
  // left test-results/ empty and junit-report.xml showing only the all-green phase 2 once phase 2 had
  // run. PLAYWRIGHT_OUTPUT_DIR (set per-phase by Run-CiTests.ps1) keeps each phase's artifacts in its
  // own directory; unset (any other invocation, e.g. a developer's ad-hoc `npx playwright test`) falls
  // back to Playwright's own default so nothing changes outside the two-phase script.
  outputDir: process.env.PLAYWRIGHT_OUTPUT_DIR || 'test-results',

  // Per-user baselines: __screenshots__/<userKey>/<specPath>/<name>-<platform>.png
  snapshotPathTemplate: '{testDir}/__screenshots__/{projectName}/{testFilePath}/{arg}-{platform}{ext}',

  use: {
    headless: true,
    viewport: { width: 1920, height: 1080 },
    baseURL: BASE_URL,
    screenshot: 'only-on-failure',
    trace: 'off',
    // The PAT "copy token" button calls the browser clipboard API directly; Chromium denies
    // clipboard-write by default for automated contexts unless the permission is granted up front.
    permissions: ['clipboard-read', 'clipboard-write'],
  },

  projects: buildProjects({
    users: activeUsers(),
    guest: GUEST,
    authFile,
    specPages: SPEC_PAGES,
    canSeePage,
    mutatingUserKeys: MUTATING_USER_KEYS,
  }),
});
