#include <prolifegql/CDeviceCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

bool CDeviceCollectionControllerComp::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const QByteArray& collectionId,
		QString& errorMessage) const
{
	bool retVal = true;

	gqlRequest.GetFields();
	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		prolifedata::IDeviceInfo* deviceInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(dataPtr.GetPtr());
		}

		if (deviceInfoPtr != nullptr){
			for (QByteArray informationId : informationIds){
				QVariant elementInformation;

				if(informationId == "TypeId"){
					elementInformation = m_objectCollectionCompPtr->GetObjectTypeId(collectionId);
				}
				else if(informationId == "Id" || informationId == "Name"){
					elementInformation = deviceInfoPtr->GetSerialNumber();
				}
				else if(informationId == "MacAddress"){
					elementInformation = deviceInfoPtr->GetMacAddress();
				}
				else if(informationId == "SerialNumber"){
					elementInformation = deviceInfoPtr->GetSerialNumber();
				}
				else if(informationId == "Description"){
					elementInformation = deviceInfoPtr->GetDescription();
				}
				else if(informationId == "Status"){
					int status = deviceInfoPtr->GetDeviceStatus();
					if (status == prolifedata::IDeviceInfo::OS_CREATED){
						elementInformation = "Created";
					}
					else if (status == prolifedata::IDeviceInfo::OS_IN_PROGRESS){
						elementInformation = "InProgress";
					}
					else if (status == prolifedata::IDeviceInfo::OS_FINISHED){
						elementInformation = "Finished";
					}
					else{
						elementInformation = "None";
					}
				}

				if (elementInformation.isNull()){
					elementInformation = "";
				}

				retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
			}

			return true;
		}
	}

	return false;
}


imtbase::CTreeItemModel* CDeviceCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return nullptr;
}


} // namespace prolifegql


