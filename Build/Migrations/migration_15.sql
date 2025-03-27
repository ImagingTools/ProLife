CREATE INDEX "SoftwareInstancesProductUuidIndex" ON "SoftwareInstances" (("DataMetaInfo"->>'ProductUuid'));
CREATE INDEX "SoftwareInstancesLicenseUuidIndex" ON "SoftwareInstances" (("DataMetaInfo"->>'LicenseUuid'));
CREATE INDEX "SoftwareInstancesOrderIdIndex" ON "SoftwareInstances" (("DataMetaInfo"->>'OrderId'));
CREATE INDEX "SoftwareInstancesCustomerIdIndex" ON "SoftwareInstances" (("DataMetaInfo"->>'CustomerId'));
CREATE INDEX "SoftwareInstancesHardwareIdIndex" ON "SoftwareInstances" (("DataMetaInfo"->>'HardwareId'));

CREATE INDEX "DevicesDeviceTypeIndex" ON "Devices" (("DataMetaInfo"->>'DeviceType'));
CREATE INDEX "DevicesConfigurationTypeIndex" ON "Devices" (("DataMetaInfo"->>'ConfigurationType'));
CREATE INDEX "DevicesCustomerIdIndex" ON "Devices" (("DataMetaInfo"->>'CustomerId'));
CREATE INDEX "DevicesOrderIdIndex" ON "Devices" (("DataMetaInfo"->>'OrderId'));
CREATE INDEX "DevicesStatusIndex" ON "Devices" (("DataMetaInfo"->>'Status'));

CREATE INDEX "OrdersCustomerIdIndex" ON "Orders" (("DataMetaInfo"->>'CustomerId'));

DROP TABLE IF EXISTS "Roles";
DROP TABLE IF EXISTS "Users";
DROP TABLE IF EXISTS "UserGroups";
DROP TABLE IF EXISTS "UserSessions";
