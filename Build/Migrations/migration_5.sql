DROP TABLE public."GroupUsers" CASCADE;
DROP TABLE public."ParentGroups" CASCADE;
DROP TABLE public."UserGroups" CASCADE;

CREATE TABLE public."UserGroups"
(
    "Id" SERIAL,
    "DocumentId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
    "Document" jsonb,
    "RevisionNumber" bigint,
    "LastModified" timestamp without time zone,
    "Checksum" bigint,
    "IsActive" boolean,
     PRIMARY KEY ("Id")
);

ALTER TABLE "Devices" RENAME COLUMN Id TO "Id";
ALTER TABLE "Devices" RENAME COLUMN DocumentId TO "DocumentId";
ALTER TABLE "Devices" RENAME COLUMN AccountId TO "AccountId";
ALTER TABLE "Devices" RENAME COLUMN Document TO "Document";
ALTER TABLE "Devices" RENAME COLUMN RevisionNumber TO "RevisionNumber";
ALTER TABLE "Devices" RENAME COLUMN LastModified TO "LastModified";
ALTER TABLE "Devices" RENAME COLUMN CheckSum TO "CheckSum";
ALTER TABLE "Devices" RENAME COLUMN IsActive TO "IsActive";

ALTER TABLE "Orders" RENAME COLUMN Id TO "Id";
ALTER TABLE "Orders" RENAME COLUMN OrderId TO "OrderId";
ALTER TABLE "Orders" RENAME COLUMN AccountId TO "AccountId";
ALTER TABLE "Orders" RENAME COLUMN Document TO "Document";
ALTER TABLE "Orders" RENAME COLUMN RevisionNumber TO "RevisionNumber";
ALTER TABLE "Orders" RENAME COLUMN LastModified TO "LastModified";
ALTER TABLE "Orders" RENAME COLUMN CheckSum TO "CheckSum";
ALTER TABLE "Orders" RENAME COLUMN IsActive TO "IsActive";