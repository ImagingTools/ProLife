-- ProLife no longer reads the Lisa catalog through postgres_fdw. A server that was killed instead of stopped
-- leaves the foreign tables "Products", "Licenses" and "Features" behind; the catalog tables of the next
-- revision must not be mistaken for them.
DROP SERVER IF EXISTS "LisaServer" CASCADE;
