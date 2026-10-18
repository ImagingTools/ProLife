-- Tenant isolation check of the binding-scoped ProLife collections, executed as the application role.
-- Every check prints PASS/FAIL; the expected values are computed in the system context (imt.rls_bypass).
\set ON_ERROR_STOP on
SET client_min_messages = notice;

DO $$
DECLARE
	tbl text;
	tenantId text;
	otherTenantId text;
	foreignDocumentId text;
	total int; unbound int; visible int; own int; leaked int; changed int; tenantCount int;
	isSuper bool; isBypass bool;
	ownerName text; rlsEnabled bool; rlsForced bool; policyCount int;
	bindingSql text;
BEGIN
	SELECT rolsuper, rolbypassrls INTO isSuper, isBypass FROM pg_roles WHERE rolname = current_user;
	RAISE NOTICE '% role %: superuser=%, bypassrls=%', CASE WHEN NOT isSuper AND NOT isBypass THEN 'PASS' ELSE 'FAIL' END, current_user, isSuper, isBypass;

	FOREACH tbl IN ARRAY ARRAY['Accounts', 'Devices', 'Orders', 'SoftwareInstances'] LOOP
		SELECT pg_get_userbyid(c.relowner), c.relrowsecurity, c.relforcerowsecurity INTO ownerName, rlsEnabled, rlsForced
			FROM pg_class c JOIN pg_namespace n ON n.oid = c.relnamespace WHERE n.nspname = 'public' AND c.relname = tbl;
		SELECT count(*) INTO policyCount FROM pg_policies WHERE schemaname = 'public' AND tablename = tbl;
		RAISE NOTICE '% %: owner=%, rls=%, force=%, policies=%',
			CASE WHEN ownerName = current_user AND rlsEnabled AND rlsForced AND policyCount > 0 THEN 'PASS' ELSE 'FAIL' END,
			tbl, ownerName, rlsEnabled, rlsForced, policyCount;

		bindingSql := format('EXISTS (SELECT 1 FROM "TenantEntityBindings" b WHERE b."EntityType" = %L AND b."EntityId" = d."DocumentId"::text AND b."TenantId" = $1)', tbl);

		-- Truth in the system context
		PERFORM set_config('imt.tenant_id', '', false);
		PERFORM set_config('imt.user_id', '', false);
		PERFORM set_config('imt.rls_bypass', 'on', false);
		EXECUTE format('SELECT count(*) FROM %I', tbl) INTO total;
		EXECUTE format('SELECT count(*) FROM %I d WHERE NOT EXISTS (SELECT 1 FROM "TenantEntityBindings" b WHERE b."EntityType" = %L AND b."EntityId" = d."DocumentId"::text AND COALESCE(b."TenantId", '''') <> '''')', tbl, tbl) INTO unbound;
		SELECT count(DISTINCT "TenantId") INTO tenantCount FROM "TenantEntityBindings" WHERE "EntityType" = tbl AND COALESCE("TenantId", '') <> '';

		-- No tenant context: global (unbound) rows only
		PERFORM set_config('imt.rls_bypass', 'off', false);
		EXECUTE format('SELECT count(*) FROM %I', tbl) INTO visible;
		RAISE NOTICE '% % without context: visible %, expected % unbound of % rows (% tenants own rows)',
			CASE WHEN visible = unbound THEN 'PASS' ELSE 'FAIL' END, tbl, visible, unbound, total, tenantCount;

		FOR tenantId IN SELECT DISTINCT "TenantId" FROM "TenantEntityBindings" WHERE "EntityType" = tbl AND COALESCE("TenantId", '') <> '' ORDER BY 1 LOOP
			PERFORM set_config('imt.rls_bypass', 'on', false);
			PERFORM set_config('imt.tenant_id', '', false);
			EXECUTE format('SELECT count(*) FROM %I d WHERE ', tbl) || bindingSql INTO own USING tenantId;
			EXECUTE format('SELECT d."DocumentId"::text FROM %I d WHERE NOT ', tbl) || bindingSql
				|| format(' AND EXISTS (SELECT 1 FROM "TenantEntityBindings" b WHERE b."EntityType" = %L AND b."EntityId" = d."DocumentId"::text AND COALESCE(b."TenantId", '''') NOT IN ('''', $1)) LIMIT 1', tbl)
				INTO foreignDocumentId USING tenantId;
			SELECT "TenantId" INTO otherTenantId FROM "TenantEntityBindings" WHERE COALESCE("TenantId", '') NOT IN ('', tenantId) LIMIT 1;

			PERFORM set_config('imt.rls_bypass', 'off', false);
			PERFORM set_config('imt.tenant_id', tenantId, false);

			EXECUTE format('SELECT count(*) FROM %I', tbl) INTO visible;
			EXECUTE format('SELECT count(*) FROM %I d WHERE NOT ', tbl) || bindingSql
				|| format(' AND EXISTS (SELECT 1 FROM "TenantEntityBindings" b WHERE b."EntityType" = %L AND b."EntityId" = d."DocumentId"::text AND COALESCE(b."TenantId", '''') <> '''')', tbl)
				INTO leaked USING tenantId;
			RAISE NOTICE '% % tenant %: visible %, expected % own + % global, foreign visible %',
				CASE WHEN visible = own + unbound AND leaked = 0 THEN 'PASS' ELSE 'FAIL' END, tbl, tenantId, visible, own, unbound, leaked;

			IF foreignDocumentId IS NOT NULL THEN
				EXECUTE format('UPDATE %I SET "DocumentId" = "DocumentId" WHERE "DocumentId"::text = %L', tbl, foreignDocumentId);
				GET DIAGNOSTICS changed = ROW_COUNT;
				RAISE NOTICE '% % tenant %: update of foreign document % changed % rows',
					CASE WHEN changed = 0 THEN 'PASS' ELSE 'FAIL' END, tbl, tenantId, foreignDocumentId, changed;

				BEGIN
					EXECUTE format('DELETE FROM %I WHERE "DocumentId"::text = %L', tbl, foreignDocumentId);
					GET DIAGNOSTICS changed = ROW_COUNT;
					RAISE EXCEPTION USING ERRCODE = 'P0001', MESSAGE = 'rollback';
				EXCEPTION WHEN raise_exception THEN
					NULL;
				END;
				RAISE NOTICE '% % tenant %: delete of foreign document % removed % rows',
					CASE WHEN changed = 0 THEN 'PASS' ELSE 'FAIL' END, tbl, tenantId, foreignDocumentId, changed;

				BEGIN
					DELETE FROM "TenantEntityBindings" WHERE "EntityType" = tbl AND "EntityId" = foreignDocumentId;
					GET DIAGNOSTICS changed = ROW_COUNT;
					RAISE EXCEPTION USING ERRCODE = 'P0001', MESSAGE = 'rollback';
				EXCEPTION WHEN raise_exception THEN
					NULL;
				END;
				RAISE NOTICE '% % tenant %: removal of the foreign binding (would make the document global) removed % rows',
					CASE WHEN changed = 0 THEN 'PASS' ELSE 'FAIL' END, tbl, tenantId, changed;
			END IF;

			IF otherTenantId IS NOT NULL THEN
				BEGIN
					INSERT INTO "TenantEntityBindings" ("Id", "TenantId", "EntityType", "EntityId", "CreatedAt")
						VALUES (gen_random_uuid()::text, otherTenantId, tbl, gen_random_uuid()::text, now());
					RAISE NOTICE 'FAIL % tenant %: binding for tenant % was inserted', tbl, tenantId, otherTenantId;
					RAISE EXCEPTION USING ERRCODE = 'P0001', MESSAGE = 'rollback';
				EXCEPTION
					WHEN insufficient_privilege THEN
						RAISE NOTICE 'PASS % tenant %: binding for foreign tenant % rejected (%)', tbl, tenantId, otherTenantId, SQLSTATE;
					WHEN raise_exception THEN
						NULL;
				END;
			END IF;
		END LOOP;
	END LOOP;

	PERFORM set_config('imt.tenant_id', '', false);
	PERFORM set_config('imt.rls_bypass', 'off', false);
END
$$;
