INSERT INTO "Roles"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'Role1',
	'{
  "Uuid": "Role1",
  "RoleId": "Role1",
  "RoleName": "Role1",
  "ProductId": "ProLife",
  "Permissions": [],
  "ParentsRoles": [],
  "Restrictions": [],
  "RoleDescription": ""
}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'su');

INSERT INTO "Users"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'su',
	'{
  "Id": "su",
  "Mail": "",
  "Name": "su",
  "Uuid": "su",
  "Groups": [
    "QUISS"
  ],
  "Products": [
    {
      "Roles": [
        "Test1"
      ],
      "ProductId": "ProLife"
    }
  ],
  "Description": "",
  "Permissions": [],
  "PasswordHash": "fbc6f6151c1ac79d3c20990be230c6d3",
  "Restrictions": [],
  "LastConnection": "2024-06-12T05:03:01.172Z"
	}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'su');
	
INSERT INTO "Users"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'UserWithGroup',
	'{
  "Id": "UserWithGroup",
  "Mail": "",
  "Name": "UserWithGroup",
  "Uuid": "UserWithGroup",
  "Groups": [
    "Group1"
  ],
  "Products": [],
  "Description": "",
  "Permissions": [],
  "PasswordHash": "fbc6f6151c1ac79d3c20990be230c6d3",
  "Restrictions": [],
  "LastConnection": "2024-06-12T05:03:01.172Z"
	}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'su');
	
INSERT INTO "Users"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'UserWithoutGroup',
	'{
  "Id": "UserWithoutGroup",
  "Mail": "",
  "Name": "UserWithoutGroup",
  "Uuid": "UserWithoutGroup",
  "Groups": [],
  "Products": [],
  "Description": "",
  "Permissions": [],
  "PasswordHash": "fbc6f6151c1ac79d3c20990be230c6d3",
  "Restrictions": [],
  "LastConnection": "2024-06-12T05:03:01.172Z"
	}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'su');

INSERT INTO "UserGroups"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'Group1',
	'{
  "Id": "Group1",
  "Name": "Group1",
  "Uuid": "Group1",
  "Users": [
	"UserWithGroup"
  ],
  "Products": [],
  "Description": "Group1",
  "Permissions": [],
  "ParentGroups": [],
  "Restrictions": []
	}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'su');
	
INSERT INTO "UserGroups"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'Group2',
	'{
  "Id": "Group2",
  "Name": "Group2",
  "Uuid": "Group2",
  "Users": [],
  "Products": [],
  "Description": "Group2",
  "Permissions": [],
  "ParentGroups": [],
  "Restrictions": []
	}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'su');

INSERT INTO "Accounts"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'AccountWithoutGroup',
	'{
		"Name": "AccountWithoutGroup",
		"Uuid": "AccountWithoutGroup",
		"Email": "information@quiss.com",
		"Groups": [],
		"Picture": {
		"BitmapData": {},
		"BitmapHeader": {
		  "Size": {
			"X": 0,
			"Y": 0
		  },
		  "PixelFormat": 0
		}
		},
		"Addresses": {
		"Addresses": {
		  "ObjectsList": [
			{
			}
		  ]
		}
		},
		"Description": "",
		"SubCompanies": []
	}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'su');
	
INSERT INTO "Accounts"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'AccountWithGroup',
	'{
		"Name": "AccountWithGroup",
		"Uuid": "AccountWithGroup",
		"Email": "information@quiss.com",
		"Groups": ["Group1"],
		"Picture": {
		"BitmapData": {},
		"BitmapHeader": {
		  "Size": {
			"X": 0,
			"Y": 0
		  },
		  "PixelFormat": 0
		}
		},
		"Addresses": {
		"Addresses": {
		  "ObjectsList": [
			{
			}
		  ]
		}
		},
		"Description": "1",
		"SubCompanies": []
	}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'su');
	
INSERT INTO "Orders"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'Order1',
	'{
  "Uuid": "Order1",
  "Status": "inProgress",
  "OrderId": "Order1",
  "Products": {
    "ObjectsList": []
  },
  "PurchaseId": "Order1",
  "Description": "Order1",
  "OrderCustomer": "AccountWithGroup"
	}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'UserWithGroup');

INSERT INTO "Orders"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'Order2',
	'{
  "Uuid": "Order2",
  "Status": "inProgress",
  "OrderId": "Order2",
  "Products": {
    "ObjectsList": []
  },
  "PurchaseId": "Order2",
  "Description": "Order2",
  "OrderCustomer": "AccountWithGroup"
	}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'UserWithGroup');
	
INSERT INTO "Orders"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'Order3',
	'{
  "Uuid": "Order3",
  "Status": "inProgress",
  "OrderId": "Order3",
  "Products": {
    "ObjectsList": []
  },
  "PurchaseId": "Order3",
  "Description": "Order3",
  "OrderCustomer": "AccountWithoutGroup"
	}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'UserWithoutGroup');
	
INSERT INTO "Orders"("DocumentId", "Document", "RevisionNumber", "LastModified", "Checksum", "IsActive", "OwnerId") 
VALUES (
	'Order4',
	'{
  "Uuid": "Order4",
  "Status": "inProgress",
  "OrderId": "Order4",
  "Products": {
    "ObjectsList": []
  },
  "PurchaseId": "Order4",
  "Description": "Order4",
  "OrderCustomer": "AccountWithoutGroup"
	}',
	1,
	CURRENT_TIMESTAMP,
	1,
	true,
	'UserWithoutGroup');
