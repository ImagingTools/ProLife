// Extended Playwright test with ProLife-aware fixtures.
//
//   test('...', async ({ page, user, gui }) => { ... })
//
// `user` is resolved from the current project name (= the user key, see playwright.config.js), so a
// single spec run under every user-project automatically knows *which* user it is running as and can
// adapt (e.g. skip mutating steps for read-only users) while its screenshots land in that user's
// per-project baseline directory.
//
// The actual fixture/newUserPage/forEachUser machinery is generic (imtcore-gui-testkit) - this file
// just wires it to ProLife's own fixtures/users.js.

const path = require('path');
const { createGuiTest } = require('imtcore-gui-testkit/fixtures/createTest');
const users = require('./users');

module.exports = createGuiTest(users, { rootDir: path.resolve(__dirname, '..') });
