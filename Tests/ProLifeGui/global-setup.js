// Global setup: produce one storageState per fixture user by logging in - nothing gets CREATED here.
//
// The fixture roles/users (fixtures/users.js, one per spec file) and the "su" superuser are baked into
// Tests\ProLifeGui\puma.backup ahead of time (see Generate-Backups.ps1 + scripts/seed-fixture-users.js,
// which run fixtures/seed.js once against a live server and dump the result), and Run-CiTests.ps1
// restores that backup before every run. So global-setup here only needs to UI-login as each user in
// turn and save its storageState - no GraphQL RoleAdd/UserAdd calls at run time.
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
  // Run-CiTests.ps1 invokes "npx playwright test" twice per run (read-only phase, then @mutating
  // phase) against the SAME still-running server - set for the second invocation only, once the first
  // has already produced fresh storageState for every active user moments ago (see
  // createGlobalSetup's own comment on why this is trusted rather than verified here).
  reuseExistingAuth: process.env.PROLIFE_GUI_REUSE_AUTH === '1',
});
