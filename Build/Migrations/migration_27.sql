-- Some software instances refer to a license by its article ("12.10128 (4222 8000 05)") instead of its ID. The licenses are
-- in this database now (migration 25): such a reference becomes the ID of the license of the same product with the same
-- article number. A reference without exactly one such license stays as it is.
UPDATE "SoftwareInstances" AS instance
SET "Document" = jsonb_set(instance."Document", '{Licenses}', (
	SELECT jsonb_agg(
		CASE
			WHEN item->'LicenseData'->>'LicenseId' ~* '^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$' THEN item
			ELSE coalesce((
				SELECT jsonb_set(item, '{LicenseData,LicenseId}', to_jsonb(min(license."DocumentId"::text)))
				FROM "Licenses" AS license
				WHERE license."State" = 'Active'
					AND license."Document"->>'ProductId' = instance."Document"->>'ProductId'
					AND split_part(regexp_replace(license."Document"->>'LicenseId', '^\[[^]]*\]\s*', ''), ' ', 1)
						= split_part(regexp_replace(item->'LicenseData'->>'LicenseId', '^\[[^]]*\]\s*', ''), ' ', 1)
				HAVING count(*) = 1), item)
		END
		ORDER BY position)
	FROM jsonb_array_elements(instance."Document"->'Licenses') WITH ORDINALITY AS element(item, position)))
WHERE instance."State" = 'Active'
	AND EXISTS (
		SELECT 1
		FROM jsonb_array_elements(instance."Document"->'Licenses') AS element(item)
		WHERE coalesce(item->'LicenseData'->>'LicenseId', '') <> ''
			AND item->'LicenseData'->>'LicenseId' !~* '^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$');
