#include <prolifedb/CDatabaseConverterComp.h>


namespace prolifedb
{


// protected methods

// reimplemented (imtdb::IMigrationController)

bool CDatabaseConverterComp::DoMigration(int& resultRevision, const istd::CIntRange& subRange) const
{
	if (!m_databaseEngineCompPtr.IsValid() ||
		!m_deviceDatabaseDelegateCompPtr.IsValid() ||
		!m_softwareInstanceDatabaseDelegateCompPtr.IsValid() ||
		!m_orderDatabaseDelegateCompPtr.IsValid() ||
		!m_customerDatabaseDelegateCompPtr.IsValid() ||
		!m_deviceBindingDatabaseDelegateCompPtr.IsValid()){
		Q_ASSERT(false);
		return false;
	}
	
	QString query = QString(R"(CREATE TYPE "DocumentState" AS ENUM ('Active', 'InActive', 'Disabled'))");
	
	if (!ExecQuery(query)){
		return false;
	}

	QString deviceTableName = m_deviceDatabaseDelegateCompPtr->GetTableName();
	QString softwaresTableName = m_softwareInstanceDatabaseDelegateCompPtr->GetTableName();
	QString orderTableName = m_orderDatabaseDelegateCompPtr->GetTableName();
	QString customerTableName = m_customerDatabaseDelegateCompPtr->GetTableName();
	QString deviceBindingTableName = m_deviceBindingDatabaseDelegateCompPtr->GetTableName();
	
	if (!RenameTable(deviceTableName, deviceTableName + "_new")){
		return false;
	}
	
	if (!RenameTable(softwaresTableName, softwaresTableName + "_new")){
		return false;
	}
	
	if (!RenameTable(orderTableName, orderTableName + "_new")){
		return false;
	}
	
	if (!RenameTable(customerTableName, customerTableName + "_new")){
		return false;
	}
	
	if (!RenameTable(deviceBindingTableName, deviceBindingTableName + "_new")){
		return false;
	}
	
	if (m_objectCollectionMigrationControllerCompPtr.IsValid()){
		int result = -1;
		istd::CIntRange range = m_objectCollectionMigrationControllerCompPtr->GetMigrationRange();
		if (!m_objectCollectionMigrationControllerCompPtr->DoMigration(result, range)){
			return false;
		}
	}
	
	if (!MigrateTable(deviceTableName, "Device", "\"Document\"->>'MacAddress'", "\"Document\"->>'Description'")){
		return false;
	}
	
	if (!MigrateTable(softwaresTableName, "SoftwareProduct", "\"Document\"->>'SerialNumber'", "''")){
		return false;
	}
	
	if (!MigrateTable(orderTableName, "Order", "\"Document\"->>'OrderId'", "\"Document\"->>'Description'")){
		return false;
	}
	
	if (!MigrateTable(customerTableName, "Account", "\"Document\"->>'Name'", "\"Document\"->>'Description'")){
		return false;
	}
	
	if (!MigrateTable(deviceBindingTableName, "HardwareBinding", "''", "''")){
		return false;
	}
	
	if (!DropTable(deviceTableName + "_new")){
		return false;
	}
	
	if (!DropTable(softwaresTableName + "_new")){
		return false;
	}
	
	if (!DropTable(orderTableName + "_new")){
		return false;
	}
	
	if (!DropTable(customerTableName + "_new")){
		return false;
	}
	
	if (!DropTable(deviceBindingTableName + "_new")){
		return false;
	}
	
	if (!UpdateMetaInfoForTable(*m_deviceBindingDatabaseDelegateCompPtr.GetPtr())){
		return false;
	}
	
	if (!UpdateMetaInfoForTable(*m_deviceDatabaseDelegateCompPtr.GetPtr())){
		return false;
	}
	
	if (!UpdateMetaInfoForTable(*m_softwareInstanceDatabaseDelegateCompPtr.GetPtr())){
		return false;
	}
	
	if (!UpdateMetaInfoForTable(*m_orderDatabaseDelegateCompPtr.GetPtr())){
		return false;
	}
	
	if (!UpdateMetaInfoForTable(*m_customerDatabaseDelegateCompPtr.GetPtr())){
		return false;
	}
	
	QString infoMessage = QString("ProLife tables succesfully migrated");
	SendInfoMessage(0, infoMessage, "CDatabaseConverterComp");
	qDebug() << infoMessage;
	
	resultRevision = GetMigrationRange().GetMaxValue();
	
	return true;
}


// private methods

bool CDatabaseConverterComp::MigrateTable(const QString& tableName, const QByteArray& type, const QString& nameField, const QString& descriptionField) const
{
	QString query = QString(
						R"(
	INSERT INTO public."%1" (
		"Id", "DocumentId", "TypeId", "Name", "Description", "Document", "TimeStamp", "RevisionInfo", "State", "DataMetaInfo"
	)
	SELECT 
		gen_random_uuid(),
		"DocumentId"::UUID,
		'%2',
		%3,
		%4,
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
	FROM public."%1_new";
	)"
	).arg(tableName, qPrintable(type), nameField, descriptionField);

	return ExecQuery(query);
}


bool CDatabaseConverterComp::RenameTable(const QString& tableName, const QString& newTableName) const
{
	QString query = QString(R"(ALTER TABLE public."%1" RENAME TO "%2";)").arg(tableName, newTableName);

	return ExecQuery(query);
}


bool CDatabaseConverterComp::DropTable(const QString& tableName) const
{
	QString query = QString(R"(DROP TABLE public."%1";)").arg(tableName);
	
	return ExecQuery(query);
}


bool CDatabaseConverterComp::UpdateMetaInfoForTable(const imtdb::ISqlDatabaseObjectDelegate& databaseDelegate) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}
	
	QString tableName = databaseDelegate.GetTableName();
	qDebug() << "Update meta info for table" << tableName;
	
	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(QString(R"(SELECT * FROM "%1" WHERE "State" = 'Active';)").arg(tableName).toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		return false;
	}
	
	while (sqlQuery.next()){
		QSqlRecord record = sqlQuery.record();
		
		QByteArray updateMetaInfoQuery = databaseDelegate.CreateUpdateMetaInfoQuery(record);
		updateMetaInfoQuery = updateMetaInfoQuery.replace('\b', ';');
		if (!ExecQuery(updateMetaInfoQuery)){
			return false;
		}
	}
	
	return true;
}


bool CDatabaseConverterComp::ExecQuery(const QString& query) const
{
	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		return false;
	}
	
	return true;
}


} // namespace prolifedb


