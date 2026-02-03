# ProLife Frontend Tests

This directory contains Playwright-based end-to-end tests for the ProLife application frontend.

## 📚 Documentation

For comprehensive information about working with and creating Playwright tests, please refer to:

- **Russian**: [PLAYWRIGHT_TESTING_RU.md](./PLAYWRIGHT_TESTING_RU.md) - Полная документация по тестированию с Playwright
- **English**: [PLAYWRIGHT_TESTING_EN.md](./PLAYWRIGHT_TESTING_EN.md) - Complete Playwright testing documentation

## 🚀 Quick Start

```bash
# Install dependencies
npm install

# Run all tests
npx playwright test

# Run only authorized tests
npx playwright test --project=authorized

# Run only guest tests
npx playwright test --project=guest

# Run specific test file
npx playwright test tests/authorized/workspace.test.js

# Run with UI
npx playwright test --headed
```

## 📋 Test Structure

```
tests/
├── utils.js              # Shared helper functions
├── guest/                # Tests for unauthorized users
│   └── login.test.js
└── authorized/           # Tests for authorized users
    ├── workspace.test.js # Example: objectName-based testing
    ├── users.test.js
    ├── sensors.test.js
    ├── orders.test.js
    └── ...
```

## 🎯 Two Testing Approaches

### 1. Coordinate-based (Not Recommended)
```javascript
await clickAt(page, 700, 386); // Click at specific coordinates
```

### 2. ObjectName-based (✅ Recommended)
```javascript
await clickOnButton(page, ["SoftwareProductsInfo", "ViewAllButton"]);
await selectComboBox(page, "QUISS", ["CustomerFilterDelegate"]);
```

## 📖 Learn More

For detailed information including:
- Helper function reference
- Best practices
- Creating new tests
- Debugging tips
- And much more...

**Please see the full documentation:**
- [Russian Documentation (RU)](./PLAYWRIGHT_TESTING_RU.md)
- [English Documentation (EN)](./PLAYWRIGHT_TESTING_EN.md)

## 🔧 Configuration

- `playwright.config.js` - Playwright configuration
- `global-setup.js` - Global setup (authentication)
- `package.json` - Project dependencies
- `start.sh` - Full test execution script
