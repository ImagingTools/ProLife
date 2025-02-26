#include <prolifedb/CDeviceDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>
#include <iprm/IEnableableParam.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

bool CDeviceDatabaseDelegateComp::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (!paramIds.isEmpty()){
#if QT_VERSION >= 0x051500
		QByteArrayList idsList(paramIds.cbegin(), paramIds.cend());
#else
		QByteArrayList idsList = paramIds.toList();
#endif

		if (idsList.contains("Status")){
			const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter("Status"));
			if (textParamPtr == nullptr){
				return false;
			}

			if (!filterQuery.isEmpty()){
				filterQuery += " AND ";
			}

			QString value = textParamPtr->GetText();
			filterQuery += QString("\"Document\"->>'Status' = '%1'").arg(value);
		}

		for (int i = 0; i < idsList.size(); i++){
			QByteArray key = idsList[i];
			if (key == "ExcludeIds"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				QStringList excludeIds = value.split(';');

				QStringList resultUuids;
				for (const QString& uuid : excludeIds){
					QString result = "'" + uuid + "'";
					resultUuids << result;
				}

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				filterQuery += QString(R"((root."DocumentId" NOT IN (%1)))").arg(resultUuids.join(','));
			}
			else if (key == "ProductUuid"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				filterQuery += QString(R"((root."ProductUuid" = '%1'))").arg(value);
			}
			else if (key == "CustomerUuid"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				filterQuery += QString(R"((root."CustomerUuid" = '%1'))").arg(value);
			}
			else if (key == "LicenseStatus"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();

				if (value == "None"){
					continue;
				}

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				QByteArray countLicenseSql = "(SELECT jsonb_array_length(\"Document\"->'SoftwareIds') FROM \"BindingProducts\" as t3  WHERE t3.\"IsActive\" = true AND t3.\"DocumentId\" = root.\"DocumentId\" )";

				if (value == "WithoutLicense"){
					filterQuery += "(" + countLicenseSql + " IS NULL OR " + countLicenseSql + " = 0)";
				}
				else{
					filterQuery += countLicenseSql + " > 0";
				}
				continue;
			}
			else if (key == "Groups"){
				iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, key);
				if (filterParamPtr.IsValid()){
					QByteArray userId;
					iprm::TParamsPtr<iprm::ITextParam> userParamPtr(filterParamPtr.GetPtr(), "UserParam");
					if (userParamPtr.IsValid()){
						userId = userParamPtr->GetText().toUtf8();
					}

					iprm::TParamsPtr<iprm::ITextParam> textParamPtr(filterParamPtr.GetPtr(), "GroupParam");
					QString groupFilter;

					if (textParamPtr.IsValid()){
						QByteArray groups = textParamPtr->GetText().toUtf8();
						QByteArrayList groupIds;
						if (!groups.isEmpty()){
							groupIds = groups.split(';');
						}

						QString ownerSubquery = QString(R"((SELECT dev."OwnerId" FROM "Devices" as dev WHERE dev."DocumentId" = root."DocumentId" AND dev."RevisionNumber" = 1 LIMIT 1))");

						if (!groupIds.isEmpty()){
							QString array = "array[";

							for (int j = 0; j < groupIds.size(); j++){
								if (j > 0){
									array += ",";
								}

								array += "'" + groupIds[j] + "'";
							}

							array += "]";

							QString groupsQuery = QString(R"((SELECT "Groups" FROM "UsersTemp" WHERE "UserId" = %1))").arg(ownerSubquery);
							groupFilter += QString(R"((root."Groups" ?| %1) OR (root."Document"->>'OrderId' = '' AND (%2 ?| %1)))").arg(array).arg(QString(R"((to_jsonb(string_to_array((%1), ';'))))").arg(groupsQuery));
						}
						else{
							groupFilter += QString(R"(%1 = '%2')").arg(ownerSubquery).arg(userId);
						}

						if (!groupFilter.isEmpty()){
							if (!filterQuery.isEmpty()){
								filterQuery += " AND ";
							}

							filterQuery += "(" + groupFilter + ")";
						}
					}
				}
			}
		}
	}

	return true;
}


} // namespace prolifedb


