#include <prolifedata/CDeviceDocumentNameProviderComp.h>


// Qt includes
#include <QtCore/QString>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/CDeviceInfo.h>


namespace prolifedata
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CDeviceDocumentNameProviderComp::GetDefaultDocumentName(
	const QByteArray& /*documentId*/,
	const istd::IChangeable& document) const
{
	const prolifedata::IDeviceInfo* deviceInfoPtr =
		dynamic_cast<const prolifedata::IDeviceInfo*>(&document);
	if (deviceInfoPtr == nullptr){
		return QString();
	}

	QByteArray deviceType = deviceInfoPtr->GetDeviceType();
	QByteArray macAddress = deviceInfoPtr->GetMacAddress();

	QString name;

	const prolifedata::COrderedIdentifiableDeviceInfo* identifiableDeviceInfoPtr =
		dynamic_cast<const prolifedata::COrderedIdentifiableDeviceInfo*>(&document);
	if (identifiableDeviceInfoPtr != nullptr && m_objectCollectionCompPtr.IsValid()){
		QByteArray objectId = identifiableDeviceInfoPtr->GetObjectUuid();
		if (!objectId.isEmpty()){
			idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
			if (metaInfoPtr.IsValid()){
				name = metaInfoPtr->GetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_NAME).toString();
			}
		}
	}

	if (name.isEmpty()){
		name = QString::fromUtf8(deviceType);
	}

	if (!macAddress.isEmpty()){
		name += " (" + QString::fromUtf8(macAddress) + ")";
	}

	return name;
}


} // namespace prolifedata


