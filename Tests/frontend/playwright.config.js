// playwright.config.js
const { defineConfig } = require('@playwright/test');

module.exports = defineConfig({
  timeout: 100000, // 100 секунд для каждого тестов (макс)
  testDir: './tests',
  globalSetup: require.resolve('./global-setup.js'),
  use: {
    headless: true,
    viewport: { width: 1400, height: 800 },
    screenshot: 'only-on-failure',
    baseURL: 'http://localhost:7778',
  },
  expect: {
    toHaveScreenshot: { threshold: 0.2 },
  },
  projects: [
    {
      name: 'authorized',
      testMatch: /tests\/auth\/.*\.test\.js/,
      use: {
        storageState: 'storageState.json',
	snapshotSuffix: 'win32',
      },
    },
    {
      name: 'guest',
      testIgnore: /tests\/auth\/.*\.test\.js/,
      use: {
	snapshotSuffix: 'win32',
      },
    }
  ],
});
