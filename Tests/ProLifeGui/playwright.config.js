// ProLife GUI test configuration.
//
// Multi-user model: one Playwright *project* per test user (see fixtures/users.js).
// Every *.test.js therefore runs once per user, and snapshots are separated per user via
// snapshotPathTemplate ({projectName} = the user key). A page a given user cannot see simply
// produces a different baseline for that user - that difference *is* the permission check.
//
// By default only `fixtures/users.js`'s DEFAULT_USER_KEYS (su + fullAccess) get a project, to keep
// local/CI iteration fast - set PROLIFE_GUI_ALL_USERS=1 (or Run-CiTests.ps1 -AllUsers) to run every
// user and actually validate the full permission matrix (accountsViewer/noAccess/etc. seeing less).

const { defineConfig } = require('@playwright/test');
const { GUEST, authFile, activeUsers } = require('./fixtures/users');

const BASE_URL = process.env.PROLIFE_BASE_URL || 'http://localhost:17778';

// One project per ACTIVE user (see fixtures/users.js's activeUsers()) + a guest project. Each
// authenticated project loads the storageState global-setup produced for that user; guest starts
// unauthenticated.
const userProjects = activeUsers().map((u) => ({
  name: u.key,
  testDir: './tests',
  use: { storageState: authFile(u.key) },
}));

const guestProject = {
  name: GUEST.key,
  testDir: './tests',
  testMatch: /.*\.guest\.test\.js/, // guest-only specs opt in via the .guest.test.js suffix
  use: {},
};

// Authenticated projects run everything that is NOT a guest-only spec.
userProjects.forEach((p) => (p.testIgnore = /.*\.guest\.test\.js/));

module.exports = defineConfig({
  testDir: './tests',
  timeout: 60_000,
  expect: { timeout: 15_000 },
  fullyParallel: false, // QML app is a single shared server; keep tests WITHIN one file deterministic
  // Tried raising Playwright's own worker count (test-level parallelism only, no server changes) and
  // measured a hard cliff, not a graceful slowdown:
  //   workers: 10 (Playwright's auto-detected default) -> net::ERR_CONNECTION_REFUSED/RESET on
  //     page.goto for the large majority of tests - ProLifeServerTest.exe (a Debug build) refuses
  //     connections outright under that load.
  //   workers: 3 -> no more connection refusals, but ~93% of non-skipped tests instead failed on
  //     "MenuPanel > DevicesButton not found" (the WASM app's first render taking longer than the
  //     action timeout under shared CPU/server contention) - confirmed reproducible at BOTH a 3000ms
  //     and a 5000ms action timeout (lib/actions.js DEFAULT_TIMEOUT), so it isn't a timeout-tuning
  //     problem, it's a real capacity ceiling of this single shared Debug-build server stack (Puma +
  //     Lisa + ProLife + Postgres + N Chromium instances, all on one dev machine).
  //   workers: 1 -> the only setting that passes reliably (17/33 on the devices suite, reproduced
  //     multiple times). Revisit if this suite ever runs against a dedicated/Release-build backend, or
  //     if the server-side bottleneck itself gets investigated and fixed (out of scope here - this is
  //     a test-harness setting, not a server one).
  workers: 1,
  forbidOnly: !!process.env.CI,
  retries: process.env.CI ? 1 : 0,
  reporter: process.env.CI ? [['list'], ['junit', { outputFile: 'junit-report.xml' }]] : 'list',
  globalSetup: require.resolve('./global-setup.js'),

  // Per-user baselines: __screenshots__/<userKey>/<specPath>/<name>-<platform>.png
  snapshotPathTemplate: '{testDir}/__screenshots__/{projectName}/{testFilePath}/{arg}-{platform}{ext}',

  use: {
    headless: true,
    viewport: { width: 1920, height: 1080 },
    baseURL: BASE_URL,
    screenshot: 'only-on-failure',
    trace: 'retain-on-failure',
  },

  projects: [...userProjects, guestProject],
});
