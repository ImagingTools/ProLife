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
		QByteArrayList ids(paramIds.cbegin(), paramIds.cend());

		for (const QByteArray& id : ids){
			if (id == "Groups"){
				const iprm::ISelectionParam* selectionPtr = dynamic_cast<const iprm::ISelectionParam*>(filterParams.GetParameter(id));
				if (selectionPtr != nullptr){
					const iprm::IOptionsList* optionsListPtr = selectionPtr->GetSelectionConstraints();
					if (optionsListPtr != nullptr){
						for (int i = 0; i < optionsListPtr->GetOptionsCount(); i++){
							if (i > 0){
								filterQuery += " OR ";
							}
							QByteArray groupId = optionsListPtr->GetOptionId(i);
							filterQuery += QString("\"Document\"->'Groups' ? '%1'").arg(qPrintable(groupId));
						}
					}
				}
			}
		}
	}

	return true;
}


} // namespace prolifedb


