const { chromium } = require('@playwright/test');

/**
 * Creates a global setup function for Playwright tests
 * @param {Object} options - Configuration options
 * @param {string} options.baseURL - Base URL for the application (default: 'http://localhost:7778')
 * @param {string} options.username - Username for login (default: 'su')
 * @param {string} options.password - Password for login (default: '1')
 * @param {Object} options.viewport - Viewport size (default: { width: 1400, height: 800 })
 * @param {string} options.storageStatePath - Path to save storage state (default: './storageState.json')
 * @returns {Function} Global setup function
 */
function createGlobalSetup(options = {}) {
  const {
    baseURL = 'http://localhost:7778',
    username = 'su',
    password = '1',
    viewport = { width: 1400, height: 800 },
    storageStatePath = './storageState.json'
  } = options;

  return async () => {
    const { waitForPageStability, login } = require('./utils');
    
    const browser = await chromium.launch();

    const context = await browser.newContext({
      viewport,
    });

    const page = await context.newPage();

    await page.goto(baseURL);
    await waitForPageStability(page);

    await login(page, username, password);

    await waitForPageStability(page);

    // Сохраняем состояние
    await context.storageState({ path: storageStatePath });

    await browser.close();
  };
}

module.exports = createGlobalSetup;
