# Customers (accounts)

An account is a company. The same record can have several roles:

- **customer** (`isCustomer`): orders products; appears on the page Customers;
- **manufacturer** (`isManufacturer`): makes hardware components;
- **supplier** (`isSupplier`): sells hardware components.

The page Vendors shows the manufacturers and suppliers. An account saved from the page Customers is
always a customer; an account saved from Vendors must be a manufacturer or a supplier ("The company is
neither a manufacturer nor a supplier").

## Fields

| Field | Meaning |
|---|---|
| `name` | Name of the account; required and unique ("Account name '...' already exists") |
| `customerId` | Customer number of the company, the "Customer-ID"; unique if given |
| `companyName`, `street`, `postalCode`, `city`, `country` | Address |
| `email`, `website`, `description` | Contact and notes; the e-mail is not validated by the server |
| `groups` | User groups that may see the account and its devices, licenses and orders (see `Permissions.md`) |
| `isCustomer`, `isManufacturer`, `isSupplier` | Roles |

The list (`AccountsList`) also has `roles`, the roles as text ("Customer, Supplier").

## Commands

- `AccountsList`, `AccountItem` (by ID) read.
- `AccountAdd`, `AccountUpdate` write `{"id": "<uuid>", "item": {...}}`. For a new account choose a new UUID.
  `AccountUpdate` replaces the account: send all fields.

## Things to know

- The name of the customer shown in the lists of orders, devices and licenses is read from the account
  at every request. Renaming the account renames it everywhere; nothing else has to be changed.
- Removing a company makes it stop being the manufacturer or a supplier of the hardware components
  that named it. Orders, devices and licenses keep the ID of a removed customer and show no name for
  it, so check them before removing a customer.
- Accounts can be shared with other organizations (`Organizations.md`).
