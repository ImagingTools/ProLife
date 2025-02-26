#include <prolifedb/COrderDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/IEnableableParam.h>
#include <iprm/TParamsPtr.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

bool COrderDatabaseDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (!paramIds.isEmpty()){
#if QT_VERSION >= 0x051500
		QByteArrayList ids(paramIds.cbegin(), paramIds.cend());
#else
		QByteArrayList ids = paramIds.toList();
#endif
		for (const QByteArray& id : ids){
			if (id == "CustomerUuid"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(id));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				filterQuery += QString("(\"Document\"->>'OrderCustomer' = '%1')").arg(value);
			}
			else if (id == "Groups"){
				iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, id);
				if (filterParamPtr.IsValid()){
					QByteArray userId;
					iprm::TParamsPtr<iprm::ITextParam> userParamPtr(filterParamPtr.GetPtr(), "UserParam");
					if (userParamPtr.IsValid()){
						userId = userParamPtr->GetText().toUtf8();
					}

					iprm::TParamsPtr<iprm::ITextParam> textParamPtr(filterParamPtr.GetPtr(), "GroupParam");
					if (textParamPtr.IsValid()){
						QByteArray groups = textParamPtr->GetText().toUtf8();
						QByteArrayList groupIds;
						if (!groups.isEmpty()){
							groupIds = groups.split(';');
						}

						if (!groupIds.isEmpty()){
							QString array = "array[";

							for (int i = 0; i < groupIds.size(); i++){
								if (i > 0){
									array += ",";
								}

								array += "'" + groupIds[i] + "'";
							}

							array += "]";

							if (!filterQuery.isEmpty()){
								filterQuery += " AND ";
							}

							filterQuery += QString(R"((root."Groups" ?| %1))").arg(array);
						}
						else{
							if (!filterQuery.isEmpty()){
								filterQuery += " AND ";
							}

							filterQuery += QString(R"((SELECT ord."OwnerId" FROM "Orders" as ord WHERE ord."DocumentId" = root."DocumentId" AND ord."RevisionNumber" = 1 LIMIT 1) = '%1')").arg(userId);
						}
					}
				}
			}
			else if (id == "OrderId" || id == "PurchaseId"){
				iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, id);
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
						filterQuery += QString("(\"Document\"->>'%1' = '%2')").arg(qPrintable(id)).arg(value);
					}
					else{
						filterQuery += QString("(\"Document\"->>'%1' != '%1')").arg(qPrintable(id)).arg(value);
					}
				}
			}
		}
	}

	return true;
}


} // namespace prolifedb


