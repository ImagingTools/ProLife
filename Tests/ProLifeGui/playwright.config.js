// ProLife's GUI suite config. Everything that is the same for every Imt-based app lives in the kit's
// createGuiConfig (per-user snapshot dirs, timeouts, reporters, per-phase output dirs, projects); this
// file only carries what is genuinely ProLife's.

const { createGuiConfig } = require('imtcore-gui-testkit/playwrightConfig/createConfig');
const users = require('./fixtures/users');

module.exports = createGuiConfig({
  rootDir: __dirname,
  // ProLifeServerTest.acc's HttpPort.
  baseUrl: process.env.PROLIFE_BASE_URL || 'http://localhost:17778',
  users,
  globalSetup: require.resolve('./global-setup.js'),
  mutatingUserKeys: ['su'],
  // Double the kit default. A cold Qt/WASM boot is ~12s on its own, and checkScreenshot still wants up
  // to 20s of DOM quiet after the page settles; with several workers booting at once the kit's 60s is
  // not enough headroom. A ceiling, not a delay - a fast test is unaffected.
  timeout: 120_000,

  use: {
    // The PAT "copy token" button calls the browser clipboard API directly; Chromium denies
    // clipboard-write by default for automated contexts unless the permission is granted up front.
    permissions: ['clipboard-read', 'clipboard-write'],
  },
});
