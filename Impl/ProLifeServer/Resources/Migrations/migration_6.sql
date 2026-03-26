ALTER TABLE "Devices" RENAME COLUMN "CheckSum" TO "Checksum";
ALTER TABLE "Orders" RENAME COLUMN "CheckSum" TO "Checksum";

DROP TABLE "Users" CASCADE;
DROP TABLE "Roles" CASCADE;

CREATE TABLE "Users"
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

CREATE TABLE "Roles"
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

