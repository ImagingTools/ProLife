# ProLifeApiPostman

Postman/Newman collection covering **every GraphQL field ProLife's own domain SDL exposes** (`Sdl\prolife\1.0\*.sdl`): Accounts, Orders, Sensors (Devices + IotDevices), Licenses (SoftwareProducts), Workspace dashboards, and DeviceCollectionDocumentService. One request/test per field, 47 fields total. Plus a multi-role permission-visibility scenario (folder `08`) covering `PagesData`, `GetCommands`, `GetProfile`, `GetSettings`/`SetSettings`/`GetStyleData`/`GetWebSocketUrl`, `Search`, `GetUserMode` and `ProductPermissions`.

Built by analogy with `Puma\Tests\TenantApiPostman` (collection structure, CI runner) and ProLife's own `Tests\DeskTicketApiPostman` (collection-level `apikey` auth, global pre-request/test scripts). Shared `imtauth` fields (`Authorization`, `CreateSuperuser`, `RoleAdd`, `UserAdd`) are used only as bootstrap/scenario-setup infrastructure here, never as their own dedicated coverage - Roles/Users/Groups are not domain fields being tested by this suite. The target database is assumed empty; folder `08` creates its own roles/users from scratch rather than depending on any pre-seeded accounts (other than `su`).

## Contents

- `ProLifeApi.postman_collection.json` - main collection (Postman v2.1)
- `ProLifeApi-Dev.postman_environment.json` - environment template, defaults to `http://localhost:7778/ProLife` (a normally-running dev `ProLifeServer`)
- `Run-CiTests.ps1` - CI entry point: resets `prolife_test`, starts `ProLifeServerTest.exe` on port 17778, runs newman, tears the server down (mirrors `Puma\Tests\TenantApiPostman\Run-CiTests.ps1`)
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
2. `01 Accounts` -> `02 Orders` -> `03 Devices` -> `04 IoT Devices` -> `05 Software Products - Licenses` -> `06 Device Document Service` -> `07 Workspace Dashboards` (read-only, run last so it sees seeded data).
3. `08 Multi-role Scenario - Pages, Commands, Profile` - creates 3 roles + 3 users from scratch (Full Access / Accounts Viewer / No Access, see below), switches the active `token` between them, and asserts `PagesData`/`GetCommands`/`GetProfile` visibility differs correctly per permission set. Re-authenticates as `su` at the end and covers the remaining app-level infra fields (`GetSettings`, `SetSettings`, `GetStyleData`, `GetWebSocketUrl`, `Search`, `GetUserMode`, `ProductPermissions`).
4. `99 Cleanup` - best-effort removal of everything created (including the roles/users from folder `08`); CI mainly relies on the full DB reset instead.

## Multi-role Scenario (folder `08`)

Permission codes are the exact `FeatureId` strings declared in `Impl\ProLifeServer\ProLifeFeatures.xml` and referenced by each page's `PagePermissions` list in `Partitura\ProLifeQmlVoce.arp\Pages.acc` / ImtCore's `PagesController.acc`. `RoleData.permissions` is a `;`-delimited string (confirmed against `Tests\backend\postman_collection.json`'s `RoleUpdate` example), not JSON or a GUID list.

| Actor | Granted permissions | Expected `PagesData` pages |
|---|---|---|
| Full Access | `ViewWorkspace;ViewAccounts;ViewSensors;ViewOrders;ViewLicenses;ViewUsers;ViewRoles;ViewGroups` | Accounts, Devices, Orders, SoftwareProducts, Administration (+ Search) |
| Accounts Viewer | `ViewAccounts` | Accounts, Search only |
| No Access | *(empty)* | Search only (universal `PagePermissions: ["*"]`) |

`GetCommands(input:{typeId:"Devices"})` is cross-checked only as a monotonic comparison (Full Access element count >= the other two), since the exact command-to-permission mapping isn't confirmed from any source read for this suite. `GetProfile`'s `permissions[].id` list is cross-checked against the same expectations as `PagesData` for each actor, as an independent verification path.

## Newman

```powershell
newman run ProLifeApi.postman_collection.json -e ProLifeApi-Dev.postman_environment.json --reporters cli,json --reporter-json-export run-report.json
```

## CI (`Run-CiTests.ps1`)

```powershell
powershell -ExecutionPolicy Bypass -File Run-CiTests.ps1
```

Starts a dedicated, isolated `ProLifeServerTest.exe` (see `Impl\ProLifeServerTest`, database `prolife_test`, HTTP port `17778`, WebSocket port `18778` - all `+10000` from ProLife's own production defaults, same convention Puma uses for `PumaServerPgTest`) so this suite never touches a real `prolife` database or collides with a running production `ProLifeServer`.

## Coverage

47 ProLife-domain GraphQL fields, one request/test each (see the collection's folder-per-SDL-file structure). A handful of fields (`IotDeviceAdd`/`Update`/`List`/`Item`, `ChildLicensesList`, `SplitLicense`, `RevokeLicense`, `GetIotDeviceRepresentation`, `UpdateIotDeviceFromRepresentation`) are **new coverage** - they aren't exercised by any prior ProLife test suite, so a failure there may be revealing a real gap rather than a suite bug.

## Known Limitations

- `DecryptLicenseFile` has no GraphQL-exposed way to obtain a real decryption key, so its test only asserts a well-formed response, not a full round-trip.
- `TransferLicenses`, `RequestTransferLicenses`, `SplitLicense`, `RevokeLicense` assert well-formed payloads rather than `ok: true`/success, since their success depends on license-tree/hardware state that isn't fully controllable from a clean bootstrap.
- `deviceType`/`productId`/`licenseUuid` values are pinned to specific pre-existing catalog records (see Environment Variables above) - if the target database is truly empty (no seed/reference data), `DeviceAdd`/`SoftwareProductAdd` and everything downstream of them will fail; adjust the environment variables to match real records in that database.
- The `IotDevices` document-service `collectionId` (used by `CreateNewDocument (IotDevice)`) is inferred by analogy with `Devices` and not confirmed against server source - adjust if it turns out to differ.
- GraphQL error shape: as with Puma/DeskTicketApiPostman, HTTP status can be 200 even when a GraphQL-level error occurs; tests check `errors[]`/payload status fields rather than relying on HTTP status alone.
- `AccountItem`/`OrderItem`/`DeviceItem`/`SoftwareProductItem`/`IotDeviceItem` (the collectionSchema `get` operations) require a `typeId` argument and wrap the response in an `item { ... }` field on the real server - this differs from what the raw pre-codegen `.sdl` source implies, and was only caught by cross-checking `Tests\backend\postman_collection.json`'s real `AccountItem`/`DeviceItem`/`RoleItem`/`GroupItem` examples. If any other `get`-style field in this collection still uses a flat, unwrapped selection, treat that as suspect and verify the same way before trusting a failure.
- `SetSettings`'s `settings` argument format (serialized `ParamsSet` string) isn't confirmed from any mined example - the test only checks for a well-formed response.
