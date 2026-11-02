#include <prolifedata/CDeviceDocumentNameProviderComp.h>


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtdoc/imtdoc.h>

// ProLife includes
#include <prolifedata/CDeviceInfo.h>


namespace prolifedata
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CDeviceDocumentNameProviderComp::GetDefaultDocumentName(
			const istd::IChangeable& document,
			const iprm::IParamsSet* paramsPtr) const
{
	const prolifedata::IDeviceInfo* deviceInfoPtr =
		dynamic_cast<const prolifedata::IDeviceInfo*>(&document);
	if (deviceInfoPtr == nullptr){
		return QString();
	}

	// The product name is read from the stored device; a document never saved has none
	const QByteArray objectId = imtdoc::GetObjectIdFromParams(paramsPtr);
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


