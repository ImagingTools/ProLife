# ProLifeGui — GUI end-to-end tests (new architecture)

A clean, `objectName`-driven, page-object based Playwright suite for the ProLife Qt/QML web app, with
**first-class multi-user testing**: every spec runs once per test user, and screenshots are captured
per user, so "what each user sees" is recorded by construction.

The suite deliberately holds **no model of who may do what**. A test logs in, drives the UI and
compares screenshots; permissions are the server's business. Where a flow simply is not offered to the
current user, the test asks the running client - is the page in the menu, is the command button
visible - and skips. The client was built from that user's own permissions, so it is the only answer
that cannot drift out of date.

This is a **new, self-contained suite**. The legacy `Tests/frontend` (and the stale `Tests/GUI`) are
left untouched; they were used only as reference.

## Why this exists / what changed vs `Tests/frontend`

| Legacy (`Tests/frontend`) | This suite |
|---|---|
| Mostly `clickAt(page, x, y)` coordinates — brittle | `objectName` paths + page objects — layout-independent |
| `utils.js` `fillTextInput` silently no-ops on a missing field | Every action **hard-fails** if its target is missing/invisible/ambiguous |
| `waitForDomStability` diffs full `outerHTML` every 100 ms | `waitForStable` uses an in-page `MutationObserver` |
| One user (`su`), one `storageState.json` | One project **per user**, one `storageState` each, restored from a backup |
| Raw `objectName` arrays copy-pasted | `controls/` + `pages/` vocabulary |
| Screenshot-only, no structure guard | Screenshot-primary **plus** an honest action layer that hard-fails instead of no-opping |

## Layout

The framework itself lives in **`imtcore-gui-testkit`** (`ImtCore/Tests/GuiTestKit`), shared with the
other Imt-based apps: `lib/` (the gui barrel + dom, actions, stability, screenshot), `controls/`
(Button, CommandBar, MenuPanel, ComboBox, TextInput, FilterPanel, Table, Dialog), the generic
page-object bases, and the fixture / global-setup / config factories. It is consumed as a `file:`
dependency, which npm COPIES rather than symlinks, so `Run-CiTests.ps1` re-mirrors it before every run —
an edit to the kit is otherwise invisible here.

What lives in this folder:

```
fixtures/       users.js (source of truth) · seed.js (GraphQL role/user creation, used to BAKE fixture
                users into puma.backup - see Generate-Backups.ps1, not called at test-run time) ·
                test.js (thin wrapper over the kit's createGuiTest)
pages/          ProLife's own page objects: Workspace/Device/Software/Order/Account (collection +
                editor) · Support · index
tests/          *.collection / *.editor multiuser specs per domain · workspace · administration ·
                organizations · search · support · login.guest ; per-user baselines in
                tests/__screenshots__/<user>/
scripts/        prune-orphan-baselines.js (dead baselines) ·
                seed-fixture-users.js (one-off, used by Generate-Backups.ps1)
global-setup.js logs in as each ACTIVE fixture user (already baked into puma.backup) and mints one
                storageState each
playwright.config.js  one project per user (+ guest); snapshots keyed by {projectName}; workers: 10,
                with Run-CiTests.ps1 running a parallel read-only phase and a serial @mutating one.
                MUTATING_USER_KEYS keeps the serial phase from growing with the matrix.
```

## Multi-user model (the core idea)

`fixtures/users.js` is the single source of truth. Today it defines:

| key | role / permissions | expected to see |
|---|---|---|
| `su` | superuser (`*`) | everything |
| `fullAccess` | full View/Add/Edit/Change/Remove for Accounts/Sensors/Orders/Licenses + Workspace + ViewOrganizations + admin views | all pages (edits & saves everywhere) |
| `hardwareManager` | full Sensor rights (Bind/Transfer/Reset/CreateLicenseFile/Revision/all Change*) + ViewAccounts | Hardware + Accounts + Search |
| `licenseManager` | full License rights incl. `SplitLicense`/`RevokeLicense` command perms + ViewAccounts | Software + Accounts + Search |
| `orderManager` | full Order rights + ViewRevisions + ViewAccounts | Orders + Accounts + Search |
| `adminManager` | ViewUsers/Roles/Groups + Change/Edit/Add/Remove User/Role/Group | Administration + Search |
| `orgViewer` | `ViewOrganizations` only | Organizations + Search |
| `accountsViewer` | `ViewAccounts` only (read-only) | Accounts + Search |
| `noAccess` | (none) | Search only |
| `guest` | unauthenticated | login page |

The granular managers exist so each domain's **full command bar + editor save path runs for a
non-superuser** (real granted permissions, not `*`), and `orgViewer`/`adminManager` are the users that
make the Organizations / Administration pages appear. What each user actually sees is recorded by
their own `workspace-start` baseline, not asserted against a table.

`playwright.config.js` turns each into a **Playwright project** with its own `storageState`. A spec is
therefore run once per user, and `snapshotPathTemplate` writes baselines to
`tests/__screenshots__/<user>/<spec>/<name>-<platform>.png`. A spec that cannot be driven as the
current user skips on a runtime probe - `page.isAvailable()` (is it in the menu) or
`page.commands.isAvailable(id)` (is the button clickable) - so no test needs to know which permission
is behind either. Permission codes in `fixtures/users.js` exist only to SEED each fixture role; the
same role sets were validated in `Tests/ProLifeApiPostman` folder "08 Multi-role Scenario".

To compare users **inside one spec body** instead, use `forEachUser(users, fn)` from `fixtures/test.js`.

## Writing a test

```js
const { test } = require('../fixtures/test');
const { WorkspacePage } = require('../pages');

test('workspace start', async ({ page, gui }) => {
  const ws = new WorkspacePage(page);
  await ws.reload();
  test.skip(!(await ws.isAvailable()), 'Workspace is not available to this user');
  await ws.open();               // throws if the Workspace button is missing
  await gui.checkScreenshot(page, 'workspace-start');   // baseline is per-user automatically
});
```

Page objects hold **actions/locators only**; tests own the `checkScreenshot`/`expect*` assertions.

## Honest screenshots

Validation is screenshot-based, but the action layer (`lib/actions.js`) throws when a target
`objectName` is absent/invisible/ambiguous, so a screenshot can never be captured of a state reached
by a click that silently did nothing (the legacy `fillTextInput` bug). Nothing here asserts who is
allowed to do what: the server enforces that, and the per-user screenshots record the result.

## Running

Requires the ProLife WASM app served at `http://localhost:17778` with Puma (auth) + Lisa up, and a
**real, populated** ProLife/Puma DB - see [CI](#ci-run-citestsps1) below for exactly what that means and
why. `Run-CiTests.ps1` sets all of this up from scratch; running against a different environment means
reproducing the same restores yourself.

```bash
cd Tests/ProLifeGui
npm install

# First run on a platform: create baselines for every user
npx playwright test --update-snapshots

# Normal run
npx playwright test

# One user only
npx playwright test --project=accountsViewer

# List the test graph without a running app (parse/plumbing check)
npx playwright test --list
```

Baselines are per-platform (`-win32` / `-linux`), so mint them on the same OS the CI uses.

Two checks need neither a server nor a browser, and are worth running before any of the above:

```bash
# Baselines nothing can compare against any more: spec deleted, check renamed, or the project no
# longer runs that spec. --delete removes them.
node scripts/prune-orphan-baselines.js
```

## CI (`Run-CiTests.ps1`)

```powershell
powershell -ExecutionPolicy Bypass -File Run-CiTests.ps1
```

TeamCity-ready entry point, mirroring `Tests\ProLifeApiPostman\Run-CiTests.ps1` (same dependency
startup/teardown; swaps newman for Playwright). Starts, in order:

All three backups live right here in this folder (`Tests\ProLifeGui\*.backup`), so this suite doesn't
depend on `Tests\ProLifeApiPostman`'s copies at runtime - they're just plain copies of the same real
exports, re-copy them (or Generate-Backups.ps1's output, for puma.backup) if they go stale.

1. `PumaServerPgTest.exe` (database `puma_test`, HTTP port `17788`) — restored from **`puma.backup`
   right here in this folder**, not the plain one `ProLifeApiPostman` uses. It's a derived backup: the
   real ~725-user Puma export plus the 8 `fixtures/users.js` roles/users baked in on top (see
   `Generate-Backups.ps1` below). This is what lets `global-setup.js` just log in instead of creating
   anything at run time.
2. `LisaServerTest.exe` (database `lisa_test`, HTTP port `17776`) — restored from **`lisa.backup`
   right here in this folder** (a plain copy of `ProLifeApiPostman`'s real export).
3. `ProLifeServerTest.exe` (database `prolife_test`, HTTP port `17778`) — restored from
   **`prolife.backup` right here in this folder** (a plain copy of `ProLifeApiPostman`'s real export,
   with Devices/Orders/Accounts/SoftwareInstances data, incl. the exact catalog/type records
   `DeviceCollectionPage`/`SoftwareCollectionPage` tests filter/select against - an empty schema can
   never exercise those, no GraphQL mutation can create them). Its
   `PumaServer` FDW foreign-server definition then gets repointed at `puma_test` (`Repair-
   ProLifeForeignServers` - see the identical, more detailed note in `ProLifeApiPostman`'s README) before
   the server starts. This is the same executable that serves the WASM app Playwright drives.
   `New-SuperuserIfNeeded` (`CreateSuperuser`, `-SuPassword`) still runs as a safety net, but is expected
   to no-op with "Superuser already exists" since `puma.backup` already has one.

`npm install` and `npx playwright install chromium` run automatically if needed, then `npx playwright
test` runs with `CI=true` (switching `playwright.config.js` to the junit reporter, `junit-report.xml`)
and `PROLIFE_BASE_URL` pointed at the just-started `ProLifeServerTest.exe`. It invokes `npx playwright
test` **twice**: a read-only phase at the config's `workers` (10), then an `@mutating` phase at
`--workers=1`. The split exists because all three servers are one shared instance over one database -
mutations must not run against a collection another worker is screenshotting, and early attempts at
full parallelism produced real `"Authorization server connection error"` failures under load. The
serial phase is therefore the expensive one, which is what `MUTATING_USER_KEYS` in
`playwright.config.js` is there to bound. Teardown stops all three servers in reverse order. Puma/Lisa checkouts are located via the `PUMADIR`/`LISADIR` environment variables (falling back
to `Puma`/`Lisa` siblings of the ProLife checkout) — pass `-PumaRepoRoot`/`-LisaRepoRoot` explicitly if
your agent lays checkouts out differently.

### Regenerating `puma.backup` (`Generate-Backups.ps1`)

```powershell
powershell -ExecutionPolicy Bypass -File Generate-Backups.ps1
```

Re-run this whenever `fixtures/users.js` changes (new fixture user, renamed permission set, different
password, ...) to keep `puma.backup` in sync. It restores the plain `puma`/`lisa`/`prolife` backups from
`Tests\ProLifeApiPostman`, boots all three servers, bootstraps `su`, runs `scripts/seed-fixture-users.js`
(the exact same `fixtures/seed.js` logic `global-setup.js` used to call directly, before it was baked
into the backup) against the live server, then `pg_dump`s `puma_test` back out to
`Tests\ProLifeGui\puma.backup`. Only `puma_test` needs a ProLifeGui-specific derived backup:
`prolife_test`'s `Roles`/`Users`/`UserGroups`/`UserSessions` are `postgres_fdw` foreign tables that read
live from `puma_test` at query time, so the fixture users are visible from `prolife_test` automatically
with nothing to re-dump on the ProLife side - `Run-CiTests.ps1` restores `prolife_test`/`lisa_test` from
the plain, shared backups.

## Hardware (Devices) — full worked coverage

`WorkspacePage` and the **Hardware page** are fully worked. Hardware covers *every* function of the
collection and the multi-tab editor:

- `pages/DeviceCollectionPage.js` — commands (New/Edit/Remove/Revision/Bind/CreateLicenseFile/
  TransferLicenses/ResetTransferCounter/DecryptFile/Support), all registered filters, sorting,
  pagination, row selection; `tests/devices.collection.multiuser.test.js`.
- `pages/DeviceEditorPage.js` — every field (Device Type, Hardware Configuration, Article,
  Description, Serial Number, MAC Address, Order-ID, Production Status, Project, Internal Use),
  MAC validation, the Finished-status confirm dialog, group collapse/expand, Undo/Redo/Save,
  "save first" gating, close-dirty confirm; `tests/devices.editor.multiuser.test.js`. The header
  comment documents the Document Service flow (New/Edit → GetDeviceRepresentation →
  edit → UpdateDeviceFromRepresentation on Save).

Commands a given user cannot drive are skipped on `commands.isAvailable(id)` - the rendered button,
not a table - so the same spec runs unchanged under every user.

## QML instrumentation added for these tests

The project's own docs recommend `objectName` for stable tests. To make rows/pagination/editor
addressable, these **inert** `objectName`s were added (no runtime/visual effect):

- `ImtCore/Qml/imtcontrols/Views/TableRowDelegateBase.qml` — `objectName: "TableRow_<index>"` on each
  row + `"MouseArea"` on the row's mouse area. Makes **every collection row across the whole product**
  addressable by index (`Table.selectRow(i)`).
- `ImtCore/Qml/imtgui/View/Pagination.qml` — `"Pagination"`, `"PageSizeCombo"`, `"PrevPageButton"`,
  `"NextPageButton"` (page-number buttons were already auto-named `"<n>Button"`).
- `ProLife/Qml/Include/prolifeqml/DeviceEditor.qml` — a unique `objectName` on each field, group,
  group header, switch and the license table (17 total).

Filters, command-bar commands and table columns were **already** instrumented upstream
(`FilterDelegateBase` → `objectName: filterId`, `CommandsView` → `"<id>Button"`,
`TableHeaderDelegate` → `objectName: headerId`), so no change was needed there.

## Page coverage & instrumentation boundary

| Page | Coverage today | Depends on |
|---|---|---|
| Workspace | tabs, filters, collection cards | already instrumented |
| Hardware (Devices) | full collection + editor (commands, filters, sort, pagination, every field, dialogs) | `DeviceEditor` objectNames |
| Software / Orders / Accounts | full collection + editor | editor field objectNames |
| **Administration** | navigate + `AdministrationView` visible + per-user screenshot | root objectName only |
| **Organizations (Tenants)** | navigate + per-user screenshot | menu button only |
| **Search** | navigate + per-user screenshot | menu button only |
| **Guest / login** | login form shown, invalid login rejected, superuser sign-in reaches the menu | `LoginInput`/`PasswordInput`/`LoginButton` |

Administration / Organizations / Search are covered at the **navigation + screenshot** level because
`AdministrationView.qml`, `TenantCollectionView.qml` and `SearchPage.qml` are not yet
`objectName`-instrumented internally. To deepen them (command bars, sub-tabs, fields), add inert
`objectName`s the same way `DeviceEditor` got them, then extend the specs with the real interactions -
this is the remaining migration work.

The Support/Tickets page (`DeskPage`, `IsVisible=false`) is intentionally not part of the ProLife menu
and is not covered here.
