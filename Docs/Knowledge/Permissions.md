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
| `AccountUpdate` | one of `/AccountManagement/Edit/Change/{Name, Description, Email, CompanyAddress, CustomerId, Groups}` |
| `OrdersList`, `OrderItem` | `/OrderManagement/View` |
| `OrderAdd` | `/OrderManagement/Edit/Add`; new product lines also `/SensorManagement/Edit/Add` or `/LicenseManagement/Edit/Add` |
| `OrderUpdate` | one of `/OrderManagement/Edit/Change/{DeliveryId, PurchaseOrderId, Description, Customer, Status, Products}` |
| `DevicesList`, `DeviceItem` | `/SensorManagement/View` |
| `DeviceAdd` | `/SensorManagement/Edit/Add` |
| `DeviceUpdate` | one of `/SensorManagement/Edit/Change/{SerialNumber, MacAddress, DeviceType, HardwareConfiguration, ProductionStatus, Order, Description, Project}` |
| `GetDeviceBinding`, `UpdateDeviceBinding` | `/SensorManagement/Bind` (adding), `/SensorManagement/Unbind` (removing) * |
| `CreateLicenseFile` | `/SensorManagement/CreateLicenseFile` * |
| `TransferLicenses` | `/SensorManagement/TransferLicenses` * |
| `ResetTransferCounter` | `/SensorManagement/ResetTransferCounter` |
| `SoftwareProductsList` | `/LicenseManagement/View` (or `/SensorManagement/Bind`/`Unbind`, to choose licenses for a device) |
| `SoftwareProductItem`, `ChildLicensesList` | `/LicenseManagement/View` |
| `SoftwareProductAdd` | `/LicenseManagement/Edit/Add` |
| `SoftwareProductUpdate` | one of `/LicenseManagement/Edit/Change/{Number, Expiration, ProductLicenses, Product, Order, Project}` |
| `SplitLicense`, `RevokeLicense` | `/LicenseManagement/Edit/Split`, `/LicenseManagement/Edit/Revoke` |
| `ProductsList`, `ProductItem`, `LicensesList`, `LicenseItem`, `FeaturesList`, `GetFeatureItem` | the `View` permission of any area |
| `HardwareComponentsList`, `GetHardwareComponentItem`, `HardwareConfigurationsList`, `GetHardwareConfigurationItem` | `/HardwareManagement/View` or `/SensorManagement/View` |
| `AddHardwareComponent`, `AddHardwareConfiguration` / `Update...` | `/HardwareManagement/Edit/Add` / `/HardwareManagement/Edit/Change` |
| `GetTotalSummaryInfo` | everyone |
| Charts of devices (`GetHardware...`) | `/SensorManagement/View` |
| Charts of licenses (`GetSoftware...`, `GetLicenseCreationInfo`) | `/LicenseManagement/View` |
| `GetOrderCreationLineChart` | `/OrderManagement/View` |

\* These permissions are applied by the commands of the client (the buttons of the page Hardware). The assistant
checks them itself before such an operation and refuses it without the permission.

## Permissions of single fields

The update commands accept a user who holds **one** of the field permissions of the area; which fields he may
change is applied by the editor of the client. The assistant must apply it the same way: before an update, compare
the old and the new document and change only fields whose permission the user holds (`Edit/Change/<field>`). If a
field the user wants to change needs a permission he lacks, say so instead of sending the update.

## Visibility by groups

Without the `ViewAll` permission of an area (for example `/SensorManagement/ViewAll`), a user sees only the documents
of the customers whose account names one of his user groups (`groups` of the account). Administrators and holders of
`ViewAll` see everything. A document the user asks for may therefore exist but be hidden from him.

## Organizations

In an organization, the owner holds the whole product; a member holds the permissions of his roles in that
organization. See `Organizations.md`.
