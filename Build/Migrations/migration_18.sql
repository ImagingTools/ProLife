UPDATE "SoftwareInstances"
SET "DataMetaInfo" = jsonb_set(
    "DataMetaInfo",
    '{HardwareId}',
    to_jsonb(ARRAY["DataMetaInfo"->>'HardwareId']),
    false
)
WHERE
    "DataMetaInfo" ? 'HardwareId'
    AND jsonb_typeof("DataMetaInfo"->'HardwareId') = 'string';