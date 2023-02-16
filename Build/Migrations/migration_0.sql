CREATE TABLE "Users"(
    UserId VARCHAR (1000) NOT NULL,
	Password VARCHAR (1000) NOT NULL,
	Name VARCHAR (1000) NOT NULL,
	Email VARCHAR (1000) NOT NULL,
	Description VARCHAR (1000),
	Added TIMESTAMP,
	LastModified TIMESTAMP,
	PRIMARY KEY (UserId)
);

CREATE TYPE ActionType AS ENUM ('Logout', 'Login', 'PasswordChange', 'NameChange');

CREATE TABLE "UserActionLog"(
    Id UUID NOT NULL,
    UserId VARCHAR (1000) NOT NULL,
    Action ActionType, 
    ActionTime TIMESTAMP,
    PRIMARY KEY (Id),
    FOREIGN KEY (UserId) REFERENCES "Users"(UserId) ON DELETE CASCADE ON UPDATE CASCADE	
);

CREATE TABLE "Roles"(
    RoleId VARCHAR (1000) NOT NULL,
    ProductId VARCHAR (1000) NOT NULL,
    Name VARCHAR (1000) NOT NULL,
    Description VARCHAR (1000),
    PRIMARY KEY (RoleId, ProductId)
);

CREATE TABLE "ParentRoles"(
    RoleId VARCHAR (1000) NOT NULL,
    ProductId VARCHAR (1000) NOT NULL,
    ParentRoleId VARCHAR (1000) NOT NULL,
    ParentProductId VARCHAR (1000) NOT NULL,
    PRIMARY KEY (RoleId, ProductId, ParentRoleId, ParentProductId),
    FOREIGN KEY (RoleId, ProductId) REFERENCES "Roles"(RoleId, ProductId) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (ParentRoleId, ParentProductId) REFERENCES "Roles"(RoleId, ProductId) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE "UserGroups"(
    UserGroupId VARCHAR (1000) NOT NULL,
    Name VARCHAR (1000) NOT NULL,
    Description VARCHAR (1000),
    PRIMARY KEY (UserGroupId)
);

CREATE TABLE "GroupUsers"(
    GroupId VARCHAR (1000) NOT NULL,
    UserId VARCHAR (1000) NOT NULL,
    PRIMARY KEY (GroupId, UserId),
    FOREIGN KEY (GroupId) REFERENCES "UserGroups"(UserGroupId) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (UserId) REFERENCES "Users"(UserId) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE "ParentGroups"(
    UserGroupId VARCHAR (1000) NOT NULL,
    ParentUserGroupId VARCHAR (1000) NOT NULL,
    PRIMARY KEY (UserGroupId, ParentUserGroupId),
    FOREIGN KEY (UserGroupId) REFERENCES "UserGroups"(UserGroupId) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (ParentUserGroupId) REFERENCES "UserGroups"(UserGroupId) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE "RolePermissions"(
    RoleId VARCHAR (1000) NOT NULL,
    ProductId VARCHAR (1000) NOT NULL,
    PermissionId VARCHAR (1000) NOT NULL,
    PermissionState BOOLEAN NOT NULL,
    PRIMARY KEY (RoleId, ProductId, PermissionId),
    FOREIGN KEY (RoleId, ProductId) REFERENCES "Roles"(RoleId, ProductId) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE "UserPermissions"(
    UserId VARCHAR (1000) NOT NULL,
    PermissionId VARCHAR (1000) NOT NULL,
    PRIMARY KEY (UserId, PermissionId),
    FOREIGN KEY (UserId) REFERENCES "Users"(UserId) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE "UserRoles"(
    UserId VARCHAR (1000) NOT NULL,
    RoleId VARCHAR (1000) NOT NULL,
    ProductId VARCHAR (1000) NOT NULL,
    PRIMARY KEY (UserId, RoleId, ProductId),
    FOREIGN KEY (RoleId, ProductId) REFERENCES "Roles" (RoleId, ProductId) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (UserId) REFERENCES "Users" (UserId) ON DELETE CASCADE ON UPDATE CASCADE
);


CREATE TABLE "UsersSettings"(
    UserId VARCHAR (1000) NOT NULL,
    Settings TEXT NOT NULL,
    PRIMARY KEY (UserId),
    FOREIGN KEY (UserId) REFERENCES "Users" (UserId) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE "UsersSessions"(
    AccessToken UUID NOT NULL,
    UserId VARCHAR (1000) NOT NULL,
    LastActivity TIMESTAMP NOT NULL,
    PRIMARY KEY (AccessToken),
    FOREIGN KEY (UserId) REFERENCES "Users" (UserId) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TYPE AccountType AS ENUM ('private', 'company');

CREATE TABLE "Accounts"(
	Id VARCHAR (1000) NOT NULL,
	Name VARCHAR (1000) NOT NULL,
	Description VARCHAR (1000),
	Type AccountType NOT NULL,
	OwnerMail VARCHAR (1000) NOT NULL,
	OwnerFirstName VARCHAR (1000) NOT NULL,
	OwnerLastName VARCHAR (1000) NOT NULL,
    PRIMARY KEY (Id)
);

CREATE TABLE "Orders"(
    Id SERIAL,
    OrderId VARCHAR (1000) NOT NULL,
    AccountId VARCHAR (1000) NOT NULL,
    Document JSONB,
    RevisionNumber BIGINT,
    LastModified TIMESTAMP,
    Checksum BIGINT,
    IsActive BOOLEAN,
    PRIMARY KEY (Id),
    FOREIGN KEY (AccountId) REFERENCES "Accounts"(Id) ON DELETE CASCADE ON UPDATE CASCADE
);

