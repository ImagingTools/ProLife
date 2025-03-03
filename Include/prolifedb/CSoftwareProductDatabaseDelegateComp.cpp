#include <prolifedb/CSoftwareProductDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

bool CSoftwareProductDatabaseDelegateComp::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (!paramIds.isEmpty()){
#if QT_VERSION >= 0x051500
		QByteArrayList paramIdsList(paramIds.cbegin(), paramIds.cend());
#else
		QByteArrayList paramIdsList = paramIds.toList();
#endif

		if (paramIdsList.contains("BindingFilter")){
			iprm::TParamsPtr<iprm::IParamsSet> bindingFilterParamPtr(&filterParams, "BindingFilter");
			if (bindingFilterParamPtr.IsValid()){
				iprm::TParamsPtr<iprm::IIdParam> hardwareUuidParamPtr(bindingFilterParamPtr.GetPtr(), "HardwareUuid");
				if (hardwareUuidParamPtr.IsValid()){
					QString value = hardwareUuidParamPtr->GetId();
					QString filter = QString(R"((bp."Document"->>'HardwareId' = '%1'))").arg(value);

					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}

					filterQuery += filter;
				}

				iprm::TParamsPtr<iprm::IIdParam> hardwareUuidFilterParamPtr(bindingFilterParamPtr.GetPtr(), "HardwareUuidFilter");
				if (hardwareUuidFilterParamPtr.IsValid()){
					QString value = hardwareUuidFilterParamPtr->GetId();
					QString elementFilter = QString(R"((bp."Document"->>'HardwareId' = '' OR bp."Document"->>'HardwareId' IS NULL OR bp."Document"->>'HardwareId' = '%1'))").arg(value);

					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}

					filterQuery += elementFilter;
				}

				iprm::TParamsPtr<iprm::IIdParam> deviceIdFilterParamPtr(bindingFilterParamPtr.GetPtr(), "DeviceId");
				if (deviceIdFilterParamPtr.IsValid()){
					QString value = deviceIdFilterParamPtr->GetId();

					QString elementFilter = QString(R"(((dev."Document"->>'MacAddress' = '' OR dev."Document"->>'MacAddress' IS NULL)))");

					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}

					filterQuery += elementFilter;
				}

				iprm::TParamsPtr<iprm::IIdParam> productUuidFilterParamPtr(bindingFilterParamPtr.GetPtr(), "ProductUuid");
				if (productUuidFilterParamPtr.IsValid()){
					QString value = productUuidFilterParamPtr->GetId();
					QString productFilter = QString(R"((root."Document"->>'ProductId' = '%1'))").arg(value);

					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}

					filterQuery += productFilter;
				}

				iprm::TParamsPtr<iprm::IIdParam> customerUuidFilterParamPtr(bindingFilterParamPtr.GetPtr(), "CustomerUuid");
				if (customerUuidFilterParamPtr.IsValid()){
					QString value = customerUuidFilterParamPtr->GetId();
					QString accountFilter = QString(R"((acc."DocumentId" = '%1'))").arg(value);

					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}

					filterQuery += accountFilter;
				}

				iprm::TParamsPtr<iprm::IIdParam> excludeFilterParamPtr(bindingFilterParamPtr.GetPtr(), "ExcludeUuids");
				if (excludeFilterParamPtr.IsValid()){
					QString value = excludeFilterParamPtr->GetId();

					if (!value.isEmpty()){
						QStringList uuids = value.split(';');

						QStringList resultUuids;
						for (const QString& uuid : uuids){
							QString result = "'" + uuid + "'";
							resultUuids << result;
						}

						QString excludeFilter = QString(R"((root."DocumentId" NOT IN (%1)))").arg(resultUuids.join(','));

						if (!filterQuery.isEmpty()){
							filterQuery += " AND ";
						}

						filterQuery += excludeFilter;
					}
				}

				iprm::TParamsPtr<iprm::IIdParam> licenseIdsFilterParamPtr(bindingFilterParamPtr.GetPtr(), "LicenseIds");
				if (licenseIdsFilterParamPtr.IsValid()){
					QString value = licenseIdsFilterParamPtr->GetId();

					if (!value.isEmpty()){
						QStringList uuids = value.split(';');

						QStringList resultUuids;
						for (const QString& uuid : uuids){
							QString result = "'" + uuid + "'";
							resultUuids << result;
						}

						QString excludeFilter = QString(R"(((SELECT lic."LicenseId" FROM "LicensesTemp" as lic WHERE lic."DocumentId" = root."Document"->'Licenses'->0->'LicenseData'->>'LicenseId') NOT IN (%1)))").arg(resultUuids.join(','));

						if (!filterQuery.isEmpty()){
							filterQuery += " AND ";
						}

						filterQuery += excludeFilter;
					}
				}
			}
		}

		if (paramIdsList.contains("Groups")){
			iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, "Groups");
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

					QString ownerSubquery = QString(R"((SELECT sof."OwnerId" FROM "SoftwareInstances" as sof WHERE sof."DocumentId" = root."DocumentId" AND sof."RevisionNumber" = 1 LIMIT 1))");

					if (!groupIds.isEmpty()){
						QString array = "array[";

						for (int i = 0; i < groupIds.size(); i++){
							if (i > 0){
								array += ",";
							}

							array += "'" + groupIds[i] + "'";
						}

						array += "]";

						QString groupsQuery = QString(R"((SELECT "Groups" FROM "UsersTemp" WHERE "UserId" = %1))").arg(ownerSubquery);
						groupFilter += QString(R"((acc."Document"->'Groups' ?| %1) OR (root."Document"->>'OrderId' = '' AND (%2 ?| %1)))").arg(array).arg(QString(R"((to_jsonb(string_to_array((%1), ';'))))").arg(groupsQuery));
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

		if (!filterQuery.isEmpty()){
			filterQuery = "(" + filterQuery + ")";
		}
	}

	return true;
}


} // namespace prolifedb


