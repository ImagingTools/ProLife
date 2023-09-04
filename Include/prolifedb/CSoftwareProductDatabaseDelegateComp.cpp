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
				 si."Document"->>'Project' as "Project",
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

		QByteArray operation = " AND ";
		int index = 0;
		for (const QByteArray& key : qAsConst(paramIdsList)){

			if (key.contains('/')){
				continue;
			}

			QString elementFilter;
			if (key == "LicenseStatus"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();

				if (value == "None"){
					continue;
				}

//				if (index > 0){
//					filterQuery += " AND ";
//				}

				elementFilter += "bp.\"Document\"->'SoftwareIds'->>0";

				if (value == "WithoutLicense"){
					elementFilter += " is null";
				}
				else{
					elementFilter += " != ''";
				}
//				continue;
			}

//			if (!filterQuery.isEmpty()){
//				if (key == "IncludeIds"){
//					filterQuery += " OR ";
//				}
//				else{
//					filterQuery += " AND ";
//				}
//			}

//			index++;
			else if (key == "OrderId"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (!value.isEmpty()){
					elementFilter += "si.\"Document\"->>'OrderId' = '";
					elementFilter += value.toUtf8();
					elementFilter += "'";
				}
			}
			else if (key == "CustomerUuid"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (!value.isEmpty()){
					elementFilter += "acc.\"DocumentId\" = '";
					elementFilter += value.toUtf8();
					elementFilter += "'";
				}
			}
			else if (key == "HardwareUuid"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (!value.isEmpty()){
					elementFilter += "bp.\"Document\"->>'HardwareId' = '";
					elementFilter += value.toUtf8();
					elementFilter += "'";
				}
			}
			else if (key == "ProductId"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (!value.isEmpty()){
					elementFilter += "si.\"Document\"->>'ProductId' = '";
					elementFilter += value.toUtf8();
					elementFilter += "'";
				}
			}
			else if (key == "FilterIds"){
				iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, key);
				if (filterParamPtr.IsValid()){
					QString excludeText;
					iprm::TParamsPtr<iprm::ITextParam> excludeTextParamPtr(filterParamPtr.GetPtr(), "ExcludeIds");
					if (excludeTextParamPtr.IsValid()){
						excludeText = excludeTextParamPtr->GetText();
					}

					QString includeText;
					iprm::TParamsPtr<iprm::ITextParam> includeTextParamPtr(filterParamPtr.GetPtr(), "IncludeIds");
					if (includeTextParamPtr.IsValid()){
						includeText = includeTextParamPtr->GetText();
					}

					QStringList excludeIds;
					if (!excludeText.isEmpty()){
						elementFilter += "si.\"DocumentId\" NOT IN (";
						QStringList keys = excludeText.split(';');
						excludeIds = keys;
						for (int index = 0; index < keys.count(); index++){
							QString key = keys[index];
							if (index > 0){
								elementFilter += ",";
							}
							elementFilter += "'" + key.toUtf8() + "'";
						}
						elementFilter += ")";

						QString deviceFilter = QString(" AND (dev.\"Document\"->>'MacAddress' = '' OR dev.\"Document\"->>'MacAddress' IS NULL)");
						elementFilter += deviceFilter;
					}

					QStringList includeIds;
					if (!includeText.isEmpty()){

						QString suffixFilter;
						if (!elementFilter.isEmpty()){
							elementFilter += " OR ";
						}

						elementFilter += "si.\"DocumentId\" IN (";
						QStringList keys = includeText.split(';');
						includeIds = keys;
						for (int index = 0; index < keys.count(); index++){
							QString key = keys[index];
							if (index > 0){
								elementFilter += ",";
							}
							elementFilter += "'" + key.toUtf8() + "'";
						}
						elementFilter += ")";
					}

					elementFilter = "(" + elementFilter + ")";

//					if (!excludeText.isEmpty()){
//						QStringList keys;

//						for (const QString& key : excludeIds){
//							if (!includeIds.contains(key)){
//								keys << key;
//							}
//						}

//						for (const QString& key : keys){
//							QString query = QString("si.\"Document\"->'Licenses'->0->'LicenseData'->>'LicenseId' != (SELECT \"Document\"->'Licenses'->0->'LicenseData'->>'LicenseId' as \"LicenseId\" FROM \"SoftwareInstances\" WHERE \"IsActive\" = true AND \"DocumentId\" = '%1' LIMIT 1)").arg(key);

//							elementFilter += " AND " + query;
//						}
//					}
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

						elementFilter += ordersFilterQuery;
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
						elementFilter += QString("(si.\"Document\"->>'SerialNumber' = '%1')").arg(value);
					}
					else{
						elementFilter += QString("si.\"Document\"->>'SerialNumber' != '%1'").arg(value);
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
						elementFilter += "(dev.\"Document\"->>'MacAddress' = '' OR dev.\"Document\"->>'MacAddress' IS NULL)";
					}
					else{
						elementFilter += "dev.\"Document\"->>'MacAddress' != ''";
					}
				}
			}

			if (filterQuery.isEmpty()){
				filterQuery = elementFilter;
			}
			else{
				filterQuery += operation + elementFilter;
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
		if (columnId == "LicenseId"){
			sortQuery = QString("ORDER BY si.\"Document\"->'Licenses'->0->'LicenseData'->>'LicenseId' %1").arg(qPrintable(sortOrder));
		}
		else if (columnId == "OrderId" || columnId == "DeviceId" || columnId == "Customer"){
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
			else if (columnId == "DeviceId"){
				shortTableName = "dev";
			}
			else if (columnId == "Customer"){
				shortTableName = "acc";
			}
			else if (columnId == "DeviceUuid"){
				shortTableName = "bp";
			}
			else if (columnId == "LicenseId"){
				textFilterQuery += QString("si.\"Document\"->'Licenses'->0->'LicenseData'->>'LicenseId' ILIKE '%%1%'").arg(textFilter);

				continue;
			}

			textFilterQuery += QString("%1.\"Document\"->>'%2' ILIKE '%%3%'").arg(shortTableName).arg(qPrintable(columnId)).arg(textFilter);
		}
	}

	return true;
}


} // namespace prolifedb


