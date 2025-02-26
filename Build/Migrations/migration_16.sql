INSERT INTO public."Devices" (
    "Id", "DocumentId", "TypeId", "Name", "Description", "Document", "TimeStamp", "RevisionInfo", "State", "DataMetaInfo"
)
SELECT 
    gen_random_uuid(),
    "DocumentId"::UUID,
    'Device',
	"Document"->>'MacAddress',
	"Document"->>'Description',
    "Document",
    COALESCE("LastModified", now()),
    jsonb_build_object(
        'OwnerId', COALESCE("OwnerId", ''),
        'OwnerName', COALESCE("OwnerName", ''),
        'OperationDescription', COALESCE("OperationDescription", ''),
        'RevisionNumber', "RevisionNumber",
        'Checksum', "Checksum"
    ),
    CASE 
        WHEN "IsActive" = TRUE THEN 'Active'
        WHEN "IsActive" = FALSE THEN 'InActive'
        ELSE 'Disabled'
    END::"DocumentState",
    "Document"
FROM public."Devices_new";
DROP TABLE public."Devices_new";


INSERT INTO public."SoftwareInstances" (
    "Id", "DocumentId", "TypeId", "Name", "Description", "Document", "TimeStamp", "RevisionInfo", "State", "DataMetaInfo"
)
SELECT 
    gen_random_uuid(), 
    "DocumentId"::UUID,
	'SoftwareProduct',
	"Document"->>'SerialNumber',
	'',
    "Document",
    COALESCE("LastModified", now()),
    jsonb_build_object(
        'OwnerId', COALESCE("OwnerId", ''),
        'OwnerName', COALESCE("OwnerName", ''),
        'OperationDescription', COALESCE("OperationDescription", ''),
        'RevisionNumber', "RevisionNumber",
        'Checksum', "Checksum"
    ),
    CASE 
        WHEN "IsActive" = TRUE THEN 'Active'
        WHEN "IsActive" = FALSE THEN 'InActive'
        ELSE 'Disabled'
    END::"DocumentState",
    "Document"
FROM public."SoftwareInstances_new";
DROP TABLE public."SoftwareInstances_new";


INSERT INTO public."Orders" (
    "Id", "DocumentId", "TypeId", "Name", "Description", "Document", "TimeStamp", "RevisionInfo", "State", "DataMetaInfo"
)
SELECT 
    gen_random_uuid(),
    "DocumentId"::UUID,
	'Order',
	"Document"->>'OrderId',
	"Document"->>'Description',
    "Document",
    COALESCE("LastModified", now()),
    jsonb_build_object(
        'OwnerId', COALESCE("OwnerId", ''),
        'OwnerName', COALESCE("OwnerName", ''),
        'OperationDescription', COALESCE("OperationDescription", ''),
        'RevisionNumber', "RevisionNumber",
        'Checksum', "Checksum"
    ),
    CASE 
        WHEN "IsActive" = TRUE THEN 'Active'
        WHEN "IsActive" = FALSE THEN 'InActive'
        ELSE 'Disabled'
    END::"DocumentState",
       "Document"
FROM public."Orders_new";
DROP TABLE public."Orders_new";

INSERT INTO public."Accounts" (
    "Id", "DocumentId", "TypeId", "Name", "Description", "Document", "TimeStamp", "RevisionInfo", "State", "DataMetaInfo"
)
SELECT 
    gen_random_uuid(),
    "DocumentId"::UUID,
	'Account',
	"Document"->>'Name',
	"Document"->>'Description',
    "Document",
    COALESCE("LastModified", now()),
    jsonb_build_object(
        'OwnerId', COALESCE("OwnerId", ''),
        'OwnerName', COALESCE("OwnerName", ''),
        'OperationDescription', COALESCE("OperationDescription", ''),
        'RevisionNumber', "RevisionNumber",
        'Checksum', "Checksum"
    ),
    CASE 
        WHEN "IsActive" = TRUE THEN 'Active'
        WHEN "IsActive" = FALSE THEN 'InActive'
        ELSE 'Disabled'
    END::"DocumentState",
        "Document"
FROM public."Accounts_new";
DROP TABLE public."Accounts_new";

INSERT INTO public."BindingProducts" (
    "Id", "DocumentId", "TypeId", "Name", "Description", "Document", "TimeStamp", "RevisionInfo", "State", "DataMetaInfo"
)
SELECT 
    gen_random_uuid(),
    "DocumentId"::UUID,
	'HardwareBinding',
	'',
	'',
    "Document",
    COALESCE("LastModified", now()),
    jsonb_build_object(
        'OwnerId', COALESCE("OwnerId", ''),
        'OwnerName', COALESCE("OwnerName", ''),
        'OperationDescription', COALESCE("OperationDescription", ''),
        'RevisionNumber', "RevisionNumber",
        'Checksum', "Checksum"
    ),
    CASE 
        WHEN "IsActive" = TRUE THEN 'Active'
        WHEN "IsActive" = FALSE THEN 'InActive'
        ELSE 'Disabled'
    END::"DocumentState",
        "Document"
FROM public."BindingProducts_new";
DROP TABLE public."BindingProducts_new";