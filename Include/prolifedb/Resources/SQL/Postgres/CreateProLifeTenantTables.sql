-- Tables of the tenant-owned collections, created in each dedicated tenant schema by the tenant storage provisioner.
-- Mirrors the current structure of these tables in the shared schema (revision 19).

CREATE TABLE IF NOT EXISTS "${TableScheme}"."Accounts"
(
    "Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "DocumentId" UUID NOT NULL,
    "TypeId" Text,
    "Document" jsonb NOT NULL,
    "Name" Text,
    "Description" Text,
    "RevisionInfo" jsonb,
    "DataMetaInfo" jsonb,
    "Derivates" jsonb,
    "TimeStamp" timestamp without time zone NOT NULL,
    "State" public."DocumentState"
);

CREATE INDEX IF NOT EXISTS "AccountsDocumentIdIndex" ON "${TableScheme}"."Accounts" ("DocumentId") WITH (deduplicate_items = true);
CREATE INDEX IF NOT EXISTS "AccountsRevisionNumberIndex" ON "${TableScheme}"."Accounts" ((("RevisionInfo"->>'RevisionNumber')::integer));
CREATE INDEX IF NOT EXISTS "AccountsStateIndex" ON "${TableScheme}"."Accounts" ("State");

CREATE TABLE IF NOT EXISTS "${TableScheme}"."Devices"
(
    "Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "DocumentId" UUID NOT NULL,
    "TypeId" Text,
    "Document" jsonb NOT NULL,
    "Name" Text,
    "Description" Text,
    "RevisionInfo" jsonb,
    "DataMetaInfo" jsonb,
    "Derivates" jsonb,
    "TimeStamp" timestamp without time zone NOT NULL,
    "State" public."DocumentState"
);

CREATE INDEX IF NOT EXISTS "DevicesDocumentIdIndex" ON "${TableScheme}"."Devices" ("DocumentId") WITH (deduplicate_items = true);
CREATE INDEX IF NOT EXISTS "DevicesRevisionNumberIndex" ON "${TableScheme}"."Devices" ((("RevisionInfo"->>'RevisionNumber')::integer));
CREATE INDEX IF NOT EXISTS "DevicesStateIndex" ON "${TableScheme}"."Devices" ("State");

CREATE TABLE IF NOT EXISTS "${TableScheme}"."Orders"
(
    "Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "DocumentId" UUID NOT NULL,
    "TypeId" Text,
    "Document" jsonb NOT NULL,
    "Name" Text,
    "Description" Text,
    "RevisionInfo" jsonb,
    "DataMetaInfo" jsonb,
    "Derivates" jsonb,
    "TimeStamp" timestamp without time zone NOT NULL,
    "State" public."DocumentState"
);

CREATE INDEX IF NOT EXISTS "OrdersDocumentIdIndex" ON "${TableScheme}"."Orders" ("DocumentId") WITH (deduplicate_items = true);
CREATE INDEX IF NOT EXISTS "OrdersRevisionNumberIndex" ON "${TableScheme}"."Orders" ((("RevisionInfo"->>'RevisionNumber')::integer));
CREATE INDEX IF NOT EXISTS "OrdersStateIndex" ON "${TableScheme}"."Orders" ("State");

CREATE TABLE IF NOT EXISTS "${TableScheme}"."SoftwareInstances"
(
    "Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "DocumentId" UUID NOT NULL,
    "TypeId" Text,
    "Document" jsonb NOT NULL,
    "Name" Text,
    "Description" Text,
    "RevisionInfo" jsonb,
    "DataMetaInfo" jsonb,
    "Derivates" jsonb,
    "TimeStamp" timestamp without time zone NOT NULL,
    "State" public."DocumentState"
);

CREATE INDEX IF NOT EXISTS "SoftwareInstancesDocumentIdIndex" ON "${TableScheme}"."SoftwareInstances" ("DocumentId") WITH (deduplicate_items = true);
CREATE INDEX IF NOT EXISTS "SoftwareInstancesRevisionNumberIndex" ON "${TableScheme}"."SoftwareInstances" ((("RevisionInfo"->>'RevisionNumber')::integer));
CREATE INDEX IF NOT EXISTS "SoftwareInstancesStateIndex" ON "${TableScheme}"."SoftwareInstances" ("State");

CREATE INDEX IF NOT EXISTS "DevicesConfigurationTypeIndex" ON "${TableScheme}"."Devices" (("DataMetaInfo"->>'ConfigurationType'));
CREATE INDEX IF NOT EXISTS "DevicesCustomerIdIndex" ON "${TableScheme}"."Devices" (("DataMetaInfo"->>'CustomerId'));
CREATE INDEX IF NOT EXISTS "DevicesDeviceTypeIndex" ON "${TableScheme}"."Devices" (("DataMetaInfo"->>'DeviceType'));
CREATE INDEX IF NOT EXISTS "DevicesOrderIdIndex" ON "${TableScheme}"."Devices" (("DataMetaInfo"->>'OrderId'));
CREATE INDEX IF NOT EXISTS "DevicesStatusIndex" ON "${TableScheme}"."Devices" (("DataMetaInfo"->>'Status'));
CREATE INDEX IF NOT EXISTS "OrdersCustomerIdIndex" ON "${TableScheme}"."Orders" (("DataMetaInfo"->>'CustomerId'));
CREATE INDEX IF NOT EXISTS "SoftwareInstancesCustomerIdIndex" ON "${TableScheme}"."SoftwareInstances" (("DataMetaInfo"->>'CustomerId'));
CREATE INDEX IF NOT EXISTS "SoftwareInstancesHardwareIdIndex" ON "${TableScheme}"."SoftwareInstances" (("DataMetaInfo"->>'HardwareId'));
CREATE INDEX IF NOT EXISTS "SoftwareInstancesLicenseUuidIndex" ON "${TableScheme}"."SoftwareInstances" (("DataMetaInfo"->>'LicenseUuid'));
CREATE INDEX IF NOT EXISTS "SoftwareInstancesOrderIdIndex" ON "${TableScheme}"."SoftwareInstances" (("DataMetaInfo"->>'OrderId'));
CREATE INDEX IF NOT EXISTS "SoftwareInstancesProductUuidIndex" ON "${TableScheme}"."SoftwareInstances" (("DataMetaInfo"->>'ProductUuid'));

CREATE TRIGGER trg_sync_document_uuid BEFORE INSERT OR UPDATE OF "Document", "DocumentId" ON "${TableScheme}"."Devices" FOR EACH ROW EXECUTE FUNCTION public.sync_document_uuid();
