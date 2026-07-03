# ProLifeGui — GUI end-to-end tests (new architecture)

A clean, `objectName`-driven, page-object based Playwright suite for the ProLife Qt/QML web app, with
**first-class multi-user (permission) testing**: every spec runs once per test user, and screenshots
are captured per user, so "what each permission level sees" is validated by construction.

This is a **new, self-contained suite**. The legacy `Tests/frontend` (and the stale `Tests/GUI`) are
left untouched; they were used only as reference.

## Why this exists / what changed vs `Tests/frontend`

| Legacy (`Tests/frontend`) | This suite |
|---|---|
| Mostly `clickAt(page, x, y)` coordinates — brittle | `objectName` paths + page objects — layout-independent |
| `utils.js` `fillTextInput` silently no-ops on a missing field | Every action **hard-fails** if its target is missing/invisible/ambiguous |
| `waitForDomStability` diffs full `outerHTML` every 100 ms | `waitForStable` uses an in-page `MutationObserver` |
| One user (`su`), one `storageState.json` | One project **per user**, one `storageState` each, seeded via GraphQL |
| Raw `objectName` arrays copy-pasted | `controls/` + `pages/` vocabulary |
| Screenshot-only, no structure guard | Screenshot-primary **plus** an honest action layer + optional structural matrix assertions |

## Layout

```
lib/            gui.js (barrel = the "utils.js replacement") + dom, actions, stability, screenshot
fixtures/       users.js (source of truth) · seed.js (GraphQL role/user creation) · test.js (fixtures)
controls/       Button, CommandBar, MenuPanel, ComboBox, TextInput, FilterPanel, Table, Dialog
pages/          BasePage · CollectionPage · WorkspacePage (worked example) · index
matrix/         permissions.js — UI element → required permission codes (mirrors Pages.acc / ProLifeFeatures.xml)
tests/          workspace.multiuser.test.js (worked example) ; per-user baselines in tests/__screenshots__/<user>/
global-setup.js seeds users, then mints one storageState per user
playwright.config.js  one project per user (+ guest); snapshots keyed by {projectName}
```

## Multi-user model (the core idea)

`fixtures/users.js` is the single source of truth. Today it defines:

| key | role / permissions | expected to see |
|---|---|---|
| `su` | superuser (`*`) | everything |
| `fullAccess` | ViewWorkspace/Accounts/Sensors/Orders/Licenses/Users/Roles/Groups (+Add*) | all pages |
| `accountsViewer` | `ViewAccounts` only | Accounts + Search |
| `noAccess` | (none) | Search only |
| `guest` | unauthenticated | login page |

`playwright.config.js` turns each into a **Playwright project** with its own `storageState`. A spec is
therefore run once per user, and `snapshotPathTemplate` writes baselines to
`tests/__screenshots__/<user>/<spec>/<name>-<platform>.png`. The `user` fixture (resolved from the
project name) lets a test adapt — e.g. `test.skip(!user.can('AddOrder'))` — while its screenshots
land in that user's directory. Permission codes come from `ProLifeFeatures.xml`; the same role sets
were validated in `Tests/ProLifeApiPostman` folder "08 Multi-role Scenario".

To compare users **inside one spec body** instead, use `forEachUser(users, fn)` from `fixtures/test.js`.

## Writing a test

```js
const { test } = require('../fixtures/test');
const { WorkspacePage } = require('../pages');

test('workspace start', async ({ page, gui, user }) => {
  const ws = new WorkspacePage(page);
  await ws.reload();
  await ws.open();               // throws if the Workspace button is missing
  await gui.checkScreenshot(page, 'workspace-start');   // baseline is per-user automatically
});
```

Page objects hold **actions/locators only**; tests own the `checkScreenshot`/`expect*` assertions.

## Honest screenshots

Validation is screenshot-based, but the action layer (`lib/actions.js`) throws when a target
`objectName` is absent/invisible/ambiguous, so a screenshot can never be captured of a state reached
by a click that silently did nothing (the legacy `fillTextInput` bug). For exactness on
permission-sensitive UI, `matrix/permissions.js` + `menu.expectHasPage()/expectNoPage()` add a small
structural check (see `menu reflects permissions` in the example spec).

## Running

Requires the ProLife WASM app served at `http://localhost:17778` with Puma (auth) + Lisa up, and an
empty/known ProLife DB (only `su` needed — the rest is seeded).

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

The reference environment (`../frontend/start.sh` / `reset_db.sh`) restores the DBs from backups,
boots Puma/Lisa/ProLife, then runs Playwright — point it at this folder to run in CI. Baselines are
per-platform (`-win32` / `-linux`), so mint them on the same OS the CI uses.

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

Command/field permission gating lives in `matrix/permissions.js`
(`canRunDeviceCommand`, `canEditDeviceField`) and drives per-user `test.skip` + the structural
"command bar reflects permissions" / "editable fields reflect permissions" checks.

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

## Still to migrate

Customers / Orders / Licenses / Administration follow the Hardware pattern: subclass `CollectionPage`
(+ an editor page object where relevant), reuse the shared controls, add a `*.multiuser.test.js`.
Any editor whose fields lack `objectName` needs the same small instrumentation as `DeviceEditor`.
