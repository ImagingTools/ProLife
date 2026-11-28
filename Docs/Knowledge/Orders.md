# Orders

An order belongs to a customer and lists the products delivered with it: devices (hardware) and
issued licenses (software).

## Fields

| Field | Meaning |
|---|---|
| `orderId` | **Delivery-ID**, the number of the order shown to users. Required, unique (without case) |
| `purchaseId` | **Purchase Order-ID** of the customer |
| `customerId` | The ordering party (an account). Required |
| `customerRoles` | Further parties with a role: `OrderingParty` (synchronized with `customerId`), `EndCustomer`, `InvoiceRecipient`, `DeliveryRecipient`, `Reseller`, `Referrer` |
| `orderStatus` | `Created`, `InProgress`, `Canceled`, `Closed` (the old values `OnHold`, `Finished` and `None` are stored as `Created`) |
| `description` | Notes |
| `orderProducts` | The product lines, see below |

The list (`OrdersList`) shows `name`, `customerName`, `status`, `orderId`, `purchaseId`.

## Product lines

Each line of `orderProducts` is a device or a license, told apart by `categoryId`:

| Field | `Hardware` line | `Software` line |
|---|---|---|
| `id` | ID of the device | ID of the issued license |
| `isNew` | `true`: create the device with this ID | `true`: create the license with this ID |
| `productUuid` | Hardware model (catalog product) | Software product (catalog) |
| `licenseUuid` | Hardware configuration | License type |
| `serialNumber` | Serial number of the device | License number ("Software-ID") |
| `macAddress` | MAC address | — |
| `expiration` | — | Expiration as `yyyy-MM-dd`, empty for unlimited |
| `isMultiple`, `productCount` | — | License for several seats and their number |

What the server does with the lines of `OrderAdd` and `OrderUpdate`:

- A line with `isNew: true` **creates** the device or the license, linked to the order. A new device with
  both a MAC address and a serial number starts with the production status `Finished`.
  Creating needs `/SensorManagement/Edit/Add` (devices) or `/LicenseManagement/Edit/Add` (licenses) in
  addition to the order permissions ("No permission to create new hardware").
- A line with `isNew: false` **links** an existing device or license to the order. A product already
  linked to another order is refused.
- Serial numbers, MAC addresses and license numbers must be unique; a duplicate is refused with the value
  named.
- A product that was linked to the order and is **missing** from the new list is **unlinked**: its order
  reference is cleared.

## The full list rule

`OrderUpdate` compares the new `orderProducts` with the products currently linked to the order (also those
linked directly through the device or license). Therefore every `OrderUpdate` must list **all** products
that shall stay linked, as `isNew: false` lines, even when only the status, the description or the
Delivery-ID changes. An update with an empty or partial list silently unlinks the missing devices and
licenses.

Procedure for any change of an order:

1. Read the order with `OrderItem` (its `orderProducts` are the linked products).
2. Change the fields; to add products, append lines; to remove a product, leave its line out.
3. Send the whole order with `OrderUpdate`.

## Things to know

- The Delivery-ID and the Purchase Order-ID shown in the lists of devices and licenses are read from the
  order at every request; changing them in the order changes them everywhere.
- A license linked to an order belongs to the customer of the order.
- Removing an order unlinks its devices and licenses; they stay, without an order.
- An order can only refer to customers, devices and licenses of the own organization (`Organizations.md`).
