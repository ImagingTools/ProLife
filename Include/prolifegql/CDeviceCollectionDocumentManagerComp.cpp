#include <prolifegql/CDeviceCollectionDocumentManagerComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/IHardwareProductBinding.h>


namespace prolifegql
{


// protected methods

// reimplemented (sdl::prolife::DeviceCollectionDocumentManager::CGraphQlHandlerCompBase)

sdl::prolife::Sensors::CDeviceData CDeviceCollectionDocumentManagerComp::OnGetDeviceRepresentation(
			const sdl::prolife::DeviceCollectionDocumentManager::CGetDeviceRepresentationGqlRequest& getDeviceRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = getDeviceRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::prolife::Sensors::CDeviceData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;

		m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return sdl::prolife::Sensors::CDeviceData();
	}

	const prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableDeviceInfo*>(documentPtr.GetPtr());
	if (deviceInfoPtr == nullptr){
		return sdl::prolife::Sensors::CDeviceData();
	}

	sdl::prolife::Sensors::CDeviceData response;
	response.Version_1_0.Emplace();

	response.Version_1_0->id = (objectId);

	QByteArray macAddress = deviceInfoPtr->GetMacAddress();
	response.Version_1_0->macAddress = (macAddress);

	QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();
	response.Version_1_0->serialNumber = (serialNumber);

	QByteArray orderId = deviceInfoPtr->GetOrderId();
	response.Version_1_0->orderId = (orderId);

	QString description = deviceInfoPtr->GetDescription();
	response.Version_1_0->description = (description);

	prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
	QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
	response.Version_1_0->productionStatus = (statusId);

	QByteArray project = deviceInfoPtr->GetProject();
	response.Version_1_0->project = (project);

	QByteArray configurationType = deviceInfoPtr->GetConfigurationType();
	response.Version_1_0->licenseName = (configurationType);

	QByteArray productUuid = deviceInfoPtr->GetDeviceType();
	response.Version_1_0->deviceType = (productUuid);

	imtsdl::TElementList<sdl::prolife::Sensors::CSoftwareBindingInfo::V1_0> softwareBindingInfoList;

	QByteArrayList softwareIds = GetBindedSoftware(objectId);
	for (const QByteArray& softwareId : std::as_const(softwareIds)){
		sdl::prolife::Sensors::CSoftwareBindingInfo::V1_0 softwareBindingInfo;
		if (GetSoftwareInfo(softwareId, softwareBindingInfo)){
			softwareBindingInfoList << softwareBindingInfo;
		}
	}

	response.Version_1_0->softwareBindingInfos = softwareBindingInfoList;
	response.Version_1_0->internalUse = deviceInfoPtr->IsInternalUse();

	return response;
}


sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus CDeviceCollectionDocumentManagerComp::OnUpdateDeviceFromRepresentation(
			const sdl::prolife::DeviceCollectionDocumentManager::CUpdateDeviceFromRepresentationGqlRequest& updateDeviceFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = updateDeviceFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus();
	}

	sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::Failed;

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray documentId;
	if (arguments.input.Version_1_0->documentId){
		documentId = *arguments.input.Version_1_0->documentId;
	}

	sdl::prolife::Sensors::CDeviceData::V1_0 deviceData;
	if (arguments.input.Version_1_0->deviceData){
		deviceData = *arguments.input.Version_1_0->deviceData;
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(documentPtr.GetPtr());
	if (deviceInfoPtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	QString macAddress;
	if (deviceData.macAddress){
		macAddress = *deviceData.macAddress;
	}

	deviceInfoPtr->SetMacAddress(macAddress.toUtf8());

	QString serialNumber;
	if (deviceData.serialNumber){
		serialNumber = *deviceData.serialNumber;
	}

	deviceInfoPtr->SetSerialNumber(serialNumber.toUtf8());

	QString project;
	if (deviceData.project){
		project = *deviceData.project;
		deviceInfoPtr->SetProject(project.toUtf8());
	}

	QString orderId;
	if (deviceData.orderId){
		orderId = *deviceData.orderId;
		deviceInfoPtr->SetOrderId(orderId.toUtf8());
	}

	if (deviceData.description){
		deviceInfoPtr->SetDescription(*deviceData.description);
	}

	if (deviceData.productionStatus){
		QString status = *deviceData.productionStatus;
		prolifedata::IDeviceInfo::DeviceProductionStatus productionStatus = prolifedata::GetProductionStatusFromId(status.toUtf8());
		deviceInfoPtr->SetDeviceProductionStatus(productionStatus);
	}

	QString licenseName;
	if (deviceData.licenseName){
		licenseName = *deviceData.licenseName;
	}

	deviceInfoPtr->SetConfigurationType(licenseName.toUtf8());

	QString deviceType;
	if (deviceData.deviceType){
		deviceType = *deviceData.deviceType;
	}

	deviceInfoPtr->SetDeviceType(deviceType.toUtf8());

	if (deviceData.internalUse){
		deviceInfoPtr->SetInternalUse(*deviceData.internalUse);
	}

	m_documentManagerCompPtr->SetDocumentData(userId, documentId, *deviceInfoPtr);

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::Success;

	return response;
}


// private methods

QByteArrayList CDeviceCollectionDocumentManagerComp::GetBindedSoftware(const QByteArray& deviceId) const
{
	if (!m_bindingCollectionCompPtr.IsValid()){
		return QByteArrayList();
	}

	const prolifedata::IHardwareProductBinding* bindingInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_bindingCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
		bindingInfoPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
	}

	if (bindingInfoPtr == nullptr){
		return QByteArrayList();
	}

	return bindingInfoPtr->GetSoftwareIds();
}


bool CDeviceCollectionDocumentManagerComp::GetSoftwareInfo(const QByteArray& softwareId, sdl::prolife::Sensors::CSoftwareBindingInfo::V1_0& softwareInfo) const
{
	if (!m_softwareProductCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr productDataPtr;
	if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, productDataPtr)){
		imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
		if (productInstanceInfoPtr != nullptr){
			imtbase::ICollectionInfo::Ids elementsIds = productInstanceInfoPtr->GetLicenseInstances().GetElementIds();
			if (!elementsIds.isEmpty()){
				QByteArray licenseId = elementsIds[0];

				imtbase::IObjectCollection::DataPtr licenseDataPtr;
				if (m_licenseCollectionCompPtr->GetObjectData(licenseId, licenseDataPtr)){
					imtlic::ILicenseDefinition* licenseDefinitionPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
					if (licenseDefinitionPtr != nullptr){
						QString licenseName = licenseDefinitionPtr->GetLicenseName();
						QByteArray licenseDefinitionId = licenseDefinitionPtr->GetLicenseId();

						softwareInfo.id = softwareId;
						softwareInfo.softwareId = productInstanceInfoPtr->GetSerialNumber();
						softwareInfo.softwareName = licenseName + " (" + licenseDefinitionId + ")";

						return true;
					}
				}
			}
		}
	}

	return false;
}


QByteArray CDeviceCollectionDocumentManagerComp::GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			return userInfoPtr->GetId();
		}
	}

	return QByteArray();
}


} // namespace prolifegql

