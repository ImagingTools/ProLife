CREATE TABLE IF NOT EXISTS "Users"
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

CREATE TABLE IF NOT EXISTS "Roles"
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

CREATE TABLE IF NOT EXISTS "UserGroups"
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

CREATE TABLE IF NOT EXISTS  "UserSessions"(
    "AccessToken" UUID NOT NULL,
    "UserId" VARCHAR (1000) NOT NULL,
    "LastActivity" TIMESTAMP NOT NULL,
    PRIMARY KEY ("AccessToken")
);

ALTER TABLE "Users" ADD COLUMN "OwnerId" VARCHAR;
ALTER TABLE "Users" ADD COLUMN "OwnerName" VARCHAR;
ALTER TABLE "Users" ADD COLUMN "OperationDescription" VARCHAR;

ALTER TABLE "Roles" ADD COLUMN "OwnerId" VARCHAR;
ALTER TABLE "Roles" ADD COLUMN "OwnerName" VARCHAR;
ALTER TABLE "Roles" ADD COLUMN "OperationDescription" VARCHAR;

ALTER TABLE "UserGroups" ADD COLUMN "OwnerId" VARCHAR;
ALTER TABLE "UserGroups" ADD COLUMN "OwnerName" VARCHAR;
ALTER TABLE "UserGroups" ADD COLUMN "OperationDescription" VARCHAR;

CREATE TABLE "Accounts"
(
    "Id" SERIAL,
	"DocumentId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
	"Document" jsonb,
	"RevisionNumber" bigint,
	"LastModified" timestamp without time zone,
	"Checksum" bigint,
	"IsActive" boolean,
	"OwnerId" VARCHAR,
	"OwnerName" VARCHAR,
	"OperationDescription" VARCHAR,
	 PRIMARY KEY ("Id")
);

CREATE TABLE "BindingProducts"
(
    "Id" SERIAL,
	"DocumentId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
	"Document" jsonb,
	"RevisionNumber" bigint,
	"LastModified" timestamp without time zone,
	"Checksum" bigint,
	"IsActive" boolean,
	"OwnerId" VARCHAR,
	"OwnerName" VARCHAR,
	"OperationDescription" VARCHAR,
	 PRIMARY KEY ("Id")
);

CREATE TABLE "Devices"
(
    "Id" SERIAL,
	"DocumentId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
	"Document" jsonb,
	"RevisionNumber" bigint,
	"LastModified" timestamp without time zone,
	"Checksum" bigint,
	"IsActive" boolean,
	"OwnerId" VARCHAR,
	"OwnerName" VARCHAR,
	"OperationDescription" VARCHAR,
	 PRIMARY KEY ("Id")
);

CREATE TABLE "Orders"
(
    "Id" SERIAL,
	"DocumentId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
	"Document" jsonb,
	"RevisionNumber" bigint,
	"LastModified" timestamp without time zone,
	"Checksum" bigint,
	"IsActive" boolean,
	"OwnerId" VARCHAR,
	"OwnerName" VARCHAR,
	"OperationDescription" VARCHAR,
	 PRIMARY KEY ("Id")
);

CREATE TABLE "SoftwareInstances"
(
    "Id" SERIAL,
	"DocumentId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
	"Document" jsonb,
	"RevisionNumber" bigint,
	"LastModified" timestamp without time zone,
	"Checksum" bigint,
	"IsActive" boolean,
	"OwnerId" VARCHAR,
	"OwnerName" VARCHAR,
	"OperationDescription" VARCHAR,
	 PRIMARY KEY ("Id")
);
