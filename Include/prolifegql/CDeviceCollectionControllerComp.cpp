#include <prolifegql/CDeviceCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CDeviceCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return QVariant();
	}

	idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
	if (metaInfoPtr.IsValid()){
		if (informationId == QByteArray("MacAddress")){
			return metaInfoPtr->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_MAC_ADDRESS);
		}
		else if (informationId == QByteArray("SerialNumber")){
			return metaInfoPtr->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_SERIAL_NUMBER);
		}
		else if (informationId == QByteArray("Status")){
			return metaInfoPtr->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_STATUS);
		}
		else if (informationId == QByteArray("Description")){
			return metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CDeviceCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return nullptr;
}


} // namespace prolifegql


