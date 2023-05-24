CREATE INDEX "OrderDocumentIdIsActiveIndex" ON "Orders" ("DocumentId", "IsActive");
CREATE INDEX "DeviceRevisionNumberDocumentIdIndex" ON "Devices" ("RevisionNumber", "DocumentId");
CREATE INDEX "OrderRevisionNumberDocumentIdIndex" ON "Orders" ("RevisionNumber", "DocumentId");
CREATE INDEX "DeviceDocumentIdIndex" ON "Devices" ("DocumentId");
CREATE INDEX "OrderDocumentIdIndex" ON "Orders" ("DocumentId");
CREATE INDEX "AccountDocumentIdIsActiveIndex" ON "Accounts" ("DocumentId", "IsActive");