CREATE TYPE "DocumentState" AS ENUM ('Active', 'InActive', 'Disabled');

ALTER TABLE public."Devices" RENAME TO "Devices_new";
ALTER TABLE public."SoftwareInstances" RENAME TO "SoftwareInstances_new";
ALTER TABLE public."Orders" RENAME TO "Orders_new";
ALTER TABLE public."Accounts" RENAME TO "Accounts_new";
ALTER TABLE public."BindingProducts" RENAME TO "BindingProducts_new";