#include <prolifegql/CHardwareConfigurationCollectionControllerComp.h>


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtlic/IProductInfo.h>

// ProLife includes
#include <prolifedata/IHardwareComponentInfo.h>


namespace prolifegql
{


// protected methods

void CHardwareConfigurationCollectionControllerComp::FillConfigurationFromRepresentation(
			const sdl::V1_0::prolife::CHardwareConfigurationData& representation,
			prolifedata::IHardwareConfigurationInfo& configurationInfo)
{
	if (representation.configurationId){
		configurationInfo.SetConfigurationId(*representation.configurationId);
	}
	if (representation.name){
		configurationInfo.SetName(*representation.name);
	}
	if (representation.description){
		configurationInfo.SetDescription(*representation.description);
	}
	if (representation.productId){
		configurationInfo.SetProductId(*representation.productId);
	}
	if (representation.componentIds){
		configurationInfo.SetComponentIds(representation.componentIds->ToList());
	}
	if (representation.bundledLicenseIds){
		configurationInfo.SetBundledLicenseIds(representation.bundledLicenseIds->ToList());
	}
}


void CHardwareConfigurationCollectionControllerComp::FillRepresentationFromConfiguration(
			const prolifedata::IHardwareConfigurationInfo& configurationInfo,
			sdl::V1_0::prolife::CHardwareConfigurationData& representation)
{
	representation.configurationId = configurationInfo.GetConfigurationId();
	representation.name = configurationInfo.GetName();
	representation.description = configurationInfo.GetDescription();
	representation.productId = configurationInfo.GetProductId();
	representation.componentIds.Emplace().FromList(configurationInfo.GetComponentIds());
	representation.bundledLicenseIds.Emplace().FromList(configurationInfo.GetBundledLicenseIds());
}


imtsdl::TElementList<sdl::V1_0::prolife::CHardwareConfigurationComponent> CHardwareConfigurationCollectionControllerComp::CreateComponentsRepresentation(
			const QByteArrayList& componentIds) const
{
	imtsdl::TElementList<sdl::V1_0::prolife::CHardwareConfigurationComponent> components;
	for (const QByteArray& componentId : componentIds){
		sdl::V1_0::prolife::CHardwareConfigurationComponent componentRepresentation;
		componentRepresentation.id = componentId;
		componentRepresentation.name = QString::fromUtf8(componentId);
		componentRepresentation.partNumber = QString();

		imtbase::IObjectCollection::DataPtr componentDataPtr;
		if (m_componentCollectionCompPtr.IsValid() && m_componentCollectionCompPtr->GetObjectData(componentId, componentDataPtr)){
			const prolifedata::IHardwareComponentInfo* componentInfoPtr = dynamic_cast<const prolifedata::IHardwareComponentInfo*>(componentDataPtr.GetPtr());
			if (componentInfoPtr != nullptr){
				componentRepresentation.name = componentInfoPtr->GetName();
				componentRepresentation.partNumber = QString::fromUtf8(componentInfoPtr->GetPartNumber());
			}
		}

		components << componentRepresentation;
	}

	return components;
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

	if (requestInfo.items.isProductNameRequested && m_productCollectionCompPtr.IsValid()){
		QString productName;
		imtbase::IObjectCollection::DataPtr productDataPtr;
		if (m_productCollectionCompPtr->GetObjectData(configurationInfoPtr->GetProductId(), productDataPtr)){
			const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(productDataPtr.GetPtr());
			if (productInfoPtr != nullptr){
				productName = productInfoPtr->GetName();
			}
		}

		representationObject.productName = productName;
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
	representationPayload.components = CreateComponentsRepresentation(configurationInfoPtr->GetComponentIds());

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

	return true;
}


} // namespace prolifegql


