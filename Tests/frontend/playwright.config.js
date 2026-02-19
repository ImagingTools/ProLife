// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

// playwright.config.js
const { defineConfig } = require('@playwright/test');

module.exports = defineConfig({
  timeout: 0,
  testDir: './tests',
  globalSetup: require.resolve('./global-setup.js'),
  //snapshotPathTemplate: '{testDir}/{testFilePath}-snapshots/{arg}-{projectName}-win32{ext}',
  use: {
    headless: true,
    viewport: { width: 1400, height: 800 },
    screenshot: 'only-on-failure',
    baseURL: 'http://localhost:7778',
  },
  projects: [
    {
      name: 'authorized',
      testMatch: /tests\/authorized\/.*\.test\.js/,
      use: {
        storageState: 'storageState.json',
      },
    },
    {
      name: 'guest',
      testIgnore: /tests\/authorized\/.*\.test\.js/,
      use: {
      },
    }
  ],
});
