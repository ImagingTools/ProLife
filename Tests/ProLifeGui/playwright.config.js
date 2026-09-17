// ProLife's GUI suite config. Everything that is the same for every Imt-based app lives in the kit's
// createGuiConfig (per-user snapshot dirs, timeouts, workers, reporters, per-phase output dirs); this
// file only carries what is genuinely ProLife's.

const { createGuiConfig } = require('imtcore-gui-testkit/playwrightConfig/createConfig');
const users = require('./fixtures/users');

module.exports = createGuiConfig({
  rootDir: __dirname,
  baseUrl: process.env.PROLIFE_BASE_URL || 'http://localhost:17778',
  users,
  globalSetup: require.resolve('./global-setup.js'),
  // All projects share one Debug ProLife/Puma/Lisa server stack. Concurrent WASM boots can leave
  // dependent combo models permanently unloaded, so deterministic verification must be serial.
  workers: 1,

  // @mutating tests run serially against one shared database (Run-CiTests.ps1 phase 2, --workers=1), so
  // their wall clock is linear in the number of users and no worker count helps. Running the same save
  // again as a seventh user re-proves the flow, nothing more. Keep the two broad users (su for the admin
  // path, fullAccess for the real-granted-permission path); every isolatedSpec user keeps its own
  // mutating tests automatically, since its spec runs under that user and nobody else.
  mutatingUserKeys: ['su', 'fullAccess'],

  use: {
    // The PAT "copy token" button calls the browser clipboard API directly; Chromium denies
    // clipboard-write by default for automated contexts unless the permission is granted up front.
    permissions: ['clipboard-read', 'clipboard-write'],
  },
});
