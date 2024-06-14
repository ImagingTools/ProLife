#include <prolifedb/CAccountDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

bool CAccountDatabaseDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (!paramIds.isEmpty()){
#if QT_VERSION >= 0x051500
		QByteArrayList ids(paramIds.cbegin(), paramIds.cend());
#else
		QByteArrayList ids = paramIds.toList();
#endif
		for (const QByteArray& id : ids){
			if (id == "Groups"){
				iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, "Groups");
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

						QString groupsQuery;

						if (groupIds.isEmpty()){
							groupsQuery = QString("\"Document\"->'Groups' = '[]'");
						}
						else {
							for (int i = 0; i < groupIds.size(); i++){
								if (i > 0){
									groupsQuery += " OR ";
								}

								groupsQuery += QString("\"Document\"->'Groups' ? '%1'").arg(qPrintable(groupIds[i]));
							}

							QString ownerSubquery = QString(R"((SELECT acc."OwnerId" FROM "Accounts" as acc WHERE acc."DocumentId" = t2."DocumentId" AND acc."RevisionNumber" = 1 LIMIT 1))");

							groupsQuery += QString(R"( OR (%1 = '%2'))").arg(ownerSubquery).arg(userId);
						}

						if (!groupsQuery.isEmpty()){
							filterQuery = '(' + groupsQuery + ')';
						}
					}
				}
			}
		}
	}

	return true;
}


} // namespace prolifedb


