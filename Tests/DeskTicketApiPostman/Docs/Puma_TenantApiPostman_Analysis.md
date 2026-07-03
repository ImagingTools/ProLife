# Analysis of Puma TenantApiPostman (reference)

Source analyzed:

- `D:/ImagingTools/Git/Puma/Tests/TenantApiPostman/README.md`
- `D:/ImagingTools/Git/Puma/Tests/TenantApiPostman/Tenant_System_Full.postman_collection.json`
- `D:/ImagingTools/Git/Puma/Tests/TenantApiPostman/Tenant_System_Full.postman_environment.json`
- `D:/ImagingTools/Git/Puma/Tests/TenantApiPostman/permission_visibility_matrix.iteration.json`

## Structural strengths

- Clear ordered foldering by domains and test phases (`00`, `01`, ... `99`).
- End-to-end chain with explicit lifecycle from bootstrap to cleanup.
- Stable naming convention:
  - business-first names
  - role/context markers in request names
  - explicit `(positive)` / `(negative)` markers
- Almost every request has a test script.
- Pre-request scripts used where dynamic preparation is needed.
- Heavy use of environment variables to pass ids/tokens between requests.
- Data-driven folder with iteration JSON matrix for role/visibility checks.
- Isolation and cross-tenant security tested in dedicated sections.

## Script and variable patterns observed

- `pm.environment.set/get` is the primary state bus.
- Dynamic ids/tokens captured from previous responses.
- Test scripts validate:
  - response shape
  - key domain invariants
  - role-based access behavior
- Data-driven requests read row values via `pm.iterationData`.

## Best practices worth carrying over

- Dedicated auth bootstrap folder before functional tests.
- Explicit negative/edge folders, not mixed implicitly.
- Scenario-driven chains for business realism.
- Role switching through token variables (not hardcoded static auth).
- Collection runner/Newman compatibility as first-class goal.

## Improvements introduced in DeskTicketApiPostman

- Added dedicated reverse-engineered endpoint catalog for imtdesk/1.0.
- Added explicit technical notes about GraphQL-200-with-errors behavior.
- Added stronger edge probes for unicode/special chars/long strings.
- Added business lifecycle chain for desk workflows with role transitions.
- Added explicit section documenting missing bulk APIs as schema-level gap.
- Added environment template tuned for ProLife server defaults.
