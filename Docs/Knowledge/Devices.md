# Devices (hardware)

A device is one built unit: a hardware model in one configuration, with its own serial number and MAC
address. In the client it is "Hardware", in the permissions and some messages "Sensor".

## Fields

| Field (`DeviceData`) | Meaning |
|---|---|
| `serialNumber` | Serial number; unique ("Serial Number '...' already exists") |
| `macAddress` | MAC address `aa:bb:cc:dd:ee:ff`; unique. Needed for the license file |
| `deviceType` | ID of the **hardware model** (a catalog product of the category `Hardware`). Required |
| `licenseName` | ID of the **hardware configuration**, despite its name. Required; the configuration must belong to the hardware model ("The configuration '...' does not belong to the device type") |
| `productionStatus` | See below |
| `orderId` | ID of the order document (not the Delivery-ID); empty without an order |
| `project` | Project the device and its licenses are used in |
| `description`, `internalUse` | Notes; `internalUse` marks a device of the own company, left out of the statistics |
| `productName`, `orderName`, `configurationName`, `configurationArticle`, `softwareBindingInfos` | Read only: names resolved by the server, the bound licenses |

The list (`DevicesList`, type `DeviceItem`) adds `customerLink`, `deliveryIdLink`, `purchaseIdLink` (read
from the order and its customer at every request), `status` (name) and `statusId`, `inUse` (the device has
bound licenses).

## Production status

| ID (`productionStatus`, `statusId`) | Number (list filter `Status`) | Meaning |
|---|---|---|
| `None` | 0 | Not set |
| `Accepted` | 1 | Order for the device accepted |
| `InProgress` | 2 | Being built |
| `Canceled` | 3 | Build canceled |
| `OnHold` | 4 | Build paused |
| `Finished` | 5 | Built and ready; only now license files and transfers are possible |
| `Defected` | 6 | Defective; set automatically on the source device of a license transfer |
| `InRepair` | 7 | Being repaired |
| `Decommissioned` | 8 | Out of service |

## Binding licenses

The licenses of a device are its **binding**: `GetDeviceBinding` (`{"id": <device>}`) reads it,
`UpdateDeviceBinding` writes it:

```json
{"deviceId": "<device>", "project": "<project>",
 "item": {"id": "<device>", "softwareIds": ["<license 1>", "<license 2>"]}}
```

- `softwareIds` is the **whole new list**: licenses missing from it are unbound. To add a license, read the
  binding and append its ID.
- All licenses of a device must be of the same software product ("Licenses must be of the same product"),
  and the same license must not be chosen twice.
- Binding a license takes one seat of it (its `productCount` is reduced by one); unbinding gives it back.
- A bound license takes over the `project` of the binding; an unbound license loses its project and its
  "in use" mark.
- A license bound to a device with a MAC address is "paired" (`isPaired`).
- Permissions: `/SensorManagement/Bind`, `/SensorManagement/Unbind`.

## License file

`CreateLicenseFile` (`{"deviceId": ..., "encrypt": true|false}`) creates the license file `<mac>_License.lic`
of a device. Conditions:

- the device is `Finished`;
- it has a valid MAC address;
- it has bound licenses, all of the same product.

After the file was created the licenses are "in use" (`inUse`): the device and these licenses can no
longer be removed ("... because a license file has been created for it"). The file content is returned to
the client; the assistant does not create or read license files.

## Transferring licenses to another device

`TransferLicenses` (`{"fromDeviceId": ..., "toDeviceId": ...}`) moves all licenses of a device to another one,
for example when the hardware was replaced:

- the source device must be `Finished`, the target must have no licenses, and they must differ;
- every license can be transferred a limited number of times (3 by default). When a license reached the
  limit, the answer has `limit: true` and the support address `supportEmail`; `RequestTransferLicenses`
  then sends a request to the support, and a user with `/SensorManagement/ResetTransferCounter` can reset
  the counters of the licenses of a device with `ResetTransferCounter` (`{"hardwareId": ...}`);
- after the transfer the **source device is set to `Defected`** and the counter of each license grows by one.

A transfer cannot be undone; it is only done in the client, by a user who confirms it there.

## Other rules

- Devices are usually created by an order (`Orders.md`); `DeviceAdd` creates one directly.
- Without `/SensorManagement/ViewAll` a user sees only the devices of the customers of his groups
  (`Permissions.md`).
- The editor of the client applies the permissions of single fields (serial number, device type,
  configuration, ...); the assistant must respect them as well: change only the fields the user may change.
