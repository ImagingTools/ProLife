# Playwright Testing - Guide

## Table of Contents
- [Introduction](#introduction)
- [Project Structure](#project-structure)
- [Installation and Setup](#installation-and-setup)
- [Configuration](#configuration)
- [Two Approaches to Creating Tests](#two-approaches-to-creating-tests)
  - [1. Click by Coordinates](#1-click-by-coordinates)
  - [2. Using objectName in QML](#2-using-objectname-in-qml)
- [Helper Functions](#helper-functions)
- [Creating New Tests](#creating-new-tests)
- [Test Examples](#test-examples)
- [Running Tests](#running-tests)
- [Best Practices](#best-practices)

## Introduction

This project uses [Playwright](https://playwright.dev/) for automated UI testing. Playwright is a modern framework for testing web applications that supports multiple browsers and provides powerful automation tools.

## Project Structure

```
Tests/frontend/
├── tests/
│   ├── utils.js                    # Helper functions for tests
│   ├── guest/                      # Tests for unauthorized users
│   │   └── login.test.js
│   └── authorized/                 # Tests for authorized users
│       ├── workspace.test.js       # Example of objectName approach
│       ├── users.test.js
│       ├── sensors.test.js
│       ├── orders.test.js
│       └── ...
├── playwright.config.js            # Playwright configuration
├── global-setup.js                 # Global setup (authentication)
├── package.json                    # Project dependencies
└── start.sh                        # Script to run tests
```

## Installation and Setup

### Requirements
- Node.js (version 14 or higher)
- npm or yarn

### Installing Dependencies

```bash
cd Tests/frontend
npm install
```

This will install Playwright and all necessary dependencies.

## Configuration

### playwright.config.js

Main settings:

```javascript
module.exports = defineConfig({
  timeout: 0,                              // Disable global timeout
  testDir: './tests',                      // Directory with tests
  globalSetup: require.resolve('./global-setup.js'),
  use: {
    headless: true,                        // Run in headless mode
    viewport: { width: 1400, height: 800 }, // Browser window size
    screenshot: 'only-on-failure',          // Screenshots only on failures
    baseURL: 'http://localhost:7778',      // Application base URL
  },
  projects: [
    {
      name: 'authorized',                   // Tests for authorized users
      testMatch: /tests\/authorized\/.*\.test\.js/,
      use: {
        storageState: 'storageState.json',  // Use saved session
      },
    },
    {
      name: 'guest',                        // Tests for guests
      testIgnore: /tests\/authorized\/.*\.test\.js/,
    }
  ],
});
```

### global-setup.js

Performs automatic authentication before running tests for authorized users. Saves session state to `storageState.json`.

## Two Approaches to Creating Tests

The project uses two main approaches for interacting with UI elements:

### 1. Click by Coordinates

This approach uses fixed screen coordinates to click on elements.

**Advantages:**
- Simple and quick to create
- Doesn't require changes in QML code

**Disadvantages:**
- Brittle - breaks when layout changes
- Difficult to maintain
- Unreadable code

**Example:**

```javascript
const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot } = require('../utils');

test('Login test', async ({ page }) => {
  await reloadPage(page);
  
  await clickAt(page, 700, 386); // Click on "Login" field
  await page.keyboard.type('username');
  
  await clickAt(page, 685, 455); // Click on "Password" field
  await page.keyboard.type('password');
  
  await clickAt(page, 700, 600); // Click on "Login" button
  
  await checkScreenshot(page, 'login_success.png');
});
```

**clickAt Function:**

```javascript
const clickAt = async (page, x, y) => {
  await page.mouse.click(x, y);
  await waitForPageStability(page);
};
```

### 2. Using objectName in QML

This is the **recommended approach**. It uses the `objectName` attribute in QML elements for their identification.

**Advantages:**
- Resistant to layout changes
- Easy to read and maintain
- Clearly indicates test intent
- Not dependent on coordinates

**Disadvantages:**
- Requires adding `objectName` to QML code

**Example from workspace.test.js:**

```javascript
const { test } = require('@playwright/test');
const { reloadPage, checkScreenshot, clickOnPage, clickOnCommand, 
        selectComboBox, clickOnButton} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "Workspace"); // Click on Workspace page
});

test('User Actions', async ({ page }) => {
  await clickOnCommand(page, "UserActions"); // Click on UserActions tab
  await checkScreenshot(page, '1-workspace-user-actions.png')
});

test('Graphics Time Filter', async ({ page }) => {
  await clickOnButton(page, ["TimeFilterDelegate"]) // Click on date filter
  await clickOnButton(page, ["TimeFilterParamView", "Month_Current"]) // Click on "This Month"
  await checkScreenshot(page, '2-workspace-time-filter.png')
});

test('Customer Filter', async ({ page }) => {
  await selectComboBox(page, "QUISS", ["CustomerFilterDelegate"])
  await checkScreenshot(page, '3-workspace-customer-filter.png')
});
```

#### How It Works

**In QML code** add the `objectName` attribute:

```qml
Button {
    objectName: "CreateNewButton"  // Unique name for identification
    text: "Create New"
    onClicked: // ...
}

ComboBox {
    objectName: "CustomerFilterDelegate"
    // ...
}
```

**Path Structure:**

A path is an array of objectNames from parent to child element:

```javascript
// Simple path (one element)
clickOnButton(page, ["CreateNewButton"])

// Nested path (multiple elements)
clickOnButton(page, ["SoftwareProductsInfo", "ViewAllButton"])
// Searches for ViewAllButton inside SoftwareProductsInfo

// Deeply nested path
clickOnButton(page, ["FilterPanel", "CustomerFilter", "ClearButton"])
```

## Helper Functions

The `tests/utils.js` file contains a set of helper functions:

### Navigation and Basic Actions

#### `reloadPage(page, url)`
Reloads the page and waits for DOM stabilization.

```javascript
await reloadPage(page); // Uses baseURL
await reloadPage(page, 'http://localhost:7778/custom'); // Custom URL
```

#### `clickAt(page, x, y)`
Clicks at coordinates and waits for page stabilization.

```javascript
await clickAt(page, 700, 400);
```

#### `waitForPageStability(page, options)`
Waits until the DOM stabilizes (no changes for a certain time).

```javascript
await waitForPageStability(page, {
  maxTotalTime: 5000,     // Maximum wait time
  domStableTime: 800,     // Stability time
});
```

### Working with objectName Elements

#### `clickOnPage(page, pageId)`
Clicks on a page button in the navigation menu.

```javascript
await clickOnPage(page, "Workspace");    // MenuPanel -> WorkspaceButton
await clickOnPage(page, "Administration"); // MenuPanel -> AdministrationButton
```

**Implementation:**
```javascript
const clickOnPage = async (page, pageId) => {
  await clickOnElement(page, ["MenuPanel", pageId + "Button"])
}
```

#### `clickOnCommand(page, commandId)`
Clicks on a command in the command panel.

```javascript
await clickOnCommand(page, "UserActions"); // CommandsView -> UserActionsButton
await clickOnCommand(page, "Edit");        // CommandsView -> EditButton
await clickOnCommand(page, "Save");        // CommandsView -> SaveButton
```

**Implementation:**
```javascript
const clickOnCommand = async (page, commandId) => {
  await clickOnElement(page, ["CommandsView", commandId + "Button"])
}
```

#### `clickOnButton(page, buttonPath)`
Universal function for clicking a button via objectName path.

```javascript
await clickOnButton(page, ["TimeFilterDelegate"]);
await clickOnButton(page, ["SoftwareProductsInfo", "CreateNewButton"]);
await clickOnButton(page, ["FilterPanel", "SearchTextInput", "ClearButton"]);
```

#### `clickOnElement(page, path)`
Base function for clicking an element via objectName path. Finds MouseArea inside the element and clicks its center.

```javascript
await clickOnElement(page, ["MenuPanel", "WorkspaceButton"]);
```

**How it works:**
1. Creates CSS selector from path: `[objectName="MenuPanel"] [objectName="WorkspaceButton"]`
2. Finds MouseArea inside element: `[objectName="MouseArea"][visible]`
3. Calculates element center
4. Clicks at center coordinates

### Working with Forms

#### `fillTextInput(page, text, path)`
Fills a text field.

```javascript
await fillTextInput(page, "test@mail.ru", ["FilterPanel", "SearchTextInput"]);
await fillTextInput(page, "username", ["LoginForm", "UsernameInput"]);
```

#### `selectComboBox(page, selectedText, path)`
Selects an item from a dropdown list (ComboBox).

```javascript
await selectComboBox(page, "QUISS", ["CustomerFilterDelegate"]);
await selectComboBox(page, "None", ["FilterPanel", "SensorStatusFilter"]);
```

**How it works:**
1. Clicks on ComboBox
2. PopupMenuDialog opens
3. Clicks on the item with specified text in the dialog

### Validation

#### `checkScreenshot(page, filename, maskParams)`
Compares current page state with saved screenshot.

```javascript
await checkScreenshot(page, 'workspace-start.png');

// With mask (hides dynamic areas)
await checkScreenshot(page, 'with-mask.png', { 
  x: 580, y: 400, width: 100, height: 40 
});

// Mask by objectName path
await checkScreenshot(page, 'with-element-mask.png', { 
  path: ["ExpirationControl"],
  padding: 10  // Additional padding around element
});
```

**Mask Parameters:**
- `x, y, width, height` - mask coordinates and size
- `path` - path to element to mask
- `padding` - padding around element

### Additional Functions

#### `wheelScroll(page, deltaY)`
Scrolls the page with mouse wheel.

```javascript
await wheelScroll(page, 500);  // Scroll down
await wheelScroll(page, -500); // Scroll up
```

#### `login(page, username, password)`
Performs system login (uses coordinates).

```javascript
await login(page, "su", "password123");
```

#### `delay(time)`
Execution delay.

```javascript
await delay(1000); // Wait 1 second
await delay();     // Wait 5 seconds (default)
```

## Creating New Tests

### Step 1: Define Test Type

- **Guest tests** - in `tests/guest/` for unauthorized users
- **Authorized tests** - in `tests/authorized/` for authorized users

### Step 2: Create Test File

Create a file named `feature.test.js`:

```javascript
const { test } = require('@playwright/test');
const { reloadPage, checkScreenshot, clickOnPage, clickOnButton } = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "YourPage"); // Navigate to your page
});

test('Test description', async ({ page }) => {
  // Your test here
});
```

### Step 3: Add objectName in QML (recommended)

For stable tests, add `objectName` to elements in QML:

```qml
Item {
    objectName: "MyContainer"
    
    Button {
        objectName: "SubmitButton"
        text: "Submit"
        onClicked: // ...
    }
    
    ComboBox {
        objectName: "CategorySelector"
        // ...
    }
}
```

### Step 4: Write the Test

**Using objectName (recommended):**

```javascript
test('Create new item', async ({ page }) => {
  // Click New button
  await clickOnButton(page, ["CommandBar", "NewButton"]);
  
  // Fill form
  await fillTextInput(page, "Item Name", ["ItemForm", "NameInput"]);
  await selectComboBox(page, "Category A", ["ItemForm", "CategorySelector"]);
  
  // Save
  await clickOnButton(page, ["ItemForm", "SubmitButton"]);
  
  // Check result
  await checkScreenshot(page, 'item-created.png');
});
```

**Using coordinates (if objectName unavailable):**

```javascript
test('Coordinate-based test', async ({ page }) => {
  await clickAt(page, 100, 200); // Click button
  await page.keyboard.type('test text');
  await clickAt(page, 300, 400); // Click another element
  await checkScreenshot(page, 'result.png');
});
```

### Step 5: Run the Test

```bash
# Run specific test
npx playwright test tests/authorized/yourtest.test.js

# Run all tests
npm test
```

## Test Examples

### Example 1: Simple Navigation Test (objectName approach)

```javascript
test('Navigate to settings', async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "Settings");
  await checkScreenshot(page, 'settings-page.png');
});
```

### Example 2: Test with Filters

```javascript
test('Apply filters', async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "Sensors");
  
  // Text filter
  await fillTextInput(page, "test", ["FilterPanel", "SearchTextInput"]);
  
  // Date filter
  await clickOnButton(page, ["FilterPanel", "CreationDateFilter"]);
  await clickOnButton(page, ["TimeFilterParamView", "Month_Current"]);
  
  // ComboBox filter
  await selectComboBox(page, "Active", ["FilterPanel", "StatusFilter"]);
  
  await checkScreenshot(page, 'filtered-sensors.png');
});
```

### Example 3: Edit Test

```javascript
test('Edit user', async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "Administration");
  await clickOnButton(page, ["AdministrationView", "Users"]);
  
  // Select user (coordinates for table selection)
  await clickAt(page, 490, 215);
  
  // Open editor
  await clickOnCommand(page, "Edit");
  
  // Change email
  await fillTextInput(page, "newemail@example.com", ["UserEditor", "EmailInput"]);
  
  // Save
  await clickOnCommand(page, "Save");
  
  await checkScreenshot(page, 'user-updated.png');
});
```

### Example 4: Test with Mask (hiding dynamic elements)

```javascript
test('Test with masked element', async ({ page }) => {
  await reloadPage(page);
  
  // Mask by coordinates
  let mask = { x: 1015, y: 200, width: 150, height: 565 };
  await checkScreenshot(page, 'with-coordinate-mask.png', mask);
  
  // Mask by objectName path
  await checkScreenshot(page, 'with-path-mask.png', {
    path: ["DateTimeDisplay"],
    padding: 5
  });
});
```

## Running Tests

### Local Execution

```bash
cd Tests/frontend

# Install dependencies (once)
npm install

# Run all tests
npx playwright test

# Run tests for authorized users
npx playwright test --project=authorized

# Run tests for guests
npx playwright test --project=guest

# Run specific test
npx playwright test tests/authorized/workspace.test.js

# Run in headed mode (with UI)
npx playwright test --headed

# Run with debugging
npx playwright test --debug
```

### Using start.sh

The `start.sh` script sets up the environment and runs tests:

```bash
./start.sh
```

### Updating Screenshots

If the UI has changed and you need to update reference screenshots:

```bash
npx playwright test --update-snapshots
```

## Best Practices

### 1. Use objectName Instead of Coordinates

❌ **Bad:**
```javascript
await clickAt(page, 700, 386);
await clickAt(page, 685, 455);
```

✅ **Good:**
```javascript
await fillTextInput(page, "username", ["LoginForm", "UsernameInput"]);
await fillTextInput(page, "password", ["LoginForm", "PasswordInput"]);
await clickOnButton(page, ["LoginForm", "SubmitButton"]);
```

### 2. Use beforeEach for Common Setup

```javascript
test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "MyPage");
});
```

### 3. Add Meaningful Test Descriptions

❌ **Bad:**
```javascript
test('test1', async ({ page }) => { /* ... */ });
```

✅ **Good:**
```javascript
test('Should filter sensors by creation date', async ({ page }) => { /* ... */ });
```

### 4. Use Masks for Dynamic Elements

Elements with changing data (time, dates, random IDs) should be masked:

```javascript
await checkScreenshot(page, 'page.png', {
  path: ["DateTimeDisplay"],
  padding: 10
});
```

### 5. Group Related Tests

```javascript
test.describe('User management', () => {
  test.beforeEach(async ({ page }) => {
    await reloadPage(page);
    await clickOnPage(page, "Administration");
    await clickOnButton(page, ["AdministrationView", "Users"]);
  });

  test('Create user', async ({ page }) => { /* ... */ });
  test('Edit user', async ({ page }) => { /* ... */ });
  test('Delete user', async ({ page }) => { /* ... */ });
});
```

### 6. Use Helper Functions

For repetitive actions, create helper functions in the test:

```javascript
async function selectFirstRow(page) {
  await clickAt(page, 600, 245);
}

test('Test 1', async ({ page }) => {
  await selectFirstRow(page);
  // ...
});

test('Test 2', async ({ page }) => {
  await selectFirstRow(page);
  // ...
});
```

### 7. Wait for Page Stabilization

All functions from `utils.js` already wait for stabilization, but if you use Playwright API directly:

```javascript
await page.click('#button');
await waitForPageStability(page); // Important!
```

### 8. Naming objectName

When adding objectName in QML, use clear and unique names:

```qml
// Good examples
objectName: "CreateNewButton"
objectName: "UserFilterDelegate"
objectName: "SaveCommand"

// Avoid generic names
objectName: "Button"     // Too generic
objectName: "Item1"      // Unclear
```

### 9. Path Structure

Use logical hierarchy in paths:

```javascript
// Clear structure
["FilterPanel", "CustomerFilter", "ClearButton"]
["EditorForm", "PersonalInfo", "EmailInput"]

// Flat structure (use unique names)
["ClearCustomerFilterButton"]
```

### 10. Comments in Tests

Add comments for coordinate clicks and complex logic:

```javascript
await clickAt(page, 490, 215); // Select first user from table
await clickOnButton(page, ["UserEditor", "RolesTab"]); // Switch to roles tab
```

## Debugging Tests

### Viewing Tests in Browser

```bash
npx playwright test --headed --project=authorized
```

### Slow Execution

```bash
npx playwright test --headed --slow-mo=1000
```

### Pause and Debug

```javascript
test('Debug test', async ({ page }) => {
  await page.pause(); // Stop execution for debugging
  // ...
});
```

### Viewing Traces

```bash
# Run with tracing
npx playwright test --trace on

# View trace
npx playwright show-trace trace.zip
```

## Troubleshooting

### Test Fails Due to Timeout

Increase wait time in `waitForPageStability`:

```javascript
await waitForPageStability(page, {
  maxTotalTime: 10000,
  domStableTime: 1000,
});
```

### Element Not Found

1. Check that objectName is correctly specified in QML
2. Check the path to the element
3. Ensure the element is visible
4. Use `page.pause()` for debugging

### Screenshots Don't Match

1. Check for dynamic elements (time, dates)
2. Use masks for dynamic areas
3. Update reference screenshots: `npx playwright test --update-snapshots`

## Conclusion

Playwright provides a powerful and flexible way to automate UI testing. By following this guide and using the objectName approach, you can create reliable and maintainable tests.

**Key Points:**
- ✅ Use objectName instead of coordinates
- ✅ Use helper functions from utils.js
- ✅ Add meaningful descriptions to tests
- ✅ Mask dynamic elements in screenshots
- ✅ Follow existing patterns in the project

For more information, see:
- [Official Playwright Documentation](https://playwright.dev/)
- Examples in `tests/authorized/workspace.test.js`
- Helper functions source code in `tests/utils.js`
