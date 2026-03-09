// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ImtCore includes
#include <imtdb/CMigrationControllerCompBase.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>


namespace prolifedb
{


class CDatabaseConverterComp: virtual public imtdb::CMigrationControllerCompBase
{
public:
	typedef imtdb::CMigrationControllerCompBase BaseClass;
	
	I_BEGIN_COMPONENT(CDatabaseConverterComp)
		I_ASSIGN(m_deviceDatabaseDelegateCompPtr, "DeviceDatabaseDelegate", "Device database delegate", true, "DeviceDatabaseDelegate");
		I_ASSIGN(m_deviceBindingDatabaseDelegateCompPtr, "DeviceBindingDatabaseDelegate", "Device binding database delegate", true, "DeviceBindingDatabaseDelegate");
		I_ASSIGN(m_softwareInstanceDatabaseDelegateCompPtr, "SoftwareInstanceDatabaseDelegate", "Software database delegate", true, "SoftwareInstanceDatabaseDelegate");
		I_ASSIGN(m_orderDatabaseDelegateCompPtr, "OrderDatabaseDelegate", "Order database delegate", true, "OrderDatabaseDelegate");
		I_ASSIGN(m_customerDatabaseDelegateCompPtr, "CustomerDatabaseDelegate", "Customer database delegate", true, "CustomerDatabaseDelegate");
		I_ASSIGN(m_objectCollectionMigrationControllerCompPtr, "ObjectCollectionMigrationController", "Object collection migration controller", true, "ObjectCollectionMigrationController");
	I_END_COMPONENT

protected:
	// reimplemented (imtdb::IMigrationController)
	virtual bool DoMigration(int& resultRevision, const istd::CIntRange& subRange = istd::CIntRange()) const override;
	
private:
	bool MigrateTable(
		const QString& tableName,
		const QByteArray& type,
		const QString& nameField,
		const QString& descriptionField,
		bool updateUuid = true) const;
	bool RenameTable(const QString& tableName, const QString& newTableName) const;
	bool DropTable(const QString& tableName) const;
	bool UpdateMetaInfoForTable(const imtdb::ISqlDatabaseObjectDelegate& databaseDelegate) const;
	bool UpdateOrderLinkInfo(const imtdb::ISqlDatabaseObjectDelegate& databaseDelegate) const;
	bool ExecQuery(const QString& query) const;
	
private:
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_deviceDatabaseDelegateCompPtr);
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_softwareInstanceDatabaseDelegateCompPtr);
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_orderDatabaseDelegateCompPtr);
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_customerDatabaseDelegateCompPtr);
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_deviceBindingDatabaseDelegateCompPtr);
	I_REF(imtdb::IMigrationController, m_objectCollectionMigrationControllerCompPtr);
};


} // namespace prolifedb


