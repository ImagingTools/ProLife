#include <prolifegql/CHardwareBindingChangeGeneratorComp.h>


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/ILicenseDefinition.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>


namespace prolifegql
{


// protected methods

bool CHardwareBindingChangeGeneratorComp::CompareDocuments(
			const istd::IChangeable* oldDocumentPtr,
			const istd::IChangeable* newDocumentPtr,
			imtbase::CObjectCollection& documentChangeCollection,
			QString& errorMessage)
{
	const prolifedata::CHardwareProductBinding* oldHardwareBindingInfoPtr = dynamic_cast<const prolifedata::CHardwareProductBinding*>(oldDocumentPtr);
	if (oldHardwareBindingInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. Old document is invalid");

		return false;
	}

	const prolifedata::CHardwareProductBinding* newHardwareBindingInfoPtr = dynamic_cast<const prolifedata::CHardwareProductBinding*>(newDocumentPtr);
	if (newHardwareBindingInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. New document is invalid");

		return false;
	}

	oldHardwareBindingInfoPtr->GetHardwareId();
	newHardwareBindingInfoPtr->GetHardwareId();

	QByteArrayList oldSoftwareIds = oldHardwareBindingInfoPtr->GetSoftwareIds();
	QByteArrayList newSoftwareIds = newHardwareBindingInfoPtr->GetSoftwareIds();

	QByteArrayList addedIds;
	QByteArrayList removedIds;

	for (const QByteArray& softwareId : oldSoftwareIds){
		if (!newSoftwareIds.contains(softwareId)){
			removedIds << softwareId;
		}
	}

	for (const QByteArray& softwareId : newSoftwareIds){
		if (!oldSoftwareIds.contains(softwareId)){
			addedIds << softwareId;
		}
	}

	for (const QByteArray& softwareId : std::as_const(addedIds)){
		QString name = GetLicenseName(softwareId);
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("Add", "License", name, "", ""));
	}

	for (const QByteArray& softwareId : std::as_const(removedIds)){
		QString name = GetLicenseName(softwareId);
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("Remove", "License", name, "", ""));
	}

	return true;
}


QString CHardwareBindingChangeGeneratorComp::GetLicenseName(const QByteArray& productUuid) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_softwareInstanceCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
		const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
		if (productInstanceInfoPtr != nullptr){
			imtbase::ICollectionInfo::Ids licenseCollectionIds = productInstanceInfoPtr->GetLicenseInstances().GetElementIds();
			if (!licenseCollectionIds.isEmpty()){
				QByteArray licenseDefinitionUuid = licenseCollectionIds[0];

				imtbase::IObjectCollection::DataPtr licenseDataPtr;
				if (m_licenseCollectionCompPtr->GetObjectData(licenseDefinitionUuid, licenseDataPtr)){
					const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
					if (licenseInfoPtr != nullptr){
						QByteArray licenseId = licenseInfoPtr->GetLicenseId();
						QString licenseName = licenseInfoPtr->GetLicenseName();

						QString name = licenseName + " (" + licenseId + ")";
						return name;
					}
				}
			}
		}
	}

	return QString();
}


} // namespace prolifegql


