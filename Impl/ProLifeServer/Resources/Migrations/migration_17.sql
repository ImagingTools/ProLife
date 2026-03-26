UPDATE "Devices"
SET "DataMetaInfo" =
    (
        jsonb_set(
            "DataMetaInfo" - 'SoftwareCount',
            '{InUse}',
            CASE
                WHEN ("DataMetaInfo"->>'SoftwareCount') ~ '^\d+$'
                     AND ("DataMetaInfo"->>'SoftwareCount')::int > 0
                THEN 'true'::jsonb
                ELSE 'false'::jsonb
            END,
            true
        )
    )
WHERE
    "DataMetaInfo" ? 'SoftwareCount';