# ProLifeApiPostman

Postman/Newman collection covering **every GraphQL field ProLife's own domain SDL exposes** (`Sdl\prolife\1.0\*.sdl`): Accounts, Orders, Sensors (Devices), Licenses (SoftwareProducts), Workspace dashboards, and DeviceCollectionDocumentService. One request/test per field, 43 fields total (IoT device fields were removed - see Known Limitations). Plus a multi-role permission-visibility scenario (folder `07`) covering `PagesData`, `GetCommands`, `GetProfile`, `GetSettings`/`SetSettings`/`GetStyleData`/`GetWebSocketUrl`, `Search`, `GetUserMode` and `GetProductPermissions`.

Built by analogy with `Puma\Tests\TenantApiPostman` (collection structure, CI runner) and ProLife's own `Tests\DeskTicketApiPostman` (collection-level `apikey` auth, global pre-request/test scripts). Shared `imtauth` fields (`Authorization`, `CreateSuperuser`, `RoleAdd`, `UserAdd`) are used only as bootstrap/scenario-setup infrastructure here, never as their own dedicated coverage - Roles/Users/Groups are not domain fields being tested by this suite. Folder `08` creates its own roles/users from scratch rather than depending on any pre-seeded accounts (other than `su`); the CI runner restores `prolife_test` from a real, populated dev-environment export rather than an empty schema (see `Run-CiTests.ps1` below for why), but this suite's own tests don't depend on any of that pre-existing data - they only need the catalog/type records it happens to bring along.

## Contents

- `ProLifeApi.postman_collection.json` - main collection (Postman v2.1)
- `ProLifeApi-Dev.postman_environment.json` - environment template, defaults to `http://localhost:7778/ProLife` (a normally-running dev `ProLifeServer`)
- `Run-CiTests.ps1` - CI entry point: starts `PumaServerPgTest.exe` and `LisaServerTest.exe` (restoring `puma_test`/`lisa_test` from real backups first), restores `prolife_test` the same way, starts `ProLifeServerTest.exe` on port 17778, runs newman, tears all three servers down in reverse order (mirrors `Puma\Tests\TenantApiPostman\Run-CiTests.ps1`)
- `prolife.backup` / `puma.backup` / `lisa.backup` - pg_dump custom-format backups (copies of `Tests\backend\backups\*.backup`, a real populated dev-environment export) restored into `prolife_test`/`puma_test`/`lisa_test` before every CI run. Not just Lisa needs this: `deviceTypeId`/`softwareCatalogProductId`/`licenseDefinitionId` (see Environment Variables below) reference catalog/type records that **no GraphQL mutation in this codebase can create** - an empty `prolife_test` can never make `DeviceAdd`/`SoftwareProductAdd` succeed. Restoring the real export also brings a working `su` account and hundreds of real Puma users/roles along for free, which is harmless here since this suite's own assertions fetch objects it just created by id rather than relying on exact list counts/positions.
- `package.json` - pins the `newman` devDependency

## Import (manual/Postman GUI use)

1. Import `ProLifeApi.postman_collection.json`.
2. Import `ProLifeApi-Dev.postman_environment.json`.
3. Select environment `ProLifeApi-Dev`.
4. Run folder `00 Bootstrap` first, then the rest in order (each folder mostly depends on ids created by the previous one).

## Required Environment Variables

- `baseUrl` (default `http://localhost:7778/ProLife`) / `productId` (`ProLife`)
- `suLogin` / `suPassword` - superuser credentials (default `su` / `1`)
- `deviceTypeId`, `softwareCatalogProductId`, `licenseDefinitionId` - these reference pre-existing catalog/type data (device type, software product catalog entry, license definition) that `DeviceAdd`/`SoftwareProductAdd` require. The defaults were mined from a real dev database export (`Tests\API\postman_collection.json`) - **replace them if the target database doesn't have matching records.**
- `decryptLicenseFileKey` - left blank by default; see the `DecryptLicenseFile` request's description for why.

Everything else (`accountId`, `orderId`, `deviceId`, `roleFullAccessId`, `fullAccessLogin`, ...) is populated automatically by earlier requests via `pm.environment.set`.

## Run Order

1. `00 Bootstrap` - ensures `su` exists and logs in (sets `token` used by every other request via collection-level auth).
2. `01 Accounts` -> `02 Orders` -> `03 Devices` -> `04 Software Products - Licenses` -> `05 Device Document Service` -> `06 Workspace Dashboards` (read-only, run last so it sees seeded data).
3. `07 Multi-role Scenario - Pages, Commands, Profile` - creates 3 roles + 3 users from scratch (Full Access / Accounts Viewer / No Access, see below), switches the active `token` between them, and asserts `PagesData`/`GetCommands`/`GetProfile` visibility differs correctly per permission set. Re-authenticates as `su` at the end and covers the remaining app-level infra fields (`GetSettings`, `SetSettings`, `GetStyleData`, `GetWebSocketUrl`, `Search`, `GetUserMode`, `GetProductPermissions`).
4. `99 Cleanup` - best-effort removal of everything created (including the roles/users from folder `07`); CI mainly relies on the full DB reset instead.

## Multi-role Scenario (folder `07`)

Permission codes are the exact `FeatureId` strings declared in `Impl\ProLifeServer\ProLifeFeatures.xml`/`.h` and referenced by each page's `PagePermissions` list. ProLife's own product-specific pages (Accounts, Devices, Orders, SoftwareProducts, Tenants) are wired in `Partitura\ProLifeQmlVoce.arp\Pages.acc`; the generic base pages (Workspace, Search, Tickets, Administration) come from ImtCore's `Partitura\ImtGraphQlVoce.arp\PagesController.acc`. `RoleData.permissions` is a `;`-delimited string (confirmed against `Tests\backend\postman_collection.json`'s `RoleUpdate` example), not JSON or a GUID list.

| Actor | Granted permissions | Expected & observed `PagesData` pages |
|---|---|---|
| Full Access | `ViewWorkspace;ViewAccounts;ViewSensors;ViewOrders;ViewLicenses;ViewUsers;ViewRoles;ViewGroups` | Workspace, Devices, Accounts, Orders, SoftwareProducts, Administration, Search, Tickets (everything except Tenants, which needs `ViewOrganizations`) |
| Accounts Viewer | `ViewAccounts` | Accounts, Search, Tickets |
| No Access | *(empty)* | Search, Tickets only (both are ungated: Search's `PagePermissions` is `["*"]`, Tickets/`DeskPage` has no `PagePermissions` at all) |

**Important - the `productid` request header:** `PagesData` (unlike `GetProfile`/`GetCommands`) carries no `productId` argument in its GraphQL schema, so the server reads the active product from a `productid` HTTP header instead. Server side, `CGqlRepresentationDataControllerComp::CreateContextParams` pulls `productId` from the request's input argument *or* the `productid` header; the page filter in `CGuiElementModelRepresentationControllerComp::GetRepresentationFromDataModel` then only calls `userInfoParamPtr->GetPermissions(productId)` when `productId` is non-empty. If the header is missing, `productId` is empty, `GetPermissions` is never called, resolved permissions come back empty, and **every permission-gated page is hidden** - leaving only the two ungated pages (Search, Tickets). This is correct server behaviour (the real GUI client always sends the header); the collection reproduces it by injecting `productid: {{productId}}` on every request from the collection-level pre-request script. An earlier draft of this README mis-attributed the resulting "only Search/Tickets" symptom to a server permission-propagation bug - that was wrong; it was purely a missing header in the test.

`GetCommands(input:{typeId:"Devices"})` is cross-checked only as a monotonic comparison (Full Access element count >= the other two), since the exact command-to-permission mapping isn't confirmed from any source read for this suite. `GetProfile`'s `permissions[].id` list is cross-checked against the same expectations for each actor, as an independent verification path - `GetProfile` correctly reports `ViewAccounts`/`ViewSensors` etc. for these actors.

## Newman

```powershell
newman run ProLifeApi.postman_collection.json -e ProLifeApi-Dev.postman_environment.json --reporters cli,json --reporter-json-export run-report.json
```

## CI (`Run-CiTests.ps1`)

```powershell
powershell -ExecutionPolicy Bypass -File Run-CiTests.ps1
```

Starts a dedicated, isolated `ProLifeServerTest.exe` (see `Impl\ProLifeServerTest`, database `prolife_test`, HTTP port `17778`, WebSocket port `18778` - all `+10000` from ProLife's own production defaults, same convention Puma uses for `PumaServerPgTest`) so this suite never touches a real `prolife` database or collides with a running production `ProLifeServer`.

`ProLifeServerTest` itself talks to Puma and Lisa over HTTP (`ProLifeServerTest.acc`'s `PumaHttpPort`/`LisaHttpPort`), so the script brings those up first, in order:

1. `PumaServerPgTest.exe` (database `puma_test`, HTTP port `17788`) - restored from `puma.backup`.
2. `LisaServerTest.exe` (database `lisa_test`, HTTP port `17776`) - restored from `lisa.backup`.
3. `ProLifeServerTest.exe` (database `prolife_test`, HTTP port `17778`) - restored from `prolife.backup`, then its `PumaServer` FDW foreign-server definition is repointed at `puma_test` (see below) before the server starts.

Teardown stops all three in reverse order. Puma/Lisa checkouts are located via the `PUMADIR`/`LISADIR` environment variables (falling back to `Puma`/`Lisa` siblings of the ProLife checkout) - pass `-PumaRepoRoot`/`-LisaRepoRoot` explicitly if your agent lays checkouts out differently.

### The `PumaServer` FDW foreign server

`prolife_test`'s `Roles`/`Users`/`UserGroups`/`UserSessions` tables are `postgres_fdw` **foreign tables**, all backed by a single foreign server literally named `PumaServer`. As restored from the dev export, that foreign server's `dbname` option is `puma` (the real, non-test database) - on a dev machine that also has a real local `puma` checkout, leaving this uncorrected means any query touching those tables would silently read (and, if the mapping allows, write) the developer's real data instead of `puma_test`. `Run-CiTests.ps1`'s `Repair-ProLifeForeignServers` runs `ALTER SERVER "PumaServer" OPTIONS (SET dbname 'puma_test')` immediately after every `prolife_test` restore to fix this. (There's also a dormant `setup_foreign_table_lisaserver()` helper function in the dump, but it's never invoked - no `LisaServer` foreign server actually exists, so nothing needs repointing for Lisa.)

## Coverage

43 ProLife-domain GraphQL fields, one request/test each (see the collection's folder-per-SDL-file structure). `ChildLicensesList`, `SplitLicense`, `RevokeLicense` are **new coverage** - they aren't exercised by any prior ProLife test suite, so a failure there may be revealing a real gap rather than a suite bug.

IoT device fields (`IotDeviceAdd`/`Update`/`List`/`Item`, `GetIotDeviceRepresentation`, `UpdateIotDeviceFromRepresentation`) were removed from this collection entirely at the user's request - they are out of scope for this suite.

## Current status

375/375 assertions pass.

## Known Limitations

- The multi-role `PagesData` checks depend on the `productid` request header being sent (see "Important - the `productid` request header" under the Multi-role Scenario section above). This is handled automatically by the collection-level pre-request script; if you copy an individual `PagesData` request out of this collection and run it standalone without that header, expect only `Search`/`Tickets` back. That is correct server behaviour, not a bug.
- Two investigation dead-ends worth recording so they aren't re-chased:
  - `GetSettings`/`GetStyleData`/`GetProfile`/`GetProductPermissions` at one point *appeared* entirely unwired ("Unexpected request for command-ID" errors). That was self-inflicted: ad-hoc diagnostic calls using empty `input: {}` where the schema requires specific fields (`GetSettings` needs `userId`, `GetStyleData` needs `schemeId`). The server returns a misleading "Unexpected request for command-ID" for a malformed/incomplete input rather than a proper validation error (a minor cosmetic server wart, not a functional gap). All four work correctly with a complete, schema-valid input, and the collection's own requests already call them correctly.
  - The "only Search/Tickets in `PagesData`" symptom was likewise mis-attributed at one point to a server permission-propagation bug. It was purely the missing `productid` header described above - a test defect, now fixed.
- The one *real* collection bug found during that investigation: the `ProductPermissions` request used a GraphQL field name (`ProductPermissions`) that does not exist in the live schema - the real field is `GetProductPermissions` (see `Sdl\imtauth\1.0\Permissions.sdl`). Fixed by renaming the request (and its query field + test-script references + description) throughout.
- `DecryptLicenseFile` has no GraphQL-exposed way to obtain a real decryption key, so its test only asserts a well-formed response, not a full round-trip.
- `TransferLicenses`, `RequestTransferLicenses`, `SplitLicense`, `RevokeLicense` assert well-formed payloads (data object OR a graceful `errors[]`) rather than requiring success, since their success depends on license-tree/hardware/transfer-count state that a clean bootstrap can't fully control - confirmed via raw response inspection that the errors returned (transfer limit reached, non-multiple license, no parent license) are genuine business-rule rejections, not malformed requests.
- `CreateLicenseFile`/`ResetTransferCounter` require `DPS_FINISHED` device status, a non-empty MAC address, and a real device-binding entry with non-empty `softwareIds` (see `CDeviceControllerComp.cpp`). The collection now creates a real software-product binding (`UpdateDeviceBinding` with `softwareIds: ["{{deviceBoundSoftwareProductId}}"]`) and runs these requests *before* `TransferLicenses` (which deliberately flips status to `DPS_DEFECTED` as a side effect - see `CDeviceControllerComp.cpp:451`), rather than after.
- `deviceType`/`productId`/`licenseUuid`/`deviceConfigurationId` values are pinned to specific pre-existing catalog records (see Environment Variables above) - `Run-CiTests.ps1` now restores `prolife_test` from a real export specifically so these exist; if you point `-ProLifeBackupPath` at a different/empty database, adjust the environment variables to match real records there instead. `licenseName` on `DeviceDataInput` is a misleadingly-named required field that actually maps to `ConfigurationType` server-side (`CDeviceCollectionControllerComp.cpp:696-708`) - it must be a real configuration-type UUID, not an empty string.
- GraphQL error shape: as with Puma/DeskTicketApiPostman, HTTP status can be 200 even when a GraphQL-level error occurs; tests check `errors[]`/payload status fields rather than relying on HTTP status alone.
- `AccountItem`/`OrderItem`/`DeviceItem`/`SoftwareProductItem` (the collectionSchema `get` operations) return flat fields directly (no `item { ... }` wrapper) on the live server, despite the collection's own former description claiming otherwise based on a legacy collection - confirmed against the live `.sdl` (`AddedNotificationPayload`/`UpdatedNotificationPayload` in `Sdl\imtbase\1.0\ImtCollection.sdl` only have `id`, no wrapper field either). Fixed throughout.
- `SetSettings`'s `settings` argument format (serialized `ParamsSet` string) isn't confirmed from any mined example - the test only checks for a well-formed response.
