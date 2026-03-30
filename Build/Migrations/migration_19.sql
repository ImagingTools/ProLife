CREATE OR REPLACE FUNCTION sync_document_uuid()
RETURNS trigger AS $$
BEGIN
    IF NEW."DocumentId" IS NULL THEN
        RAISE EXCEPTION 'DocumentId cannot be NULL';
    END IF;

    -- If Document is NULL, create it
    IF NEW."Document" IS NULL THEN
        NEW."Document" := jsonb_build_object(
            'Uuid', NEW."DocumentId"::text
        );
    ELSE
        -- Update only if it actually differs
        IF NEW."Document"->>'Uuid' IS DISTINCT FROM NEW."DocumentId"::text THEN
            NEW."Document" := jsonb_set(
                NEW."Document",
                '{Uuid}',
                to_jsonb(NEW."DocumentId"::text),
                true
            );
        END IF;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_sync_document_uuid
BEFORE INSERT OR UPDATE OF "Document", "DocumentId"
ON "Devices"
FOR EACH ROW
EXECUTE FUNCTION sync_document_uuid();