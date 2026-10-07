-- Bind a deterministic subset of documents to two test tenants, in the format the application writes.
DELETE FROM "TenantEntityBindings" WHERE "TenantId" IN ('rls-tenant-1', 'rls-tenant-2');
INSERT INTO "TenantEntityBindings" ("Id", "TenantId", "EntityType", "EntityId", "CreatedAt")
SELECT gen_random_uuid()::text, CASE WHEN rn % 2 = 0 THEN 'rls-tenant-1' ELSE 'rls-tenant-2' END, t, id, now()
FROM (
	SELECT 'Accounts' AS t, "DocumentId"::text AS id, row_number() OVER (ORDER BY "DocumentId") AS rn FROM (SELECT DISTINCT "DocumentId" FROM "Accounts") a
	UNION ALL
	SELECT 'Devices', "DocumentId"::text, row_number() OVER (ORDER BY "DocumentId") FROM (SELECT DISTINCT "DocumentId" FROM "Devices") d
) x
WHERE rn <= 10;
SELECT "EntityType", "TenantId", count(*) FROM "TenantEntityBindings" WHERE "TenantId" LIKE 'rls-tenant-%' GROUP BY 1, 2 ORDER BY 1, 2;
SELECT "EntityId" FROM "TenantEntityBindings" WHERE "TenantId" = 'rls-tenant-1' AND "EntityType" = 'Accounts' ORDER BY 1 LIMIT 1;
