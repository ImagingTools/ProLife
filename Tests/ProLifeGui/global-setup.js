// Global setup: seed the multi-user permission fixtures, then produce one storageState per user.
//
// Steps:
//   1. UI-login as the pre-existing superuser -> .auth/su.json
//   2. Using a superuser GraphQL token, create the seeded roles + users (fixtures/seed.js)
//   3. UI-login as each seeded user -> .auth/<key>.json
//
// Everything is self-contained: an empty ProLife DB (only `su` present) is enough; the roles/users
// are created here rather than assumed to exist in a backup.

const fs = require('fs');
const path = require('path');
const { chromium, request: pwRequest } = require('@playwright/test');

const { USERS, seededUsers, authFile } = require('./fixtures/users');
const { seedUsers, authorizeSu } = require('./fixtures/seed');
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
  const dest = path.resolve(__dirname, authFile(user.key));
  fs.mkdirSync(path.dirname(dest), { recursive: true });
  await context.storageState({ path: dest });
  await context.close();
  return dest;
}

module.exports = async () => {
  const su = USERS.find((u) => u.key === 'su');
  if (!su) throw new Error('global-setup: no "su" user defined in fixtures/users.js');

  const browser = await chromium.launch();
  try {
    // 1. Superuser session (used as the login baseline and as the seeding identity).
    await saveStateForUser(browser, su);

    // 2. Seed roles + users via GraphQL, authenticated as su.
    const api = await pwRequest.newContext();
    try {
      const suToken = await authorizeSu(api, BASE_URL, su.login, su.password);
      await seedUsers(api, BASE_URL, suToken);
    } finally {
      await api.dispose();
    }

    // 3. One storageState per seeded user.
    for (const user of seededUsers()) {
      await saveStateForUser(browser, user);
    }
  } finally {
    await browser.close();
  }
};
