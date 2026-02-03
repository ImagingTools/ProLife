# @imtcore/playwright-utils

Common Playwright testing utilities for ImtCore projects.

## Installation

```bash
npm install @imtcore/playwright-utils --save-dev
```

## Usage

### Basic Example

```javascript
const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, login } = require('@imtcore/playwright-utils');

test('Example test', async ({ page }) => {
  await reloadPage(page);
  await login(page, 'username', 'password');
  await checkScreenshot(page, 'test-screenshot.png');
});
```

### Global Setup

To configure global state before running tests:

```javascript
// global-setup.js
const { createGlobalSetup } = require('@imtcore/playwright-utils');

module.exports = createGlobalSetup({
  baseURL: 'http://localhost:7778',
  username: 'su',
  password: '1',
  viewport: { width: 1400, height: 800 },
  storageStatePath: './storageState.json'
});
```

Then in `playwright.config.js`:

```javascript
module.exports = defineConfig({
  globalSetup: require.resolve('./global-setup.js'),
  // ... other settings
});
```

## API

### Page Interaction Utilities

#### `clickAt(page, x, y)`
Click at coordinates with page stability waiting.

#### `clickOnElement(page, path)`
Click on element found by objectName path.

#### `clickOnPage(page, pageId)`
Click on a menu page.

#### `clickOnCommand(page, commandId)`
Click on a command.

#### `clickOnButton(page, buttonPath)`
Click on a button by path.

### Form Utilities

#### `fillTextInput(page, text, path)`
Fill a text input field.

#### `selectComboBox(page, selectedText, path)`
Select a value from a dropdown.

### Navigation and Waiting

#### `reloadPage(page, url)`
Reload page with stability waiting.

#### `waitForPageStability(page, options)`
Wait for DOM stability.

#### `delay(time)`
Simple delay.

### Screenshots

#### `checkScreenshot(page, filename, maskParams)`
Create screenshot with optional mask.

### Authentication

#### `login(page, username, password)`
Login user.

### Scrolling

#### `wheelScroll(page, deltaY)`
Scroll with mouse wheel.

For detailed Russian documentation, see README.md.

## License

UNLICENSED - For internal ImagingTools use only
