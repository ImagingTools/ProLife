// Extended Playwright test with ProLife-aware fixtures.
//
//   test('...', async ({ page, user, gui }) => { ... })
//
// `user` is resolved from the current project name (= the user key, see playwright.config.js), so a
// single spec run under every user-project automatically knows *which* user it is running as and can
// adapt (e.g. skip mutating steps for read-only users) while its screenshots land in that user's
// per-project baseline directory.

const base = require('@playwright/test');
const { byKey, can } = require('./users');
const gui = require('../lib/gui');

const test = base.test.extend({
  // The current test user, derived from the Playwright project name.
  user: async ({}, use, testInfo) => {
    const user = byKey(testInfo.project.name);
    if (!user) {
      throw new Error(
        `No test user maps to project "${testInfo.project.name}". Project names must match fixtures/users.js keys.`
      );
    }
    // Attach a convenience predicate.
    const decorated = { ...user, can: (perm) => can(user, perm) };
    await use(decorated);
  },

  // The GUI helper barrel, handed to tests so they don't each require it.
  gui: async ({}, use) => {
    await use(gui);
  },
});

const expect = base.expect;

/**
 * Run the same body for several users inside ONE spec, for side-by-side permission comparisons.
 * Prefer the per-project model (one project per user) for the common case; use this only when a test
 * genuinely needs to compare multiple users in a single body.
 *
 * @param {import('./users').TestUser[]} users
 * @param {(user: import('./users').TestUser) => void} defineTests
 */
function forEachUser(users, defineTests) {
  for (const user of users) {
    test.describe(`[user: ${user.title}]`, () => defineTests(user));
  }
}

module.exports = { test, expect, forEachUser, gui };
