# PatTokenApiPostman

Postman/Newman collection dedicated to the **Personal Access Token (PAT)** feature (`Sdl\imtauth\1.0\PersonalAccessTokens.sdl`, implemented in ImtCore and enabled for ProLife via `PersonalAccessTokenSqlRepository` in `Partitura\ProLifeQmlVoce.arp\ProLifeServerBase.acc`): `CreateToken`, `GetToken`, `GetTokenList`, `ValidateToken`, `RevokeToken`, `DeleteToken`, plus `GetUserPermissions` (`Sdl\imtauth\1.0\Permissions.sdl`) used as a probe for PAT-authenticated effective permissions.

This is a new, standalone suite - it does **not** modify or duplicate `Tests\backend\postman_collection.json`'s existing `PersonalAccessTokens` folder (11 requests, happy-path only, git-tracked legacy collection). That folder is left untouched.

Built by analogy with `Tests\ProLifeApiPostman` and `Tests\DeskTicketApiPostman` (collection-level `apikey` auth header `x-authentication-token`, global pre-request/test scripts, numbered phase folders, bootstrap-creates-its-own-users rather than depending on pre-seeded accounts).

## Contents

- `PatTokenApi.postman_collection.json` - main collection (Postman v2.1), 92 requests across 12 folders
- `PatTokenApi-Dev.postman_environment.json` - environment template, defaults to `http://localhost:17778/ProLife`
- `Run-CiTests.ps1` - CI entry point: restores `puma_test` from a backup and starts `PumaServerPgTest.exe` (auth provider - `CreateSuperuser`/`Authorization`/`RoleAdd`/`UserAdd` proxy to it over HTTP), resets `prolife_test`, starts `ProLifeServerTest.exe` on port 17778, runs newman against the default (non-opt-in) folders, tears both servers down. Same isolated test server/DB as `ProLifeApiPostman`'s own CI script - starts and stops its own Puma instance, so it's safe to run standalone or back-to-back with `ProLifeApiPostman`/`DeskTicketApiPostman` in either order.
- `package.json` - pins the `newman` devDependency

## Import (manual/Postman GUI use)

1. Import `PatTokenApi.postman_collection.json`.
2. Import `PatTokenApi-Dev.postman_environment.json`.
3. Select environment `PatTokenApi-Dev`.
4. Run folders `00` through `08`, then `99`, in order (each folder depends on ids/tokens created by earlier ones). Skip `90`/`91` in a normal run (see "Opt-in folders" below).

## Required Environment Variables

- `baseUrl` (default `http://localhost:17778/ProLife`) / `productId` (`ProLife`)
- `suLogin` / `suPassword` - superuser credentials (default `su` / `1`)
- `cacheStalenessLongWaitEnabled` (default `false`) - see folder `91` below

Everything else (`tokenOwnerUserId`, `primaryTokenId`, `intersectionNarrowRawToken`, ...) is populated automatically by earlier requests via `pm.environment.set` - no other manual setup is required, and the suite assumes nothing beyond a working `su` login.

## Folder-by-folder coverage

| Folder | What it proves |
|---|---|
| `00 Bootstrap` | `su` login; creates two dedicated non-admin users from scratch: **Token Owner** (role permissions `ViewSensors;ViewOrders`) and **Other User** (role permissions `ViewAccounts`, deliberately disjoint) |
| `01 CreateToken - Validation and Success` | missing/empty `userId`/`name`/`scopes`, whitespace-only name (passes - known gap), malformed `expiresAt` (silently no-expiration), past `expiresAt` (immediately expired), canonical success (feeds folders `02`-`05`) |
| `02 GetToken and GetTokenList` | success shape incl. `tokenHash` exposure, missing/empty `id`/`userId`, not-found, unknown-user-returns-empty-list |
| `03 ValidateToken` | success, missing/empty `token`, garbage token, past-expiry token, malformed-expiry token |
| `04 RevokeToken` | success, `GetToken`/`ValidateToken` reflect revocation, idempotent double-revoke (see "Server bugs found" below), missing/empty/not-found `id`, immediate 403 on first PAT-auth use after revoke |
| `05 DeleteToken` | delete-after-revoke, not-found afterwards, idempotent double-delete / never-existed id (both succeed - see "Server bugs found" below), missing/empty `id` |
| `06 Scope ∩ Permissions` | **the centerpiece**: PAT effective permissions = user's real permissions ∩ token scopes, not a union - full match, narrower subset, over-broad scope (extra scope ignored), fully disjoint scope (zero effective permissions), and the `IsAdmin()` bypass (admin's PAT gets the full tree regardless of scope) |
| `07 Ownership and Cross-User Access` | source-confirmed: none of the six PAT resolvers check the caller's identity - any authenticated user can list/read/revoke/delete **and even create** PATs for any other `userId` |
| `08 Comma-in-Scope Storage Edge Case` | a scope value containing a literal comma corrupts round-tripping (`["ViewSensors,Extra","ViewOrders"]` reads back as 3 entries, not 2) - scopes are stored as one comma-joined SQL column with no escaping |
| `90` / `91` | opt-in, see below |
| `99 Cleanup` | best-effort teardown of every token/user/role created |

### The scope-intersection probe (folder `06`)

Testing "PAT effective permissions = user permissions ∩ scopes" against the superuser is meaningless - `CPermissionsProviderComp::OnGetUserPermissions` special-cases `IsAdmin()==true` and always returns the unfiltered permission tree, bypassing any scope restriction entirely (folder `06` includes one request proving exactly this, using `su` deliberately). All the real intersection scenarios instead run as **Token Owner**, a plain non-admin user created in `00 Bootstrap` with a small, known permission set (`ViewSensors;ViewOrders`).

The probe used to observe a PAT session's actual effective permissions is:
```graphql
query GetUserPermissions {
  GetUserPermissions(input: { productId: "ProLife" }) {
    groups { groupId groupName entries { permissionId displayName description } }
    errorMessage
  }
}
```
called **with the PAT itself as the `x-authentication-token` header**. This resolver reads `gqlContext.GetUserInfo().GetPermissions(productId)` - exactly the object `CAuthenticationManagerComp::CreateGqlContext` rewrites with the scope-intersected permission set for a PAT session. (The `Authorization.sdl` `GetPermissions` field was deliberately **not** used as a probe - it re-derives the user from a raw JWT decode and never touches the PAT-aware `gqlContext.GetUserInfo()`, so it can't observe the intersection at all; and no ProLife domain query has any field-level permission gate to probe against either.)

## Opt-in folders (never in the default Newman run)

- **`90 Exploratory - Omitted Input`**: the `input` argument is now non-null (`!`) in the SDL for all six PAT operations (required per GraphQL semantics and consistent with inner-field non-nulls). Omitting the entire `input` now fails request validation with the standard "Bad request. Unexpected request for command-ID: '...'" error before reaching any resolver (no Q_ASSERT). These 6 requests set `expectedStatus=any` and only log the observed response; they never hard-fail the run. Run manually: `newman run PatTokenApi.postman_collection.json -e PatTokenApi-Dev.postman_environment.json --folder "90 Exploratory - Omitted Input (informational, opt-in, never in default run)"`.
- **`91 Auth-Cache Staleness`**: `CAuthenticationManagerComp` caches a validated PAT's resolved user/scopes for up to `TokenCacheLifetime` seconds (default 300s), and this cache is **never invalidated** by `RevokeToken`/`DeleteToken`. Concretely: a PAT that was used for authentication (and thus cached) **before** being revoked can continue to authenticate ordinary GraphQL requests for up to 5 minutes after revocation - even though the dedicated `ValidateToken` query (folder `04`) reflects the revocation immediately, because it always calls the manager directly and never consults the cache. This folder's first four requests run fast and document the immediate-reuse case (still authenticates, as expected). Its fifth request is a real wait-gated check: run requests 1-4, wait **at least 300 seconds** wall-clock, then re-run request 5 alone (Postman GUI, or a second scoped `newman run ... --folder "91 ..."` invocation) with environment variable `cacheStalenessLongWaitEnabled=true` to actually assert the cache entry expired (403). This is intentionally excluded from CI - it is not worth a 5-minute stall on every pipeline run.

## Newman (default folders)

```powershell
newman run PatTokenApi.postman_collection.json -e PatTokenApi-Dev.postman_environment.json `
  --folder "00 Bootstrap" `
  --folder "01 CreateToken - Validation and Success" `
  --folder "02 GetToken and GetTokenList" `
  --folder "03 ValidateToken" `
  --folder "04 RevokeToken" `
  --folder "05 DeleteToken" `
  --folder "06 Scope Intersection - User Permissions ∩ Token Scopes" `
  --folder "07 Ownership and Cross-User Access (no enforcement, source-confirmed)" `
  --folder "08 Comma-in-Scope Storage Edge Case" `
  --folder "99 Cleanup" `
  --reporters cli,json --reporter-json-export run-report.json
```
(`Run-CiTests.ps1` runs this exact `--folder` list for you; pass `-IncludeOptInFolders` to add `90`/`91` too.)

## CI (`Run-CiTests.ps1`)

```powershell
powershell -ExecutionPolicy Bypass -File Run-CiTests.ps1
```

Starts a dedicated, isolated `ProLifeServerTest.exe` (database `prolife_test`, HTTP port `17778`) plus its own `PumaServerPgTest.exe` (database `puma_test`, HTTP port `17788`, restored from `Tests\ProLifeApiPostman\puma.backup`) - the same test servers `ProLifeApiPostman`'s CI script uses. Both are torn down at the end of the run, so this script no longer depends on (or leaves behind) any externally-managed Puma instance; run these as separate sequential CI steps rather than in parallel (port/DB reuse, not a hidden ordering dependency).

## Known Limitations

- GraphQL error shape: as with the other suites, HTTP status can be 200 even when a GraphQL-level error occurs for most requests. The PAT operations specifically report resolver-level errors (validation failures, not-found) as HTTP 200 with a top-level `errors[]` array and **no `data` key at all** - tests assert on `errors[0].message`/`path`/`extensions.type`, never on `data.<Field>.success` for these cases. Genuine auth/context failures (bad/expired JWT, invalid or revoked-and-uncached PAT) are real HTTP 401/403 with an empty body instead - tests for these assert on status code only, never call `pm.response.json()` unguarded.
- **Two distinct "invalid input" failure layers exist and are tested separately**: omitting a non-null (`!`) SDL field entirely (e.g. `CreateToken`'s `userId`/`name`, or the `id`/`token`/`userId` field of `GetToken`/`RevokeToken`/`DeleteToken`/`ValidateToken`/`GetTokenList`) fails typed-argument deserialization *before* the resolver ever runs, producing the generic `"Bad request. Unexpected request for command-ID: '<Command>'"` dispatcher message (source: `CGqlHandlerBaseClassGeneratorComp.cpp`-generated dispatch code, e.g. `CCreateTokenGqlRequest::IsValid()`/`ReadFromGraphQlObject` in the generated `PersonalAccessTokens.cpp`) - this is shared, generic behavior for *every* SDL command in the codebase with a required field, and matches real GraphQL non-null-argument semantics, not a bug. Sending the field as an empty string (`""`) instead reaches the resolver's own, friendlier `"Invalid request: X is required"` message. Both variants are tested; do not conflate them if a future SDL change makes a field nullable (would change which message a given "omit" test should expect).
- No ownership enforcement is a real, current, source-confirmed property of this API (folder `07`) - not a suite bug. If this is ever tightened (e.g. a `CommandPermissions` gate wired for these commands), folder `07`'s requests are expected to start failing and should be updated to assert the new, stricter behavior instead.
- The comma-in-scope corruption (folder `08`) is a genuine storage-layer property (comma-join on write, `split(',')` on read, no escaping) - not something this suite works around.
- Folder `91`'s long-wait check is skipped by default (`pm.test.skip`) unless `cacheStalenessLongWaitEnabled=true` is set and the requisite wait actually happened - it is not meant to run unattended in CI.
- Permission id strings used as scopes (`ViewSensors`, `ViewOrders`, `ViewAccounts`, `AddOrder`, ...) are real `FeatureId` values from `Impl\ProLifeServer\ProLifeFeatures.xml`, not arbitrary strings - if that catalog changes, folder `06`'s role/token setup should be revisited to keep using permissions the Token Owner role can plausibly hold.

## Server bugs found by this suite (fixed in ImtCore source, pending a rebuild)

Running this suite against a live server (a Debug `ProLifeServer.exe` built *before* the fixes below) surfaced two real, confirmed server-side bugs, now fixed at the source level in ImtCore:

1. **`RevokeToken` rejected re-revoking an already-revoked (or expired) token.** `CPersonalAccessTokenManagerComp::RevokeToken` (`Include\imtauth\CPersonalAccessTokenManagerComp.cpp`) checked `tokenPtr->IsValid()` (`!IsRevoked() && !IsExpired()`) as a *precondition* for revoking - but a token's very purpose in being revoked is to become invalid, so this made the operation fail with `"Invalid token object"` (surfaced as `"Failed to revoke token with id '<id>'"`) on any second revoke attempt, inconsistent with `DeleteToken`'s idempotent behavior (see below) and with what a revoke operation should do. Fixed by replacing the `IsValid()` check with a plain null-pointer guard (the check was also an unguarded null-dereference risk before the fix, since `dynamic_cast` can return `nullptr`).
2. **`DeleteToken` is (correctly) idempotent - our own initial test expectations were wrong, not the server.** `CPersonalAccessTokenManagerComp::DeleteToken` just forwards to the collection's generic `RemoveElements()`, which succeeds even for an id that never existed or was already deleted. This suite's `05 DeleteToken` folder now asserts `success:true` for double-delete and never-existed-id, matching this real, intentional behavior (this was previously an explicitly untested/commented-out case in `Tests\PersonalAccessTokenTest\CPersonalAccessTokenTest.cpp`).

**This means folders `04`/`05`/`06` need a server rebuilt from current ImtCore source to fully pass**: fixed via #1 above (`04`'s idempotent double-revoke), and via the PAT scope-intersection fix already made earlier to `CUserBaseInfo::GetPermissions`/`CAuthenticationManagerComp::CreateGqlContext` (`06`'s scope-intersection probes returned an empty permission set for every scenario against the pre-fix binary, even where the user's role genuinely grants the permission - confirmed via the `Authorization` login response itself reporting the correct `permissions` string, isolating the gap to the PAT auth-context path rather than role/permission resolution in general). Re-run the suite after rebuilding and restarting the server.
