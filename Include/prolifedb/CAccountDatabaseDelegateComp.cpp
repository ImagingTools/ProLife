#include <prolifedb/CAccountDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>


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
				const iprm::ISelectionParam* selectionPtr = dynamic_cast<const iprm::ISelectionParam*>(filterParams.GetParameter(id));
				if (selectionPtr != nullptr){
					const iprm::IOptionsList* optionsListPtr = selectionPtr->GetSelectionConstraints();
					if (optionsListPtr != nullptr){
						QString groupsQuery;

						int optionCount = optionsListPtr->GetOptionsCount();
						if (optionCount == 0){
							groupsQuery = QString("\"Document\"->'Groups' = '[]'");
						}
						else {
							for (int i = 0; i < optionCount; i++){
								if (i > 0){
									groupsQuery += " OR ";
								}
								QByteArray groupId = optionsListPtr->GetOptionId(i);
								groupsQuery += QString("\"Document\"->'Groups' ? '%1'").arg(qPrintable(groupId));
							}
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


