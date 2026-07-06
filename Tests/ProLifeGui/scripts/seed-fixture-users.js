// One-off seeding script used to BAKE the ProLifeGui fixture users into a backup (see
// ..\Generate-Backups.ps1), rather than creating them at every CI run. Reuses the exact same
// fixtures/seed.js + fixtures/users.js logic global-setup.js used to call directly - only the
// transport differs: Playwright's `request.post()` API is replaced by a tiny fetch()-based shim with
// the same shape (`post(url, {headers, data}) -> { json() }`), since this runs standalone (plain
// `node`), not inside a Playwright worker.
//
// Usage: node scripts/seed-fixture-users.js [baseURL]
//   baseURL defaults to http://localhost:17778 (matches ProLifeServerTest.acc's HttpPort).
//
// Idempotent: safe to re-run against an already-seeded database (seed.js tolerates "already exists").

const path = require('path');
const { authorizeSu, seedUsers } = require(path.join(__dirname, '..', 'fixtures', 'seed'));
const { byKey } = require(path.join(__dirname, '..', 'fixtures', 'users'));

const request = {
  async post(url, opts) {
    const res = await fetch(url, { method: 'POST', headers: opts.headers, body: opts.data });
    return { json: () => res.json() };
  },
};

async function main() {
  const baseURL = process.argv[2] || 'http://localhost:17778';
  const su = byKey('su');

  console.log(`[seed-fixture-users] Authorizing as '${su.login}' against ${baseURL} ...`);
  const suToken = await authorizeSu(request, baseURL, su.login, su.password);
  console.log('[seed-fixture-users] Authorized. Seeding fixture roles/users ...');

  await seedUsers(request, baseURL, suToken);
  console.log('[seed-fixture-users] Done.');
}

main().catch((err) => {
  console.error('[seed-fixture-users] FAILED:', err);
  process.exit(1);
});
