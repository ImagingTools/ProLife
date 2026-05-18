#include <prolifegql/CDeviceCollectionDocumentServiceComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/CIotDeviceInfo.h>
#include <prolifedata/IHardwareProductBinding.h>


namespace prolifegql
{


// protected methods

// reimplemented (sdl::prolife::DeviceCollectionDocumentService::CGraphQlHandlerCompBase)

sdl::prolife::Sensors::CDeviceData CDeviceCollectionDocumentServiceComp::OnGetDeviceRepresentation(
			const sdl::prolife::DeviceCollectionDocumentService::CGetDeviceRepresentationGqlRequest& getDeviceRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = getDeviceRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::prolife::Sensors::CDeviceData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray documentId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input.Version_1_0->id){
		documentId = *arguments.input.Version_1_0->id;

		m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return sdl::prolife::Sensors::CDeviceData();
	}

	const prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableDeviceInfo*>(documentPtr.GetPtr());
	if (deviceInfoPtr == nullptr){
		return sdl::prolife::Sensors::CDeviceData();
	}

	QByteArray objectId = deviceInfoPtr->GetObjectUuid();

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


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CDeviceCollectionDocumentServiceComp::OnUpdateDeviceFromRepresentation(
			const sdl::prolife::DeviceCollectionDocumentService::CUpdateDeviceFromRepresentationGqlRequest& updateDeviceFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = updateDeviceFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus();
	}

	sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Failed;

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
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(documentPtr.GetPtr());
	if (deviceInfoPtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
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

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Success;

	return response;
}


// private methods

QByteArrayList CDeviceCollectionDocumentServiceComp::GetBindedSoftware(const QByteArray& deviceId) const
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


bool CDeviceCollectionDocumentServiceComp::GetSoftwareInfo(const QByteArray& softwareId, sdl::prolife::Sensors::CSoftwareBindingInfo::V1_0& softwareInfo) const
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


sdl::prolife::Sensors::CIotDeviceData CDeviceCollectionDocumentServiceComp::OnGetIotDeviceRepresentation(
			const sdl::prolife::DeviceCollectionDocumentService::CGetIotDeviceRepresentationGqlRequest& getIotDeviceRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = getIotDeviceRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::prolife::Sensors::CIotDeviceData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;

		m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return sdl::prolife::Sensors::CIotDeviceData();
	}

	const prolifedata::COrderedIdentifiableIotDeviceInfo* iotDeviceInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableIotDeviceInfo*>(documentPtr.GetPtr());
	if (iotDeviceInfoPtr == nullptr){
		return sdl::prolife::Sensors::CIotDeviceData();
	}

	sdl::prolife::Sensors::CIotDeviceData response;
	response.Version_1_0.Emplace();

	QByteArray serialNumber = iotDeviceInfoPtr->GetSerialNumber();
	response.Version_1_0->serialNumber = (serialNumber);

	QByteArray modelId = iotDeviceInfoPtr->GetModelId();
	response.Version_1_0->modelId = (modelId);

	QByteArray manufacturer = iotDeviceInfoPtr->GetManufacturer();
	response.Version_1_0->manufacturer = (manufacturer);

	return response;
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CDeviceCollectionDocumentServiceComp::OnUpdateIotDeviceFromRepresentation(
			const sdl::prolife::DeviceCollectionDocumentService::CUpdateIotDeviceFromRepresentationGqlRequest& updateIotDeviceFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = updateIotDeviceFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus();
	}

	sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Failed;

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray documentId;
	if (arguments.input.Version_1_0->documentId){
		documentId = *arguments.input.Version_1_0->documentId;
	}

	sdl::prolife::Sensors::CIotDeviceData::V1_0 iotDeviceData;
	if (arguments.input.Version_1_0->iotDeviceData){
		iotDeviceData = *arguments.input.Version_1_0->iotDeviceData;
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	prolifedata::COrderedIdentifiableIotDeviceInfo* iotDeviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableIotDeviceInfo*>(documentPtr.GetPtr());
	if (iotDeviceInfoPtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	QByteArray factoryNumber;
	if (iotDeviceData.serialNumber){
		factoryNumber = *iotDeviceData.serialNumber;
	}
	iotDeviceInfoPtr->SetSerialNumber(factoryNumber);

	QByteArray modelId;
	if (iotDeviceData.modelId){
		modelId = *iotDeviceData.modelId;
	}
	iotDeviceInfoPtr->SetModelId(modelId);

	if (iotDeviceData.manufacturer){
		iotDeviceInfoPtr->SetManufacturer(*iotDeviceData.manufacturer);
	}

	m_documentManagerCompPtr->SetDocumentData(userId, documentId, *iotDeviceInfoPtr);

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Success;

	return response;
}


} // namespace prolifegql


