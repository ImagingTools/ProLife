CREATE TABLE public."BindingProducts"
(
    "Id" SERIAL,
    "DocumentId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
    "Document" jsonb,
    "RevisionNumber" bigint,
    "LastModified" timestamp without time zone,
    "Checksum" bigint,
    "IsActive" boolean,
	"OwnerId" character varying(1000),
	"OwnerName" character varying(1000),
	"OperationDescription" character varying,
     PRIMARY KEY ("Id")
);