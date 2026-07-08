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

// True once the app has written a real, non-empty accessToken to localStorage. login() only waits for
// the DOM to go quiet after clicking "Sign in", which can resolve *before* the auth round-trip has
// stored the session - snapshotting storageState then yields an empty/unauthenticated state (only the
// pre-login placeholder keys), and every test using it then runs as if logged out (this was the
// "DevicesButton not found" cause, not a server/concurrency issue). Waiting for this signal fixes it.
const TOKEN_READY = () => {
  try {
    const raw = localStorage.getItem('AuthorizationController/accessToken');
    return !!raw && JSON.parse(raw).length > 0;
  }
  catch {
    return false;
  }
};

async function loginAndWaitForToken(context, user) {
  const page = await context.newPage();
  await page.goto(BASE_URL);
  await waitForStable(page, { timeout: 20000, quietMs: 600 });
  await login(page, user.login, user.password);
  await page.waitForFunction(TOKEN_READY, undefined, { timeout: 20000 });
  await page.close();
}

async function saveStateForUser(browser, user) {
  const context = await browser.newContext({ viewport: VIEWPORT });

  // The QML/WASM login click is occasionally lost (a canvas click that lands mid-repaint never fires),
  // leaving no accessToken. That is a genuine flaky-input reality of driving a WASM canvas, so retry
  // the whole login a couple of times rather than failing the entire suite on one dropped click.
  const MAX_ATTEMPTS = 3;
  let lastErr;
  for (let attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
    try {
      await loginAndWaitForToken(context, user);
      lastErr = null;
      break;
    }
    catch (err) {
      lastErr = err;
      // eslint-disable-next-line no-console
      console.warn(`global-setup: login attempt ${attempt}/${MAX_ATTEMPTS} for "${user.key}" did not produce a session (${err.message}); retrying`);
    }
  }
  if (lastErr) {
    await context.close();
    throw lastErr;
  }

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
