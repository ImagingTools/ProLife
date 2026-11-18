-- The hardware vendors are companies now: every company has the roles customer, manufacturer and supplier.
-- The companies made so far are the customers. The roles are filtered on, so the meta info gets them too.
UPDATE "Accounts"
SET "Document" = "Document" || jsonb_build_object('Website', '', 'IsCustomer', true, 'IsManufacturer', false, 'IsSupplier', false)
WHERE NOT ("Document" ? 'IsCustomer');

UPDATE "Accounts"
SET "DataMetaInfo" = coalesce("DataMetaInfo", '{}'::jsonb) || jsonb_build_object(
	'Website', coalesce("Document"->>'Website', ''),
	'IsCustomer', coalesce(("Document"->>'IsCustomer')::bool, false),
	'IsManufacturer', coalesce(("Document"->>'IsManufacturer')::bool, false),
	'IsSupplier', coalesce(("Document"->>'IsSupplier')::bool, false))
WHERE NOT (coalesce("DataMetaInfo", '{}'::jsonb) ? 'IsCustomer');
