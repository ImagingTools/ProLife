# Organizations and shared documents

Users can work in organizations (tenants) of Puma. The users, groups, roles and organizations are managed in Puma; ProLife
asks Puma for them. The general rules of the user administration are in the ImtCore knowledge (`Administration.md`).

## Working in an organization

- Accounts, orders, devices and licenses belong to the organization they were created in. In an organization a user sees
  its documents and the documents shared with it, nothing of other organizations.
- The owner of an organization holds the whole product in it. A member holds the permissions of his roles in that
  organization; an organization first allows features to its members, then gives them to roles.

## Relations and contracts

Organizations are related through a connection (a connection code and a request) and a contract that both sign. A contract
can also give permissions from one organization to another; the receiving organization passes them to one of its groups,
whose members then work in the giving organization with exactly these permissions ("delegated access"). When the contract
ends or a party leaves, the access ends with the next request.

## Sharing documents

An organization can share its accounts, devices and licenses with related organizations
(`/AccountManagement/Share`, `/SensorManagement/Share`, `/LicenseManagement/Share`):

- for viewing or for editing, optionally until an expiry date;
- only the owning organization shares, revokes and removes; the target can decline or accept;
- an organization that edits a shared document does not become its owner: it cannot share it further or remove it;
- a target sees who shared the document ("Shared by"), the owner sees all targets, a target never sees the other targets;
- the owner of the target organization can restrict a received document to one of its groups;
- when the relation ends, the shares stop granting access.

Operations of ProLife on shared documents:

- reading the binding of a shared device is allowed to a viewing target; binding, license files, transfers, transfer
  requests, split and revoke are reserved to the owner;
- an order can only refer to customers, devices and licenses of the own organization, also not to shared ones.

## For the assistant

- Answer from the organization the user works in; do not mix documents of several organizations.
- A document that the user cannot find may belong to another organization or be shared only with another group.
- Sharing, contracts and members are changed only in the client.
