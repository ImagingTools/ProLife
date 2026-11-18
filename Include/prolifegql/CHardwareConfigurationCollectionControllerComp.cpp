#include <prolifegql/CHardwareConfigurationCollectionControllerComp.h>


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtlic/IProductInfo.h>

// ProLife includes
#include <prolifedata/IHardwareComponentInfo.h>
#include <prolifegql/HardwareCatalog.h>
#include <prolifegql/HardwareRepresentation.h>


namespace prolifegql
{


// protected methods

bool CHardwareConfigurationCollectionControllerComp::CheckConfiguration(
			const prolifedata::IHardwareConfigurationInfo& configurationInfo,
			QString& errorMessage) const
{
	if (!m_productCollectionCompPtr.IsValid() || !m_componentCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to check the hardware configuration. Error: Product or component collection is not set");

		return false;
	}

	if (!CheckHardwareConfigurationFields(configurationInfo, errorMessage)){
		return false;
	}

	if (!CheckHardwareModel(*m_productCollectionCompPtr, configurationInfo.GetProductId(), errorMessage)){
		return false;
	}

	return CheckHardwareComponents(*m_componentCollectionCompPtr, configurationInfo.GetComponentIds(), errorMessage);
}


// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

QByteArray CHardwareConfigurationCollectionControllerComp::GetObjectTypeIdFromRequest(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	// The collection holds hardware configurations only, whatever the client sends.
	return QByteArrayLiteral("HardwareConfiguration");
}


QString CHardwareConfigurationCollectionControllerComp::GetObjectNameFromRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	QString name = BaseClass::GetObjectNameFromRequest(gqlRequest);
	if (!name.isEmpty()){
		return name;
	}

	// Without an explicit document name the collection shows the name of the item itself.
	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		const imtgql::CGqlParamObject* itemParamPtr = inputParamPtr->GetParamArgumentObjectPtr("item");
		if (itemParamPtr != nullptr){
			name = itemParamPtr->GetParamArgumentValue("name").toString();
		}
	}

	return name;
}


// reimplemented (sdl::V1_0::prolife::CHardwareConfigurationCollectionControllerCompBase)

bool CHardwareConfigurationCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::V1_0::prolife::CHardwareConfigurationsListGqlRequest& listRequest,
			sdl::V1_0::prolife::CHardwareConfigurationItem& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();

	const prolifedata::IHardwareConfigurationInfo* configurationInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		configurationInfoPtr = dynamic_cast<const prolifedata::IHardwareConfigurationInfo*>(dataPtr.GetPtr());
	}

	if (configurationInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to create representation of the hardware configuration '%1'").arg(QString::fromUtf8(objectId));
		SendErrorMessage(0, errorMessage, "CHardwareConfigurationCollectionControllerComp");

		return false;
	}

	sdl::V1_0::prolife::HardwareConfigurationsListRequestInfo requestInfo = listRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = objectId;
	}

	if (requestInfo.items.isTypeIdRequested && m_objectCollectionCompPtr.IsValid()){
		representationObject.typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	}

	if (requestInfo.items.isNameRequested){
		representationObject.name = configurationInfoPtr->GetName();
	}

	if (requestInfo.items.isConfigurationIdRequested){
		representationObject.configurationId = configurationInfoPtr->GetConfigurationId();
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.productId = configurationInfoPtr->GetProductId();
	}

	if (requestInfo.items.isProductNameRequested){
		representationObject.productName = GetProductName(m_productCollectionCompPtr.GetPtr(), configurationInfoPtr->GetProductId());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = configurationInfoPtr->GetDescription();
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime().toUTC();
		representationObject.added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("TimeStamp").toDateTime().toUTC();
		representationObject.timeStamp = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
	}

	return true;
}


istd::IChangeableUniquePtr CHardwareConfigurationCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::V1_0::prolife::CHardwareConfigurationData& representation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_configurationInfoFactCompPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to create a hardware configuration. Error: Attribute 'HardwareConfigurationFactory' was not set");
		SendErrorMessage(0, errorMessage, "CHardwareConfigurationCollectionControllerComp");

		return nullptr;
	}

	istd::TUniqueInterfacePtr<prolifedata::IHardwareConfigurationInfo> configurationInstancePtr = m_configurationInfoFactCompPtr.CreateInstance();
	if (!configurationInstancePtr.IsValid()){
		errorMessage = QStringLiteral("Unable to create a hardware configuration instance");
		SendErrorMessage(0, errorMessage, "CHardwareConfigurationCollectionControllerComp");

		return nullptr;
	}

	if (representation.id){
		newObjectId = *representation.id;
	}
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	FillConfigurationFromRepresentation(representation, *configurationInstancePtr);

	if (!CheckConfiguration(*configurationInstancePtr, errorMessage)){
		SendErrorMessage(0, errorMessage, "CHardwareConfigurationCollectionControllerComp");

		return nullptr;
	}

	return configurationInstancePtr;
}


bool CHardwareConfigurationCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::V1_0::prolife::CGetHardwareConfigurationItemGqlRequest& itemRequest,
			sdl::V1_0::prolife::CHardwareConfigurationData& representationPayload,
			QString& errorMessage) const
{
	const prolifedata::IHardwareConfigurationInfo* configurationInfoPtr = dynamic_cast<const prolifedata::IHardwareConfigurationInfo*>(&data);
	if (configurationInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to create representation of the hardware configuration. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CHardwareConfigurationCollectionControllerComp");

		return false;
	}

	sdl::V1_0::prolife::GetHardwareConfigurationItemRequestArguments arguments = itemRequest.GetRequestedArguments();
	if (arguments.input && arguments.input->id){
		representationPayload.id = *arguments.input->id;
	}

	FillRepresentationFromConfiguration(*configurationInfoPtr, representationPayload);
	representationPayload.productName = GetProductName(m_productCollectionCompPtr.GetPtr(), configurationInfoPtr->GetProductId());
	representationPayload.components = CreateComponentsRepresentation(m_componentCollectionCompPtr.GetPtr(), configurationInfoPtr->GetComponentIds());

	return true;
}


bool CHardwareConfigurationCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const ::imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::V1_0::prolife::CUpdateHardwareConfigurationGqlRequest& updateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	prolifedata::IHardwareConfigurationInfo* configurationInfoPtr = dynamic_cast<prolifedata::IHardwareConfigurationInfo*>(&object);
	if (configurationInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to update the hardware configuration. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CHardwareConfigurationCollectionControllerComp");

		return false;
	}

	sdl::V1_0::prolife::UpdateHardwareConfigurationRequestArguments arguments = updateRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->item){
		errorMessage = QStringLiteral("Unable to update the hardware configuration. Error: Request has no configuration data");
		SendErrorMessage(0, errorMessage, "CHardwareConfigurationCollectionControllerComp");

		return false;
	}

	configurationInfoPtr->ResetData();

	FillConfigurationFromRepresentation(*arguments.input->item, *configurationInfoPtr);

	if (!CheckConfiguration(*configurationInfoPtr, errorMessage)){
		SendErrorMessage(0, errorMessage, "CHardwareConfigurationCollectionControllerComp");

		return false;
	}

	return true;
}


} // namespace prolifegql


