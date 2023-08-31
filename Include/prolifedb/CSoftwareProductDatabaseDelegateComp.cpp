#include <prolifedb/CSoftwareProductDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/IEnableableParam.h>
#include <iprm/TParamsPtr.h>

//ImtCore includes
#include <imtlic/CProductInstanceInfo.h>

//ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSoftwareProductDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		QByteArray baseQuery = GetBaseSelectionQuery().toUtf8();

		QByteArray selectionQuery = QString(" AND si.\"DocumentId\" = '%1' ").arg(qPrintable(objectId)).toUtf8();

		selectionQuery = baseQuery + selectionQuery;

		return selectionQuery;
	}

	QByteArray selectionQuery = BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);

	return selectionQuery;
}


QByteArray CSoftwareProductDatabaseDelegateComp::GetObjectIdFromRecord(const QSqlRecord& record) const
{
	if (record.contains("DocumentId")){
		QByteArray documentId = record.value(qPrintable("DocumentId")).toByteArray();

		return documentId;
	}

	return QByteArray();
}


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString CSoftwareProductDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	return R"(SELECT
				 si."DocumentId",
				 si."Document"->>'SerialNumber' as "SerialNumber",
				 si."Document"->>'OrderId' as "OrderUuid",
				 bp."Document"->>'HardwareId'  as "DeviceUuid",
				 ord."Document"->>'OrderId' as "OrderId",
				 acc."Document"->>'Name' as "Customer",
				 acc."DocumentId" as "CustomerUuid",
				 dev."Document"->>'MacAddress'  as "DeviceId",
				 si."Document"->'Licenses'->0->'LicenseData'->>'LicenseId' as "LicenseId",
				 si."Document"->'Licenses'->0->'LicenseData'->>'LicenseName' as "LicenseName",
				 si."Document"->>'InUse' as "InUse",
				 si."Document"->>'ProductId' as "ProductId",
				 si."Document"
			FROM "SoftwareInstances" as si
			LEFT JOIN "BindingProducts" as bp  ON bp."Document"->'SoftwareIds' ? si."DocumentId" AND bp."IsActive"=true
			LEFT JOIN "Devices" as dev ON  dev."IsActive" = true AND dev."DocumentId" = bp."Document"->>'HardwareId'
			LEFT JOIN "Orders" as ord ON ord."DocumentId"=si."Document"->>'OrderId' AND ord."IsActive"=true
			LEFT JOIN "Accounts" as acc ON acc."IsActive" = true AND acc."DocumentId" = ord."Document"->>'OrderCustomer'
			WHERE si."IsActive"=true)";
}


bool CSoftwareProductDatabaseDelegateComp::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (!paramIds.isEmpty()){
#if QT_VERSION >= 0x051500
		QByteArrayList paramIdsList(paramIds.cbegin(), paramIds.cend());
#else+
		QByteArrayList paramIdsList = paramIds.toList();
#endif

		int index = 0;
		for (const QByteArray& key : qAsConst(paramIdsList)){
			if (key.contains('/')){
				continue;
			}

			if (key == "LicenseStatus"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();

				if (value == "None"){
					continue;
				}

				if (index > 0){
					filterQuery += " AND ";
				}

				filterQuery += "bp.\"Document\"->'SoftwareIds'->>0";

				if (value == "WithoutLicense"){
					filterQuery += " is null";
				}
				else{
					filterQuery += " != ''";
				}
				continue;
			}

			if (!filterQuery.isEmpty()){
				if (key == "IncludeIds"){
					filterQuery += " OR ";
				}
				else{
					filterQuery += " AND ";
				}
			}

			index++;
			if (key == "OrderId"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (!value.isEmpty()){
					filterQuery += "si.\"Document\"->>'OrderId' = '";
					filterQuery += value.toUtf8();
					filterQuery += "'";
				}
			}
			else if (key == "CustomerUuid"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (!value.isEmpty()){
					filterQuery += "acc.\"DocumentId\" = '";
					filterQuery += value.toUtf8();
					filterQuery += "'";
				}
			}
			else if (key == "HardwareUuid"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (!value.isEmpty()){
					filterQuery += "bp.\"Document\"->>'HardwareId' = '";
					filterQuery += value.toUtf8();
					filterQuery += "'";
				}
			}
			else if (key == "ProductId"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (!value.isEmpty()){
					filterQuery += "si.\"Document\"->>'ProductId' = '";
					filterQuery += value.toUtf8();
					filterQuery += "'";
				}
			}
			else if (key == "ExcludeIds"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (!value.isEmpty()){
					filterQuery += "si.\"DocumentId\" NOT IN (";
					QStringList keys = value.split(';');
					for (int index = 0; index < keys.count(); index++){
						QString key = keys[index];
						if (index > 0){
							filterQuery += ",";
						}
						filterQuery += "'" + key.toUtf8() + "'";
					}
					filterQuery += ")";
				}
			}
			else if (key == "IncludeIds"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (!value.isEmpty()){
					filterQuery += "si.\"DocumentId\" IN (";
					QStringList keys = value.split(';');
					for (int index = 0; index < keys.count(); index++){
						QString key = keys[index];
						if (index > 0){
							filterQuery += ",";
						}
						filterQuery += "'" + key.toUtf8() + "'";
					}
					filterQuery += ")";
				}
			}
			else if (key == "Orders"){
				const iprm::ISelectionParam* selectionPtr = dynamic_cast<const iprm::ISelectionParam*>(filterParams.GetParameter(key));
				if (selectionPtr != nullptr){
					const iprm::IOptionsList* optionsListPtr = selectionPtr->GetSelectionConstraints();
					if (optionsListPtr != nullptr){
						QString ordersFilterQuery;
						if (optionsListPtr->GetOptionsCount() > 0){
							ordersFilterQuery += "(";
						}

						for (int i = 0; i < optionsListPtr->GetOptionsCount(); i++){
							if (i > 0){
								ordersFilterQuery += " OR ";
							}
							QByteArray orderId = optionsListPtr->GetOptionId(i);
							ordersFilterQuery += QString("si.\"Document\"->>'OrderId' = '%1'").arg(qPrintable(orderId));
						}

						if (!ordersFilterQuery.isEmpty()){
							ordersFilterQuery += ')';
						}

						filterQuery += ordersFilterQuery;
					}
				}
			}
			else if (key == "SerialNumber"){
				iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, key);
				if (filterParamPtr.IsValid()){
					QString value;
					iprm::TParamsPtr<iprm::ITextParam> valueParamPtr(filterParamPtr.GetPtr(), "Value");
					if (valueParamPtr.IsValid()){
						value = valueParamPtr->GetText();
					}

					bool isEqual = true;
					iprm::TParamsPtr<iprm::IEnableableParam> enableableParamPtr(filterParamPtr.GetPtr(), "IsEqual");
					if (enableableParamPtr.IsValid()){
						isEqual = enableableParamPtr->IsEnabled();
					}

					if (isEqual){
						filterQuery += QString("(si.\"Document\"->>'SerialNumber' = '%1')").arg(value);
					}
					else{
						filterQuery += QString("si.\"Document\"->>'SerialNumber' != '%1'").arg(value);
					}
				}
			}
			else if (key == "DeviceId"){
				iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, key);
				if (filterParamPtr.IsValid()){
					QString value;
					iprm::TParamsPtr<iprm::ITextParam> valueParamPtr(filterParamPtr.GetPtr(), "Value");
					if (valueParamPtr.IsValid()){
						value = valueParamPtr->GetText();
					}

					bool isEqual = true;
					iprm::TParamsPtr<iprm::IEnableableParam> enableableParamPtr(filterParamPtr.GetPtr(), "IsEqual");
					if (enableableParamPtr.IsValid()){
						isEqual = enableableParamPtr->IsEnabled();
					}

					if (isEqual){
						filterQuery += "(dev.\"Document\"->>'MacAddress' = '' OR dev.\"Document\"->>'MacAddress' IS NULL)";
					}
					else{
						filterQuery += "dev.\"Document\"->>'MacAddress' != ''";
					}
				}
			}
		}
	}

	return true;
}


bool CSoftwareProductDatabaseDelegateComp::CreateSortQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& sortQuery) const
{
	QByteArray columnId;
	QByteArray sortOrder;

	if (!collectionFilter.GetSortingInfoIds().isEmpty()){
		columnId = collectionFilter.GetSortingInfoIds().first();
	}

	switch (collectionFilter.GetSortingOrder()){
	case imtbase::ICollectionFilter::SO_ASC:
		sortOrder = "ASC";
		break;
	case imtbase::ICollectionFilter::SO_DESC:
		sortOrder = "DESC";
		break;
	case imtbase::ICollectionFilter::SO_NO_ORDER:
		break;
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		if (columnId == "OrderId" || columnId == "DeviceId" || columnId == "Customer"){
			sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
		else{
			sortQuery = QString("ORDER BY si.\"Document\"->>'%1' %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
	}

	return true;
}


bool CSoftwareProductDatabaseDelegateComp::CreateTextFilterQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){
		return true;
	}

	QString textFilter = collectionFilter.GetTextFilter();
	if (!textFilter.isEmpty()){
		for (int i = 0; i < filteringColumnIds.count(); i++){
			if (i > 0){
				textFilterQuery += " OR ";
			}

			QByteArray columnId = filteringColumnIds[i];

			QString shortTableName = "si";
			if (columnId == "OrderId"){
				shortTableName = "ord";
			}
			if (columnId == "DeviceId"){
				shortTableName = "dev";
			}
			if (columnId == "Customer"){
				shortTableName = "acc";
			}
			if (columnId == "DeviceUuid"){
				shortTableName = "bp";
			}

			textFilterQuery += QString("%1.\"Document\"->>'%2' ILIKE '%%3%'").arg(shortTableName).arg(qPrintable(columnId)).arg(textFilter);
		}
	}

	return true;
}


} // namespace prolifedb


