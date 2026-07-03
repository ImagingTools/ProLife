# imtdesk/1.0 Endpoint Catalog (Reverse-engineered)

Scope:

- SDL: `D:/ImagingTools/Git/ImtCore/Sdl/imtdesk/1.0/ImtDesk.sdl`
- SDL document service: `D:/ImagingTools/Git/ImtCore/Sdl/imtdesk/1.0/TicketCollectionDocumentService.sdl`
- GraphQL controllers and permissions:
  - `D:/ImagingTools/Git/ImtCore/Include/imtdeskgql/CEntityContextTicketsControllerComp.cpp`
  - `D:/ImagingTools/Git/ImtCore/Include/imtdeskgql/CTicketCollectionControllerComp.cpp`
  - `D:/ImagingTools/Git/ImtCore/Include/imtdeskgql/CTicketCollectionDocumentServiceComp.cpp`
  - `D:/ImagingTools/Git/ImtCore/Include/imtdeskgql/TicketPermissions.h`
- DB constraints:
  - `D:/ImagingTools/Git/ImtCore/Include/imtdeskdb/Resources/SQL/Postgres/CreateTicketsTable.sql`
  - `D:/ImagingTools/Git/ImtCore/Include/imtdeskdb/Resources/SQL/Postgres/CreateTicketEntityReferencesTable.sql`
  - `D:/ImagingTools/Git/ImtCore/Include/imtdeskdb/Resources/SQL/Postgres/CreateEntityReferencesTable.sql`

## Exposed GraphQL operations

## Query: TicketsList(input: TicketsListInput): TicketsListPayload!

Purpose:

- Returns ticket list (`TicketItemData[]`) with pagination via `viewParams`.

Input:

- `viewParams.offset` (optional)
- `viewParams.count` (optional)

Behavior notes:

- Non-admin visibility is filtered by user/group constraints (`GroupFilter`).
- Response includes `id`, `number`, `title`, `status`, `priority`, assignee display names, `createdAt`.

Expected error/edge outcomes:

- Invalid pagination values are clamped in code path, not fatal.
- Unauthorized tokens may return GraphQL errors (HTTP can remain 200).

## Query: EntityContextTickets(input: EntityContextTicketsInput): EntityContextTicketsPayload!

Purpose:

- Returns tickets linked to specific `(entityType, entityId)` context.

Required input:

- `entityType: String!`
- `entityId: ID!`

Optional input:

- `viewParams` (`offset`, `count`)

Behavior notes:

- Empty `entityType` or `entityId` -> controller error (`Entity context is empty`).
- Visibility filtering applies before context match.
- Offset/count are guarded: offset >= 0, count >= 1 (or default).

## Mutation: CreateEntityContextTicket(input: CreateEntityContextTicketInput): CreateEntityContextTicketPayload!

Purpose:

- Creates ticket linked to entity context and optionally creates conversation.

Required input:

- `entityType`
- `entityId`
- `title` (trimmed non-empty)

Optional:

- `entityDisplayName`
- `description`

Server defaults on create:

- `ticketType = SupportRequest`
- `status = Open`
- `stateReason = None`
- `priority = Medium`
- empty assignees
- `reporterId = current user`

Validation/business rules:

- Empty context/title -> rejected (`Entity context and title are required`).

## Query: GetTicketRepresentation(input: DocumentId): TicketData!

Purpose:

- Returns full editable ticket representation (document-service endpoint).

Includes:

- core fields (`title`, `description`, enums, assignees, reporter, lock fields, timestamps)
- `entityReferences`
- `entityTypes`
- `comments` with attachments and reply metadata
- computed `accessLevel`

Visibility rules:

- allowed: admin OR reporter OR assignee OR same-group as reporter
- denied: permission error returned

Access levels:

- `FullAccess`: admin/reporter
- `LimitedAccess`: assignee
- `CommentOnly`: same-group viewer
- `ViewOnly`: others

## Mutation: UpdateTicketFromRepresentation(input: UpdateTicketInput): DocumentOperationStatus!

Purpose:

- Updates ticket document and processes comment operations.

Input:

- `documentId` (required)
- `ticket` payload with mutable fields

Status values (`EDocumentOperationStatus`):

- `Success`
- `InvalidUserId`
- `InvalidDocumentId`
- `InvalidDocumentData`
- `Failed`

Permission rules:

- Edit allowed only for admin/reporter/assignee.
- Lock/unlock allowed only for admin/reporter.
- Core fields (`title`, `description`, assignees, type, priority) editable only by reporter/admin.

Comment behavior:

- New comment: item without `id`
- Edit comment: existing `id` + changed content/attachments
- Delete comment: `id` + `deleted=true`

Attachment behavior:

- Attachment ids in comments are expected as `uuid.ext` from upload endpoint.
- Server strips extension and stores pure UUID id.

## Subscriptions and notifications (integration points)

- `CTicketAssigneeNotifierComp`: notifies newly added assignees.
- `CTicketMessageNotifierComp`: notifies participants (reporter/assignees/admin) on new messages.
- Message/attachment domain is integrated via `imtchat` and attachment storage.

## Data model and DB constraints

Tickets table constraints (Postgres):

- `Id UUID PK`
- `Title TEXT NOT NULL`
- `Description TEXT NOT NULL DEFAULT ''`
- `ReporterId UUID NOT NULL`
- `Number SERIAL UNIQUE`
- indexed: status, priority, createdAt, number

Junction constraints:

- `TicketEntityReferences(TicketId, EntityReferenceId)` unique index

Implications for tests:

- duplicate relation rows should conflict at DB level
- reporter is mandatory
- title cannot be null

## Authorization and ProLife context

- Requests use `x-authentication-token` header.
- Token is obtained via `Authorization` query in ProLife GraphQL endpoint.
- Multitenant isolation in imtdesk itself is indirect (depends on authenticated user context and groups).

## Gaps relative to requested broad Desk/Ticket scope

Not exposed in current imtdesk/1.0 SDL:

- dedicated Desk CRUD entity endpoints
- explicit SLA endpoint set
- explicit escalation endpoint set
- explicit workflow transition endpoint set (transitions done via update fields)
- explicit audit log query endpoint
- explicit bulk endpoints

These gaps are covered in tests as negative schema probes where relevant.
