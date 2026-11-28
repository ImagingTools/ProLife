# Hardware catalog

The hardware catalog describes what devices are built from. A device (`Devices.md`) refers to a hardware model
and to one of its configurations.

| Page | API | What it is |
|---|---|---|
| Hardware Models | `ProductsList`, `ProductItem` with `categoryId` `Hardware` | A kind of device that is built and sold; the "device type" of a device |
| Configurations | `HardwareConfigurationsList`, `GetHardwareConfigurationItem`, `AddHardwareConfiguration`, `UpdateHardwareConfiguration` | A variant of a hardware model that can be ordered and built: which components it takes |
| Components | `HardwareComponentsList`, `GetHardwareComponentItem`, `AddHardwareComponent`, `UpdateHardwareComponent` | A part, module, assembly or consumable, with its manufacturer and suppliers |
| Vendors | Accounts with the role manufacturer or supplier (`Customers.md`) | Companies that make or sell components |

The single-item queries take `{"id": ...}`; the writing commands `{"id": ..., "item": {...}}` and replace the document.

## Components

| Field | Meaning |
|---|---|
| `partNumber` | Own part number. Required |
| `name` | Name. Required |
| `componentType` | `Part`, `Module`, `Assembly` or `Consumable` |
| `revision` | Revision of the component |
| `manufacturerId` | Account of the manufacturer; must have the role manufacturer |
| `mpn` | Part number of the manufacturer |
| `supplierOffers` | List of `{supplierId, sku}`: who sells it under which number; each supplier must have the role supplier and appear once |
| `lifecycleStatus` | `Active`, `NRND` (not recommended for new designs) or `EOL` (end of life) |
| `serialized` | Each unit has its own serial number |
| `manufacturerName`, `supplierName` | Read only |

## Configurations

| Field | Meaning |
|---|---|
| `configurationId` | Article number of the configuration. Required |
| `name` | Name. Required |
| `productId` | The hardware model. Required; must be a product of the category `Hardware` |
| `componentIds` | IDs of the components it takes; each must exist |
| `description` | Notes |
| `productName`, `components` | Read only: names resolved by the server |

## Rules

- A device must name a hardware model and a configuration of that model.
- Removing a component removes it from the configurations that take it.
- Removing a company makes it stop being the manufacturer or a supplier of components.
- To find components that are no longer recommended, filter or read the components by `lifecycleStatus` and check the
  configurations that take them (`componentIds`).
- Permissions: `/HardwareManagement/...` for components, configurations and vendors, `/ProductManagement/...` for hardware
  models.
