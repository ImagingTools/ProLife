# Software licensing

ProLife keeps the catalog of the software (products, features, license types) and the licenses issued to
customers.

## Catalog

| Page | API | What it is |
|---|---|---|
| Software Products | `ProductsList`, `ProductItem` with `categoryId` `Software` | A software product that is licensed, with its features |
| License Types | `LicensesList`, `LicenseItem` | A license definition of a product: which `features` it gives; `parentLicenses` it extends |
| Features | `FeaturesList`, `GetFeatureItem` | Functions of the products that a license switches on; can have sub-features and requirements |

Hardware models are catalog products too, with `categoryId` `Hardware` (`HardwareCatalog.md`). The catalog can
also be imported from another database configured in the server settings. Changes of the catalog affect
every license issued from it; the assistant only reads it.

## Issued licenses

An issued license (page Issued Licenses, API `SoftwareProduct*`) is one license of a software product for a
customer.

| Field (`SoftwareProductData`) | Meaning |
|---|---|
| `serialNumber` | License number ("Software-ID"). Required, unique |
| `productId` | The software product of the catalog. Required; must be of the category `Software` |
| `licenseUuid` | The license type. Required |
| `expiration` | Expiration date as `dd.MM.yyyy`; empty or left out for unlimited. A value in another format is stored as unlimited |
| `orderUuid` | ID of the order; the license then belongs to the customer of the order |
| `customerId` | Customer of a license without order |
| `project` | Project, taken over from the binding to a device |
| `isMultiple`, `productCount` | A license for several seats and the number of seats still free (see below) |
| `internalUse` | License of the own company, left out of the statistics |
| `inUse` | A license file was created for its device; set by the server |
| `parentInstanceId`, `licenseTree`, `hasChildren`, `hasParent` | The tree of a split license; read only in practice |
| `productName`, `licenseName`, `orderName` | Read only |

The list (`SoftwareProductsList`, type `SoftwareProductItem`) adds `customerLink`, `deliveryIdLink`, `purchaseIdLink`,
`hardwareLink` (the device it is bound to), `isPaired` (bound to a device with a MAC address), `inUse`, `status`.

Writing: `SoftwareProductAdd` and `SoftwareProductUpdate` take `{"id": ..., "item": {...}}` and replace the license. Licenses
are usually created by an order (`Orders.md`). `isMultiple` and `productCount` cannot be changed for a license that has a
parent or children: an update keeps the stored values and the link to the parent.

## Life cycle of a license

1. Issued: created by an order or directly.
2. Paired: bound to a device (`Devices.md`); a seat is taken.
3. In use: a license file was created for the device. The license and the device can no longer be removed.
4. Unbound: back to issued, without project.
5. Expired: the expiration date has passed. The server does not change the license then. The lists cannot be filtered by
   the expiration on the server; to find expired or soon expiring licenses, read the list (narrowed by customer or product
   where possible) and compare the `expiration` of the items.

## Licenses for several seats: split and revoke

A license with `isMultiple` has `productCount` free seats. It can be divided between customers, for example from a
reseller to its end customers. The parts form a tree; `SoftwareProductItem` returns it as `licenseTree`, and
`ChildLicensesList` (`{"parentLicenseId": ...}`) lists the direct parts with their `availableCount` and `boundCount`.

`SplitLicense` (`{"licenseId", "licenseCount", "accountId", "targetLicenseId"?}`):

- only licenses with `isMultiple` and of the own organization can be split;
- `licenseCount` must be greater than 0 and at most the free seats of the license;
- without `targetLicenseId` a new part is created for the account, numbered `<license number>-1`, `-2`, ..., without order;
  with `targetLicenseId` the seats are added to that existing part, which must be a direct part of the license and belong to
  the same account;
- the seats are taken from the license.

`RevokeLicense` (`{"childLicenseId", "revokeCount"}`) gives seats of a part back to its parent:

- at most the seats of the part that are not bound to devices ("... exceeds available count ...; N licenses are bound to
  hardware");
- a part that has no seats left, nothing bound and no own parts is removed; otherwise it is kept.

Permissions: `/LicenseManagement/Edit/Split`, `/LicenseManagement/Edit/Revoke`. A split or a revoke cannot be rolled back by
the assistant; to undo a split, revoke the seats again.
