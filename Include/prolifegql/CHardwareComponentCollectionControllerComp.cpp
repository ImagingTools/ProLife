#include <prolifegql/CHardwareComponentCollectionControllerComp.h>


// Qt includes
#include <QtCore/QUuid>

// ProLife includes
#include <prolifegql/HardwareCatalog.h>
#include <prolifegql/HardwareRepresentation.h>


namespace prolifegql
{


// protected methods

bool CHardwareComponentCollectionControllerComp::CheckComponent(
			const prolifedata::IHardwareComponentInfo& componentInfo,
			QString& errorMessage) const
{
	if (!CheckHardwareComponentFields(componentInfo, errorMessage)){
		return false;
	}

	return CheckHardwareComponentVendors(componentInfo, *m_companyCollectionCompPtr, errorMessage);
}


// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

QByteArray CHardwareComponentCollectionControllerComp::GetObjectTypeIdFromRequest(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	// The collection holds hardware components only, whatever the client sends.
	return QByteArrayLiteral("HardwareComponent");
}


QString CHardwareComponentCollectionControllerComp::GetObjectNameFromRequest(const imtgql::CGqlRequest& gqlRequest) const
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


// reimplemented (sdl::V1_0::prolife::CHardwareComponentCollectionControllerCompBase)

bool CHardwareComponentCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::V1_0::prolife::CHardwareComponentsListGqlRequest& listRequest,
			sdl::V1_0::prolife::CHardwareComponentItem& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();

	const prolifedata::IHardwareComponentInfo* componentInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		componentInfoPtr = dynamic_cast<const prolifedata::IHardwareComponentInfo*>(dataPtr.GetPtr());
	}

	if (componentInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to create representation of the hardware component '%1'").arg(QString::fromUtf8(objectId));
		SendErrorMessage(0, errorMessage, "CHardwareComponentCollectionControllerComp");

		return false;
	}

	sdl::V1_0::prolife::HardwareComponentsListRequestInfo requestInfo = listRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = objectId;
	}

	if (requestInfo.items.isTypeIdRequested && m_objectCollectionCompPtr.IsValid()){
		representationObject.typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	}

	if (requestInfo.items.isNameRequested){
		representationObject.name = componentInfoPtr->GetName();
	}

	if (requestInfo.items.isPartNumberRequested){
		representationObject.partNumber = componentInfoPtr->GetPartNumber();
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = componentInfoPtr->GetDescription();
	}

	if (requestInfo.items.isComponentTypeRequested){
		representationObject.componentType = componentInfoPtr->GetComponentType();
	}

	if (requestInfo.items.isRevisionRequested){
		representationObject.revision = componentInfoPtr->GetRevision();
	}

	if (requestInfo.items.isManufacturerIdRequested){
		representationObject.manufacturerId = componentInfoPtr->GetManufacturerId();
	}

	if (requestInfo.items.isManufacturerNameRequested){
		representationObject.manufacturerName = GetCompanyName(m_companyCollectionCompPtr.GetPtr(), componentInfoPtr->GetManufacturerId());
	}

	if (requestInfo.items.isLifecycleStatusRequested){
		representationObject.lifecycleStatus = componentInfoPtr->GetLifecycleStatus();
	}

	if (requestInfo.items.isSerializedRequested){
		representationObject.serialized = componentInfoPtr->IsSerialized();
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


istd::IChangeableUniquePtr CHardwareComponentCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::V1_0::prolife::CHardwareComponentData& representation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_componentInfoFactCompPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to create a hardware component. Error: Attribute 'HardwareComponentFactory' was not set");
		SendErrorMessage(0, errorMessage, "CHardwareComponentCollectionControllerComp");

		return nullptr;
	}

	istd::TUniqueInterfacePtr<prolifedata::IHardwareComponentInfo> componentInstancePtr = m_componentInfoFactCompPtr.CreateInstance();
	if (!componentInstancePtr.IsValid()){
		errorMessage = QStringLiteral("Unable to create a hardware component instance");
		SendErrorMessage(0, errorMessage, "CHardwareComponentCollectionControllerComp");

		return nullptr;
	}

	if (representation.id){
		newObjectId = *representation.id;
	}
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	FillComponentFromRepresentation(representation, *componentInstancePtr);

	if (!CheckComponent(*componentInstancePtr, errorMessage)){
		SendErrorMessage(0, errorMessage, "CHardwareComponentCollectionControllerComp");

		return nullptr;
	}

	return componentInstancePtr;
}


bool CHardwareComponentCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::V1_0::prolife::CGetHardwareComponentItemGqlRequest& itemRequest,
			sdl::V1_0::prolife::CHardwareComponentData& representationPayload,
			QString& errorMessage) const
{
	const prolifedata::IHardwareComponentInfo* componentInfoPtr = dynamic_cast<const prolifedata::IHardwareComponentInfo*>(&data);
	if (componentInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to create representation of the hardware component. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CHardwareComponentCollectionControllerComp");

		return false;
	}

	sdl::V1_0::prolife::GetHardwareComponentItemRequestArguments arguments = itemRequest.GetRequestedArguments();
	if (arguments.input && arguments.input->id){
		representationPayload.id = *arguments.input->id;
	}

	FillRepresentationFromComponent(*componentInfoPtr, m_companyCollectionCompPtr.GetPtr(), representationPayload);

	return true;
}


bool CHardwareComponentCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const ::imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::V1_0::prolife::CUpdateHardwareComponentGqlRequest& updateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	prolifedata::IHardwareComponentInfo* componentInfoPtr = dynamic_cast<prolifedata::IHardwareComponentInfo*>(&object);
	if (componentInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to update the hardware component. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CHardwareComponentCollectionControllerComp");

		return false;
	}

	sdl::V1_0::prolife::UpdateHardwareComponentRequestArguments arguments = updateRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->item){
		errorMessage = QStringLiteral("Unable to update the hardware component. Error: Request has no component data");
		SendErrorMessage(0, errorMessage, "CHardwareComponentCollectionControllerComp");

		return false;
	}

	componentInfoPtr->ResetData();

	FillComponentFromRepresentation(*arguments.input->item, *componentInfoPtr);

	if (!CheckComponent(*componentInfoPtr, errorMessage)){
		SendErrorMessage(0, errorMessage, "CHardwareComponentCollectionControllerComp");

		return false;
	}

	return true;
}


} // namespace prolifegql


