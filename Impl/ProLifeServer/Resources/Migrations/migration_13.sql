UPDATE "Devices" SET "Document" = jsonb_set("Document", '{Project}', "Document"->'Description') WHERE "IsActive" = true;
DROP TABLE IF EXISTS "Roles";
DROP TABLE IF EXISTS "Users";
DROP TABLE IF EXISTS "UserGroups";
DROP TABLE IF EXISTS "UserSessions";