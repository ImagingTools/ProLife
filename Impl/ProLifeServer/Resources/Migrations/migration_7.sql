DROP TABLE "Accounts" CASCADE;

CREATE TABLE "Accounts"
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

ALTER TABLE "Orders" RENAME COLUMN "OrderId" TO "DocumentId";

ALTER TABLE "Orders" DROP COLUMN "AccountId";
ALTER TABLE "Devices" DROP COLUMN "AccountId";

DROP TABLE "ParentRoles" CASCADE;
DROP TABLE "RolePermissions" CASCADE;
DROP TABLE "UserPermissions" CASCADE;
DROP TABLE "UserRoles" CASCADE;