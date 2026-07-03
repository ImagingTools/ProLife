# DeskTicketApiPostman

Postman collection for full Desk/Ticket API testing of `imtdesk/1.0` against ProLife server.

## Contents

- `DeskTicketApi.postman_collection.json` - main collection (Postman v2.1)
- `ProLife-Dev.postman_environment.json` - environment template for ProLife dev server
- `data/entity_context_matrix.iteration.json` - data-driven input for matrix checks
- `Docs/ImtDesk_Endpoint_Catalog.md` - reverse-engineered endpoint and behavior catalog
- `Docs/Puma_TenantApiPostman_Analysis.md` - analysis of reference collection patterns and improvements

## Import

1. Open Postman.
2. Import collection file: `DeskTicketApi.postman_collection.json`.
3. Import environment file: `ProLife-Dev.postman_environment.json`.
4. Select environment `ProLife-Dev`.

## Required Environment Variables

Minimum:

- `baseUrl` (default: `http://localhost:7778/ProLife`)
- `productId` (default: `ProLife`)
- `suLogin`, `suPassword`

For role-based scenarios:

- `managerLogin`, `managerPassword`
- `agentLogin`, `agentPassword`
- `clientLogin`, `clientPassword`

Optional helpers:

- `entityType`, `entityId`, `entityDisplayName`
- `uploadedAttachmentId` (pre-uploaded file id in `uuid.ext` format)

## Run Order (recommended)

1. `Authentication` folder.
2. `Desk` folder.
3. `Ticket/Lifecycle` folder.
4. `Ticket/Comments`, `Ticket/Attachments`, `Ticket/Search & Filters`.
5. `Negative & Edge Cases`.
6. `Business Scenarios`.
7. `Data-driven` (with iteration file).

## Newman

Run full collection:

```powershell
newman run DeskTicketApi.postman_collection.json -e ProLife-Dev.postman_environment.json --reporters cli,json --reporter-json-export run-report.json
```

Run only business scenarios:

```powershell
newman run DeskTicketApi.postman_collection.json -e ProLife-Dev.postman_environment.json --folder "Business Scenarios"
```

Run data-driven matrix:

```powershell
newman run DeskTicketApi.postman_collection.json -e ProLife-Dev.postman_environment.json --folder "Data-driven" -d data/entity_context_matrix.iteration.json
```

## Coverage Summary

Covered API surface of `imtdesk/1.0`:

- `Authorization` (for role tokens)
- `TicketsList`
- `EntityContextTickets`
- `CreateEntityContextTicket`
- `GetTicketRepresentation`
- `UpdateTicketFromRepresentation`

Covered scenario groups:

- Positive paths for list/create/read/update
- Negative validation (empty required fields, invalid ids, invalid token, no token)
- Permission probes (insufficient rights)
- Edge input probes (unicode/special chars/long strings)
- Business flow (Client -> Manager -> Agent -> Client)
- Data consistency checks after chained operations
- Data-driven context matrix

## Notes and Limitations

- `imtdesk/1.0` currently exposes only GraphQL operations listed above; dedicated bulk endpoints and standalone Desk entities are not present in SDL.
- Attachments in comments require pre-uploaded file ids (`uploadedAttachmentId`), because upload endpoint belongs to file/chat services outside `imtdesk` namespace.
- HTTP status for GraphQL errors can still be 200; tests check `errors[]` and operation status fields.
- For strict role checks, use real dedicated users in ProLife environment and fill corresponding credentials.
