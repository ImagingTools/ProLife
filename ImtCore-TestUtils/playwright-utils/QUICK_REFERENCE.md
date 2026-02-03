# Quick Reference: @imtcore/playwright-utils

## Installation
```bash
npm install @imtcore/playwright-utils --save-dev
```

## Basic Usage
```javascript
const { reloadPage, clickAt, login, checkScreenshot } = require('@imtcore/playwright-utils');
```

## Quick Function Reference

### Click Functions
```javascript
await clickAt(page, x, y);                              // Click at coordinates
await clickOnElement(page, ["Menu", "Button"]);         // Click by objectName path
await clickOnPage(page, "Users");                       // Click menu page
await clickOnCommand(page, "Create");                   // Click command
await clickOnButton(page, ["Dialog", "Ok"]);            // Click button
```

### Form Functions
```javascript
await fillTextInput(page, "text", ["Dialog", "Input"]); // Fill text input
await selectComboBox(page, "Option", ["Form", "Combo"]); // Select from combo
```

### Navigation
```javascript
await reloadPage(page);                                  // Reload page
await reloadPage(page, "http://localhost:8080");        // Reload with URL
await waitForPageStability(page);                        // Wait for DOM stable
await delay(1000);                                       // Simple delay
```

### Screenshots
```javascript
await checkScreenshot(page, "test.png");                 // Simple screenshot
await checkScreenshot(page, "test.png", {                // With mask
  x: 100, y: 100, width: 200, height: 50
});
```

### Authentication
```javascript
await login(page, "username", "password");               // Default coordinates
await login(page, "user", "pass", {                      // Custom coordinates
  loginX: 700, loginY: 400,
  passwordX: 700, passwordY: 500,
  buttonX: 700, buttonY: 600
});
```

### Scroll
```javascript
await wheelScroll(page, 100);                            // Scroll down
await wheelScroll(page, -100);                           // Scroll up
```

## Global Setup
```javascript
// global-setup.js
const { createGlobalSetup } = require('@imtcore/playwright-utils');

module.exports = createGlobalSetup({
  baseURL: 'http://localhost:7778',
  username: 'su',
  password: '1',
  viewport: { width: 1400, height: 800 },
  storageStatePath: './storageState.json',
  loginCoordinates: { /* optional */ }
});
```

## Playwright Config
```javascript
// playwright.config.js
module.exports = defineConfig({
  globalSetup: require.resolve('./global-setup.js'),
  use: {
    viewport: { width: 1400, height: 800 },
    baseURL: 'http://localhost:7778',
  }
});
```

## Test Example
```javascript
const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, login } = require('@imtcore/playwright-utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
});

test('My test', async ({ page }) => {
  await login(page, 'user', 'pass');
  await clickOnPage(page, 'Dashboard');
  await checkScreenshot(page, 'dashboard.png');
});
```

## Documentation
- **Full Russian Docs**: `README.md`
- **English Docs**: `README_EN.md`
- **Migration Guide**: `../MIGRATION_GUIDE.md`
- **Instructions**: `../INSTRUCTIONS_FOR_IMTCORE.md`
