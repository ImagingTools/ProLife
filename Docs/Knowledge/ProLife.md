# ProLife: overview for the assistant

ProLife manages the life cycle of the hardware and software products a company builds and sells:
the customers and their orders, every built device, the software licenses issued for the devices
and the catalog both are made from. It is a server with a web client; users, groups, roles and
organizations come from Puma, support tickets from the desk service of ImtCore.

This page explains the areas and the words used in the user interface, the API and the
permissions. The other pages of this knowledge go into one area each: `Customers.md`, `Orders.md`,
`Devices.md`, `Licensing.md`, `HardwareCatalog.md`, `Analytics.md`, `Permissions.md`,
`Organizations.md`.

## Areas of the client

| Menu group | Page | What it holds |
|---|---|---|
| Workspace | Dashboard, User Actions, Analytics | Counters and charts (`Analytics.md`), the log of user actions |
| Sales | Customers | Companies: customers, and the manufacturers and suppliers of components |
| Sales | Orders | Orders of customers with their products (devices and licenses) |
| Equipment | Hardware | Every built device with its serial number, MAC address, production status and bound licenses |
| Equipment | Components, Configurations, Vendors, Hardware Models | Catalog of the hardware (`HardwareCatalog.md`) |
| Software Licensing | Issued Licenses | Software licenses issued to customers, bound to devices |
| Software Licensing | Software Products, License Types, Features | Catalog of the software |
| — | Organizations, Administration | Organizations, users, groups and roles (Puma) |
| — | Support | Tickets |
| — | Search | Search over the documents |

## Glossary

The same thing has different names in the client, the API and the permissions. Always map the
words of the user to the API names below before searching.

| In the client | In the API | In the permissions | Notes |
|---|---|---|---|
| Customer, Account, Company | `Account*` (`AccountsList`, `AccountItem`, `AccountAdd`, `AccountUpdate`) | `/AccountManagement/...` | Vendors are accounts with the role manufacturer or supplier |
| Order | `Order*` | `/OrderManagement/...` | "Delivery-ID" is `orderId`, "Purchase Order-ID" is `purchaseId` |
| Hardware, Device, Sensor | `Device*` (`DevicesList`, `DeviceItem`, `DeviceAdd`, `DeviceUpdate`) | `/SensorManagement/...` | "Sensor" is the old name; it is still used in permissions and messages |
| Issued License, Software, License instance | `SoftwareProduct*` (`SoftwareProductsList`, `SoftwareProductItem`, ...) | `/LicenseManagement/...` | One record is one issued license, possibly for several seats |
| Software Product (catalog) | `Product*` with `categoryId` `Software` | `/ProductManagement/...` | |
| Hardware Model | `Product*` with `categoryId` `Hardware` | `/ProductManagement/...` | The "device type" of a device |
| License Type | `License*` (`LicensesList`, `LicenseItem`) | `/LicenseDefinitionManagement/...` | A definition: which features of a product a license gives |
| Feature | `Feature*` | `/FeatureManagement/...` | |
| Component, Configuration | `HardwareComponent*`, `HardwareConfiguration*` | `/HardwareManagement/...` | |
| Binding, Bind/Unbind | `GetDeviceBinding`, `UpdateDeviceBinding` | `/SensorManagement/Bind`, `/Unbind` | Licenses bound to a device |
| Production status | `productionStatus` (`DeviceData`), `statusId` (`DeviceItem`) | `/SensorManagement/Edit/Change/ProductionStatus` | |
| Paired | `isPaired` | | A license bound to a device |
| In use | `inUse` | | A license for which a license file was created; a device with bound licenses |
| Internal use | `internalUse` | | Used by the own company; left out of the statistics |

## How the data hangs together

- An **account** is a company. A customer account orders; a manufacturer or supplier account appears
  in the hardware catalog.
- An **order** belongs to a customer (the ordering party) and lists products: devices and licenses.
  Adding a new product line to an order creates the device or license (`Orders.md`).
- A **device** is one built unit of a **hardware model** in one **configuration**. It has a serial
  number, a MAC address and a production status, and refers to its order.
- An **issued license** is a license of a **software product** of the catalog, of one **license type**,
  with an expiration date. It is bound to a device; a license file for the device can then be
  created (`Devices.md`, `Licensing.md`).
- A license for several seats can be split between customers; the parts form a tree (`Licensing.md`).
- The name of the customer and the order numbers shown in the lists of devices and licenses are read
  from the account and the order at every request, so a renamed customer appears everywhere at once.

## Rules for working with ProLife

- Find IDs with list queries first; never invent them. Most writing commands take the ID of the
  document (a UUID), not the number the user sees (serial number, Delivery-ID, MAC address).
- The single-item queries (`AccountItem`, `OrderItem`, `DeviceItem`, `SoftwareProductItem`) return
  the fields directly, without an `item` wrapper. The writing commands take `{"id": ..., "item": {...}}`.
- Updates replace the whole document: read it, change the fields, send all of them back. This matters
  most for orders (`Orders.md`) and bindings (`Devices.md`).
- Users see only what their permissions and groups allow (`Permissions.md`); a missing document may be
  hidden, not absent.
- Dates of licenses are written as `dd.MM.yyyy` in `SoftwareProductAdd`/`SoftwareProductUpdate`, but as
  `yyyy-MM-dd` in the product lines of an order.

## Typical tasks

| Task | How |
|---|---|
| "Which devices does customer X have, and which licenses are bound to them?" | Find the account (`AccountsList`), list the devices filtered by `CustomerId`, read the bindings (`GetDeviceBinding`) |
| "Which licenses of customer X expire this quarter?" | List the licenses of the customer and compare `expiration` (`Licensing.md`) |
| "Why can no license file be created for device Y?" | Read the device and its binding; check status `Finished`, MAC address, licenses of one product (`Devices.md`) |
| "Add two licenses of product P to order Z" | Read the order, append two new `Software` lines, send the whole order (`Orders.md`) |
| "Bind license L to device D" | Read the binding, append L, check the product of the licenses, update the binding (`Devices.md`) |
| "Give the reseller's customer 5 of its 20 seats" | `SplitLicense` with the account of the customer (`Licensing.md`) |
| "How many devices did we build per month this year?" | `GetHardwareCreationBarChart` with a time filter (`Analytics.md`) |
| "Which configurations use component C, which is end of life?" | Read the component and the configurations by `componentIds` (`HardwareCatalog.md`) |
| "Why does user U not see order O?" | Check his permissions and groups (`Permissions.md`) and the organization (`Organizations.md`) |
