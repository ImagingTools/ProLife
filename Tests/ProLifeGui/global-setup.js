// Global setup: produce one storageState per ACTIVE fixture user by logging in - nothing gets
// CREATED here.
//
// The 8 fixture roles/users (fixtures/users.js) and the "su" superuser are all baked into
// Tests\ProLifeGui\puma.backup ahead of time (see Generate-Backups.ps1 + scripts/seed-fixture-users.js,
// which run the exact same fixtures/seed.js logic once against a live server and dump the result), and
// Run-CiTests.ps1 restores that backup before every run. So global-setup here only needs to UI-login as
// each user in turn and save its storageState - no GraphQL RoleAdd/UserAdd calls at run time.
//
// Only logs in users fixtures/users.js's activeUsers() actually returns (su + fullAccess by default;
// every user when PROLIFE_GUI_ALL_USERS=1) - skipping the rest here, not just at the project level,
// is what makes the fast default subset actually fast (each login is a full page reload + UI flow).
//
// Regenerate Tests\ProLifeGui\puma.backup (via Generate-Backups.ps1) whenever fixtures/users.js changes.

const fs = require('fs');
const path = require('path');
const { chromium } = require('@playwright/test');

const { activeUsers, authFile } = require('./fixtures/users');
const { login } = require('./lib/actions');
const { waitForStable } = require('./lib/stability');

const BASE_URL = process.env.PROLIFE_BASE_URL || 'http://localhost:17778';
const VIEWPORT = { width: 1920, height: 1080 };

async function saveStateForUser(browser, user) {
  const context = await browser.newContext({ viewport: VIEWPORT });
  const page = await context.newPage();
  await page.goto(BASE_URL);
  await waitForStable(page, { timeout: 20000, quietMs: 600 });
  await login(page, user.login, user.password);

  // login() only waits for the DOM to go quiet for a bit after clicking "Sign in" - that can resolve
  // *before* the app has actually finished writing the authenticated session (accessToken etc.) to
  // localStorage, if there's a brief lull in DOM mutations while the login GraphQL call is still in
  // flight. Capturing storageState() at that point silently produces an empty/unauthenticated snapshot
  // (only the pre-login placeholder keys), which every test using it then runs against as if logged
  // out - confirmed empirically: this is what was producing "DevicesButton not found" etc. across the
  // whole suite, not a server or concurrency issue. Wait for the real signal (a non-empty accessToken)
  // before snapshotting.
  await page.waitForFunction(
    () => {
      try {
        const raw = localStorage.getItem('AuthorizationController/accessToken');
        return !!raw && JSON.parse(raw).length > 0;
      }
      catch {
        return false;
      }
    },
    undefined,
    { timeout: 30000 }
  );

  const dest = path.resolve(__dirname, authFile(user.key));
  fs.mkdirSync(path.dirname(dest), { recursive: true });
  await context.storageState({ path: dest });
  await context.close();
  return dest;
}

module.exports = async () => {
  const users = activeUsers();
  const su = users.find((u) => u.key === 'su');
  if (!su) throw new Error('global-setup: "su" must be in activeUsers() (check DEFAULT_USER_KEYS / PROLIFE_GUI_ALL_USERS)');

  const browser = await chromium.launch();
  try {
    await saveStateForUser(browser, su);
    for (const user of users) {
      if (user.key === 'su' || !user.seed) continue;
      await saveStateForUser(browser, user);
    }
  } finally {
    await browser.close();
  }
};
