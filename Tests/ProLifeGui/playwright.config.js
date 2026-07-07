const { defineConfig } = require('@playwright/test');
const { GUEST, authFile, activeUsers } = require('./fixtures/users');

const BASE_URL = process.env.PROLIFE_BASE_URL || 'http://localhost:17778';

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
  timeout: 0,
  expect: { timeout: 15_000 },
  forbidOnly: !!process.env.CI,
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
