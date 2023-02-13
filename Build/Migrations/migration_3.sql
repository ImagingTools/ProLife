DROP TABLE "Devices" CASCADE;

CREATE TABLE "Devices"(
    Id SERIAL,
    DocumentId VARCHAR (1000) NOT NULL,
    AccountId VARCHAR (1000) NOT NULL, 
    Document JSONB,
    RevisionNumber BIGINT,
    LastModified TIMESTAMP,
    Checksum BIGINT,
    IsActive BOOLEAN,
    PRIMARY KEY (Id)
);

