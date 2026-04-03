-- Migration 20: Add Procurement and IQC tables
-- Procurement Orders: stores procurement order header data as JSONB documents

CREATE TABLE public."ProcurementOrders"
(
    "Id"                 SERIAL,
    "DocumentId"         character varying(1000) COLLATE pg_catalog."default" NOT NULL,
    "Document"           jsonb,
    "RevisionNumber"     bigint,
    "LastModified"       timestamp without time zone,
    "Checksum"           bigint,
    "IsActive"           boolean,
    "OwnerId"            character varying(1000),
    "OwnerName"          character varying(1000),
    "OperationDescription" character varying,
    "DataMetaInfo"       jsonb,
    "RevisionInfo"       jsonb,
    "State"              character varying(100),
    PRIMARY KEY ("Id")
);

CREATE INDEX "ProcurementOrdersStatusIndex"
    ON public."ProcurementOrders" (("Document"->>'Status'));

CREATE INDEX "ProcurementOrdersSupplierIdIndex"
    ON public."ProcurementOrders" (("Document"->>'SupplierId'));

-- IQC Runs: stores IQC run data (manual and automated) as JSONB documents

CREATE TABLE public."IqcRuns"
(
    "Id"                 SERIAL,
    "DocumentId"         character varying(1000) COLLATE pg_catalog."default" NOT NULL,
    "Document"           jsonb,
    "RevisionNumber"     bigint,
    "LastModified"       timestamp without time zone,
    "Checksum"           bigint,
    "IsActive"           boolean,
    "OwnerId"            character varying(1000),
    "OwnerName"          character varying(1000),
    "OperationDescription" character varying,
    "DataMetaInfo"       jsonb,
    "RevisionInfo"       jsonb,
    "State"              character varying(100),
    PRIMARY KEY ("Id")
);

CREATE INDEX "IqcRunsBatchUuidIndex"
    ON public."IqcRuns" (("Document"->>'BatchUuid'));

CREATE INDEX "IqcRunsSystemIdIndex"
    ON public."IqcRuns" (("Document"->>'SystemId'));

CREATE INDEX "IqcRunsExternalRunIdIndex"
    ON public."IqcRuns" (("Document"->>'ExternalRunId'));

-- Unique index to support idempotent upsert by (SystemId, ExternalRunId)
CREATE UNIQUE INDEX "IqcRunsSystemExternalRunUniqueIndex"
    ON public."IqcRuns" (("Document"->>'SystemId'), ("Document"->>'ExternalRunId'))
    WHERE ("Document"->>'SystemId') IS NOT NULL
      AND ("Document"->>'ExternalRunId') IS NOT NULL;

-- IQC Templates: stores IQC checklist template definitions as JSONB documents

CREATE TABLE public."IqcTemplates"
(
    "Id"                 SERIAL,
    "DocumentId"         character varying(1000) COLLATE pg_catalog."default" NOT NULL,
    "Document"           jsonb,
    "RevisionNumber"     bigint,
    "LastModified"       timestamp without time zone,
    "Checksum"           bigint,
    "IsActive"           boolean,
    "OwnerId"            character varying(1000),
    "OwnerName"          character varying(1000),
    "OperationDescription" character varying,
    "DataMetaInfo"       jsonb,
    "RevisionInfo"       jsonb,
    "State"              character varying(100),
    PRIMARY KEY ("Id")
);

CREATE INDEX "IqcTemplatesSupplierIdIndex"
    ON public."IqcTemplates" (("Document"->>'SupplierId'));

CREATE INDEX "IqcTemplatesComponentIdIndex"
    ON public."IqcTemplates" (("Document"->>'ComponentId'));

CREATE INDEX "IqcTemplatesActiveIndex"
    ON public."IqcTemplates" (("Document"->>'Active'));
