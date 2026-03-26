UPDATE "SoftwareInstances"
SET "DataMetaInfo" = jsonb_set(
    "DataMetaInfo",
    '{HardwareId}',
    CASE
        WHEN "DataMetaInfo"->>'HardwareId' = ''
            THEN '[]'::jsonb
        ELSE to_jsonb(ARRAY["DataMetaInfo"->>'HardwareId'])
    END,
    false
)
WHERE
    "DataMetaInfo" ? 'HardwareId'
    AND jsonb_typeof("DataMetaInfo"->'HardwareId') = 'string';