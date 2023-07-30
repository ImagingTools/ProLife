ALTER TABLE "Accounts" ADD COLUMN "OwnerId" VARCHAR;
ALTER TABLE "Accounts" ADD COLUMN "OwnerName" VARCHAR;
ALTER TABLE "Accounts" ADD COLUMN "OperationDescription" VARCHAR;

ALTER TABLE "Devices" ADD COLUMN "OwnerId" VARCHAR;
ALTER TABLE "Devices" ADD COLUMN "OwnerName" VARCHAR;
ALTER TABLE "Devices" ADD COLUMN "OperationDescription" VARCHAR;

ALTER TABLE "Orders" ADD COLUMN "OwnerId" VARCHAR;
ALTER TABLE "Orders" ADD COLUMN "OwnerName" VARCHAR;
ALTER TABLE "Orders" ADD COLUMN "OperationDescription" VARCHAR;