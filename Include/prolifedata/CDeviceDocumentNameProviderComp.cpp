#include <prolifedata/CDeviceDocumentNameProviderComp.h>


// Qt includes
#include <QtCore/QString>

// ProLife includes
#include <prolifedata/CDeviceInfo.h>


namespace prolifedata
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CDeviceDocumentNameProviderComp::GetDefaultDocumentName(
			const QByteArray& objectId,
			const istd::IChangeable& document) const
{
	const prolifedata::IDeviceInfo* deviceInfoPtr =
		dynamic_cast<const prolifedata::IDeviceInfo*>(&document);
	if (deviceInfoPtr == nullptr){
		return QString();
	}

	if (objectId.isEmpty()){
		return QString();
	}

	QString productName;

	idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
	if (metaInfoPtr.IsValid()){
		productName = metaInfoPtr->GetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_NAME).toString();
	}

	QString name;
	QByteArray macAddress = deviceInfoPtr->GetMacAddress();

	if (productName.isEmpty() && !macAddress.isEmpty()){
		name = macAddress;
	}

	if (!productName.isEmpty() && macAddress.isEmpty()){
		name = productName;
	}

	if (!productName.isEmpty() && !macAddress.isEmpty()){
		name = productName + " (" + QString::fromUtf8(macAddress) + ")";
	}

	return name;
}


} // namespace prolifedata


