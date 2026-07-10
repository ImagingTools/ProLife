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
//
// The actual login/retry/token-wait machinery is generic (imtcore-gui-testkit) - this file just wires
// it to ProLife's own fixtures/users.js and base URL.

const { createGlobalSetup } = require('imtcore-gui-testkit/globalSetup/createGlobalSetup');
const { activeUsers, authFile } = require('./fixtures/users');

module.exports = createGlobalSetup({
  activeUsers,
  authFile,
  rootDir: __dirname,
  baseUrl: process.env.PROLIFE_BASE_URL || 'http://localhost:17778',
});
