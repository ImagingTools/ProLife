# Statistics (workspace)

The dashboard and the analytics page of ProLife read prepared statistics. Every query takes a `ChartInput`:

```json
{"timeFilter": {"timeUnit": "Month", "interpretationMode": "Current"}, "customerId": "<account>"}
```

- `timeFilter` (optional): `timeUnit` `Day`, `Week`, `Month`, `Year` with `interpretationMode` `Current` (the current
  day, week, ...), `Last` (the previous one) or `For` (the last `unitMultiplier` units up to now, at least one), or `timeUnit` `Custom` with
  `timeRange` `{"Begin": "<ISO date>", "End": "<ISO date>"}`. Without it all time is counted.
- `customerId` (optional): only the products of one customer.

Common rules:

- Documents of **internal use** are left out of the charts (the summary counts them separately).
- The user sees only the documents his groups allow (`Permissions.md`), so two users can get different numbers.
- For **licenses** the time filter is applied to the date the **license file** was first created, not to the date the
  license was issued. Licenses without a license file are counted only when no time filter is given. For devices and
  orders the time filter is applied to the date they were created.

## Queries

| Query | Result | What is counted |
|---|---|---|
| `GetTotalSummaryInfo` | `summaryInfos`: per area (`SoftwareProducts`, `Devices`, `Orders`) `total`, `inUseCount`, `notInUseCount`, `internalUseCount` | Licenses: in use = license file created. Devices: in use = has bound licenses. Orders: total only |
| `GetSoftwareUsedPieChart`, `GetSoftwareUsedBarChart` | Segments per software product | Licenses in use (with a license file), by product |
| `GetHardwareUsedPieChart`, `GetHardwareUsedBarChart` | Segments per hardware model | Devices in use (with bound licenses), by model |
| `GetHardwareStatusInfo` | Segments per production status | Devices by production status |
| `GetHardwareConfigurationPieChart` | Segments per configuration | Devices by hardware configuration |
| `GetHardwareCustomerPieChart`, `GetSoftwareCustomerPieChart` | Segments per customer | Devices or licenses by customer |
| `GetHardwareCreationBarChart`, `GetSoftwareCreationBarChart` | Bars per period, segments per model or product | Devices or licenses created per period |
| `GetLicenseCreationInfo` | Line, one point per period | License files created per period |
| `GetOrderCreationLineChart` | Line, one point per period | Orders created per period |

Pie charts return `segments` (`label`, `value`) and a `summary` (`total`, the largest segment as `maxItem`). Bar and line charts
return `axes`, the `bars` or `labels`/`points`, and a `summary`; the period of the bars follows the time filter (days, weeks,
months or years).

## Answering questions

- Prefer these queries for counts and distributions; read the lists only when the user needs the documents themselves
  or a criterion the charts do not have (for example expiring licenses, `Licensing.md`).
- Say which time filter and which customer the numbers are for, and that licenses are dated by their license file.
- Permissions: `GetTotalSummaryInfo` for everyone; the charts of devices need `/SensorManagement/View`, of licenses
  `/LicenseManagement/View`, of orders `/OrderManagement/View`.
