# Permissions in ProLife

Permissions are features of the product ProLife, granted through roles (directly or through groups) in Puma.
A permission is written as a path (`/SensorManagement/Edit/Change/SerialNumber`); a role that holds a path also
holds what the path requires. When the assistant is refused, it names the missing permission from the tables
below and does not try another way.

## Areas

| Area | Permissions | Page |
|---|---|---|
| Devices | `/SensorManagement/...` | Hardware |
| Orders | `/OrderManagement/...` | Orders |
| Issued licenses | `/LicenseManagement/...` | Issued Licenses |
| Accounts | `/AccountManagement/...` | Customers |
| Catalog | `/ProductManagement/...` (products and hardware models), `/LicenseDefinitionManagement/...` (license types), `/FeatureManagement/...` (features) | Software Products, Hardware Models, License Types, Features |
| Hardware catalog | `/HardwareManagement/...` | Components, Configurations, Vendors |
| Workspace | `/WorkspaceManagement/ViewPage`, `ViewUserActions`, `ViewAllUserActions`, `ViewAnalytics` | Dashboard, User Actions, Analytics |
| Search | `/SearchManagement/ViewPage` | Search |

Each of the areas Devices, Orders, Issued licenses and Accounts has the same structure:

| Permission | Allows |
|---|---|
| `ViewPage` | Opening the page |
| `View` | Reading the documents (lists and items) |
| `ViewAll` | Seeing the documents of all customers, not only those of the own groups (below) |
| `ViewHistory` | Reading the history of changes |
| `Edit/Add`, `Edit/Remove` | Creating, removing |
| `Edit/Change/<field>` | Changing one field (below) |
| `Share` | Sharing documents with other organizations (`Organizations.md`) |
| `Revisions/View`, `/Restore`, `/Delete` | Revisions of a document |

## Commands and the permissions they need

| Command | Permission |
|---|---|
| `AccountsList`, `AccountItem` | `/AccountManagement/View` (the list also with `/OrderManagement/Edit/Change/Customer`) |
| `AccountAdd` | `/AccountManagement/Edit/Add` |
| `AccountUpdate` | one of `/AccountManagement/Edit/Change/{Name, Description, Email, CompanyAddress, CustomerId, Groups}`; each changed field its own (below) |
| `OrdersList`, `OrderItem` | `/OrderManagement/View` |
| `OrderAdd` | `/OrderManagement/Edit/Add`; new product lines also `/SensorManagement/Edit/Add` or `/LicenseManagement/Edit/Add` |
| `OrderUpdate` | one of `/OrderManagement/Edit/Change/{DeliveryId, PurchaseOrderId, Description, Customer, Status, Products}`; each changed field its own (below) |
| `DevicesList`, `DeviceItem` | `/SensorManagement/View` |
| `DeviceAdd` | `/SensorManagement/Edit/Add` |
| `DeviceUpdate` | one of `/SensorManagement/Edit/Change/{SerialNumber, MacAddress, DeviceType, HardwareConfiguration, ProductionStatus, Order, Description, Project}`; each changed field its own (below) |
| `GetDeviceBinding` | `/SensorManagement/View` |
| `UpdateDeviceBinding` | `/SensorManagement/Bind` or `/SensorManagement/Unbind`; adding licenses needs `Bind`, removing them `Unbind` |
| `CreateLicenseFile` | `/SensorManagement/CreateLicenseFile` |
| `TransferLicenses`, `RequestTransferLicenses` | `/SensorManagement/TransferLicenses` |
| `DecryptLicenseFile` | `/SensorManagement/DecryptLicenseFile`, and only an administrator |
| `ResetTransferCounter` | `/SensorManagement/ResetTransferCounter` |
| `SoftwareProductsList` | `/LicenseManagement/View` (or `/SensorManagement/Bind`/`Unbind`, to choose licenses for a device) |
| `SoftwareProductItem`, `ChildLicensesList` | `/LicenseManagement/View` |
| `SoftwareProductAdd` | `/LicenseManagement/Edit/Add` |
| `SoftwareProductUpdate` | one of `/LicenseManagement/Edit/Change/{Number, Expiration, ProductLicenses, Product, Order, Project}`; each changed field its own (below) |
| `SplitLicense`, `RevokeLicense` | `/LicenseManagement/Edit/Split`, `/LicenseManagement/Edit/Revoke` |
| `ProductsList`, `ProductItem`, `LicensesList`, `LicenseItem`, `FeaturesList`, `GetFeatureItem` | the `View` permission of any area |
| `HardwareComponentsList`, `GetHardwareComponentItem`, `HardwareConfigurationsList`, `GetHardwareConfigurationItem` | `/HardwareManagement/View` or `/SensorManagement/View` |
| `AddHardwareComponent`, `AddHardwareConfiguration` / `Update...` | `/HardwareManagement/Edit/Add` / `/HardwareManagement/Edit/Change` |
| `GetTotalSummaryInfo` | everyone |
| Charts of devices (`GetHardware...`) | `/SensorManagement/View` |
| Charts of licenses (`GetSoftware...`, `GetLicenseCreationInfo`) | `/LicenseManagement/View` |
| `GetOrderCreationLineChart` | `/OrderManagement/View` |

## Permissions of single fields

The server compares the stored document with the new one and refuses an update that changes a field without its
permission (`Edit/Change/<field>`), with the error `No permission to change: <fields>`. Nothing is saved then. This
holds for the update commands, for `SetObjectData` and for restoring a revision. The fields and their permissions:

| Area | Field (permission) |
|---|---|
| Orders | `orderId` (`DeliveryId`), `purchaseId` (`PurchaseOrderId`), `description` (`Description`), `customerId` and `customerRoles` (`Customer`), `orderStatus` (`Status`), the product list `orderProducts` (`Products`) |
| Licenses | `serialNumber` (`Number`), `expiration` (`Expiration`), `licenseUuid`, `isMultiple`, `productCount` (`ProductLicenses`), `productId` (`Product`), `orderUuid` and the customer (`Order`), `project` (`Project`); `internalUse` needs none |
| Accounts | `name`, `website`, `isManufacturer`, `isSupplier` (`Name`), `description` (`Description`), `email` (`Email`), the address (`CompanyAddress`), `customerId` (`CustomerId`), `groups` (`Groups`) |
| Devices | `serialNumber`, `macAddress`, `deviceType`, `licenseName` (`HardwareConfiguration`), `productionStatus`, `orderId` (`Order`), `description`, `project` |

An update sends the whole document, so the assistant reads the document first and sends every field unchanged except
the ones the user wants to change. Before sending, it checks that the user holds the permission of each changed field
and says so instead of sending a request the server would refuse.

## Visibility by groups

Without the `ViewAll` permission of an area (for example `/SensorManagement/ViewAll`), a user sees only the documents
of the customers whose account names one of his user groups (`groups` of the account). Administrators and holders of
`ViewAll` see everything. A document the user asks for may therefore exist but be hidden from him.

## Organizations

In an organization, the owner holds the whole product; a member holds the permissions of his roles in that
organization. See `Organizations.md`.
