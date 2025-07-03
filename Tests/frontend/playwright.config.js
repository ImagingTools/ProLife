// playwright.config.js
const { defineConfig } = require('@playwright/test');

module.exports = defineConfig({
  timeout: 100000, // 100 секунд для каждого тестов (макс)
  testDir: './tests',
  globalSetup: require.resolve('./global-setup.js'),
  snapshotPathTemplate: '{testDir}/{testFilePath}-snapshots/{arg}-{projectName}-win32{ext}',
  use: {
    headless: true,
    viewport: { width: 1400, height: 800 },
    screenshot: 'only-on-failure',
    baseURL: 'http://localhost:7778',
  },
  expect: {
    toHaveScreenshot: { threshold: 0.3 },
  },
  projects: [
    {
      name: 'authorized',
      testMatch: /tests\/auth\/.*\.test\.js/,
      use: {
        storageState: 'storageState.json',
  	launchOptions: {
    	args: [
     	 '--font-render-hinting=none',     // Отключает хинтинг
     	 '--disable-skia-runtime-opts',    // Отключает оптимизации Skia
     	 '--disable-lcd-text',             // Отключает субпиксельный рендеринг
    	],
  },
      },
    },
    {
      name: 'guest',
      testIgnore: /tests\/auth\/.*\.test\.js/,
      use: {
    	args: [
     	 '--font-render-hinting=none',     // Отключает хинтинг
     	 '--disable-skia-runtime-opts',    // Отключает оптимизации Skia
     	 '--disable-lcd-text',             // Отключает субпиксельный рендеринг
    	],
      },
    }
  ],
});
