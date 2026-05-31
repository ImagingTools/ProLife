#include <prolifegql/CDeviceCollectionDocumentServiceComp.h>
#include <prolifesdl/SDL/1.0/CPP/DeviceCollectionDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>


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

// reimplemented (sdl::V1_0::prolife::CDeviceCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::prolife::CDeviceData CDeviceCollectionDocumentServiceComp::OnGetDeviceRepresentation(
			const sdl::V1_0::prolife::CGetDeviceRepresentationGqlRequest& getDeviceRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = getDeviceRepresentationRequest.GetRequestedArguments();
	if (!arguments.input){
		Q_ASSERT(false);
		return sdl::V1_0::prolife::CDeviceData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray documentId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input->id){
		documentId = *arguments.input->id;

		m_documentServiceCompPtr->GetDocumentData(userId, documentId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return sdl::V1_0::prolife::CDeviceData();
	}

	const prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableDeviceInfo*>(documentPtr.GetPtr());
	if (deviceInfoPtr == nullptr){
		return sdl::V1_0::prolife::CDeviceData();
	}

	QByteArray objectId = deviceInfoPtr->GetObjectUuid();

	sdl::V1_0::prolife::CDeviceData response;
	response.Emplace();

	response.id = (objectId);

	QByteArray macAddress = deviceInfoPtr->GetMacAddress();
	response.macAddress = (macAddress);

	QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();
	response.serialNumber = (serialNumber);

	QByteArray orderId = deviceInfoPtr->GetOrderId();
	response.orderId = (orderId);

	QString description = deviceInfoPtr->GetDescription();
	response.description = (description);

	prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
	QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
	response.productionStatus = (statusId);

	QByteArray project = deviceInfoPtr->GetProject();
	response.project = (project);

	QByteArray configurationType = deviceInfoPtr->GetConfigurationType();
	response.licenseName = (configurationType);

	QByteArray productUuid = deviceInfoPtr->GetDeviceType();
	response.deviceType = (productUuid);

	imtsdl::TElementList<sdl::V1_0::prolife::CSoftwareBindingInfo> softwareBindingInfoList;

	QByteArrayList softwareIds = GetBindedSoftware(objectId);
	for (const QByteArray& softwareId : std::as_const(softwareIds)){
		sdl::V1_0::prolife::CSoftwareBindingInfo softwareBindingInfo;
		if (GetSoftwareInfo(softwareId, softwareBindingInfo)){
			softwareBindingInfoList << softwareBindingInfo;
		}
	}

	response.softwareBindingInfos = softwareBindingInfoList;
	response.internalUse = deviceInfoPtr->IsInternalUse();

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CDeviceCollectionDocumentServiceComp::OnUpdateDeviceFromRepresentation(
			const sdl::V1_0::prolife::CUpdateDeviceFromRepresentationGqlRequest& updateDeviceFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = updateDeviceFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input){
		Q_ASSERT(false);
		return sdl::V1_0::imtbase::CDocumentOperationStatus();
	}

	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.Emplace();
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray documentId;
	if (arguments.input->documentId){
		documentId = *arguments.input->documentId;
	}

	sdl::V1_0::prolife::CDeviceData deviceData;
	if (arguments.input->deviceData){
		deviceData = *arguments.input->deviceData;
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentServiceCompPtr->GetDocumentData(userId, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(documentPtr.GetPtr());
	if (deviceInfoPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
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

	m_documentServiceCompPtr->SetDocumentData(userId, documentId, *deviceInfoPtr);

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

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


bool CDeviceCollectionDocumentServiceComp::GetSoftwareInfo(const QByteArray& softwareId, sdl::V1_0::prolife::CSoftwareBindingInfo& softwareInfo) const
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


sdl::V1_0::prolife::CIotDeviceData CDeviceCollectionDocumentServiceComp::OnGetIotDeviceRepresentation(
			const sdl::V1_0::prolife::CGetIotDeviceRepresentationGqlRequest& getIotDeviceRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = getIotDeviceRepresentationRequest.GetRequestedArguments();
	if (!arguments.input){
		Q_ASSERT(false);
		return sdl::V1_0::prolife::CIotDeviceData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input->id){
		objectId = *arguments.input->id;

		m_documentServiceCompPtr->GetDocumentData(userId, objectId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return sdl::V1_0::prolife::CIotDeviceData();
	}

	const prolifedata::COrderedIdentifiableIotDeviceInfo* iotDeviceInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableIotDeviceInfo*>(documentPtr.GetPtr());
	if (iotDeviceInfoPtr == nullptr){
		return sdl::V1_0::prolife::CIotDeviceData();
	}

	sdl::V1_0::prolife::CIotDeviceData response;
	response.Emplace();

	QByteArray serialNumber = iotDeviceInfoPtr->GetSerialNumber();
	response.serialNumber = (serialNumber);

	QByteArray modelId = iotDeviceInfoPtr->GetModelId();
	response.modelId = (modelId);

	QByteArray manufacturer = iotDeviceInfoPtr->GetManufacturer();
	response.manufacturer = (manufacturer);

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CDeviceCollectionDocumentServiceComp::OnUpdateIotDeviceFromRepresentation(
			const sdl::V1_0::prolife::CUpdateIotDeviceFromRepresentationGqlRequest& updateIotDeviceFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = updateIotDeviceFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input){
		Q_ASSERT(false);
		return sdl::V1_0::imtbase::CDocumentOperationStatus();
	}

	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.Emplace();
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray documentId;
	if (arguments.input->documentId){
		documentId = *arguments.input->documentId;
	}

	sdl::V1_0::prolife::CIotDeviceData iotDeviceData;
	if (arguments.input->iotDeviceData){
		iotDeviceData = *arguments.input->iotDeviceData;
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentServiceCompPtr->GetDocumentData(userId, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	prolifedata::COrderedIdentifiableIotDeviceInfo* iotDeviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableIotDeviceInfo*>(documentPtr.GetPtr());
	if (iotDeviceInfoPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
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

	m_documentServiceCompPtr->SetDocumentData(userId, documentId, *iotDeviceInfoPtr);

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace prolifegql


