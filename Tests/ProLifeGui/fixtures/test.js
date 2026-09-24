// Extended Playwright test with ProLife-aware fixtures.
//
//   test('...', async ({ page, user, gui }) => { ... })
//
// `user` is resolved from the current project name (= the user key, see playwright.config.js), and
// every spec file runs as exactly one user (fixtures/users.js), whose baselines live in
// tests/__screenshots__/<user>/.
//
// The fixture/newUserPage/forEachUser machinery is generic (imtcore-gui-testkit); this file only wires
// it to ProLife's own fixtures/users.js.

const path = require('path');
const { createGuiTest } = require('imtcore-gui-testkit/fixtures/createTest');
const users = require('./users');

module.exports = createGuiTest(users, { rootDir: path.resolve(__dirname, '..') });
