// ProLife GUI test configuration.
//
// Multi-user model: one Playwright *project* per test user (see fixtures/users.js).
// Every *.test.js therefore runs once per user, and snapshots are separated per user via
// snapshotPathTemplate ({projectName} = the user key). A page a given user cannot see simply
// produces a different baseline for that user - that difference *is* the permission check.

const { defineConfig } = require('@playwright/test');
const { USERS, GUEST, authFile } = require('./fixtures/users');

const BASE_URL = process.env.PROLIFE_BASE_URL || 'http://localhost:17778';

// One project per authenticated user + a guest project. Each authenticated project loads the
// storageState global-setup produced for that user; guest starts unauthenticated.
const userProjects = USERS.map((u) => ({
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
  fullyParallel: false, // QML app is a single shared server; keep runs deterministic
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
