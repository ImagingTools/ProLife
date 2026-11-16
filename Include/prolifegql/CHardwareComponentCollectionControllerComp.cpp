#include <prolifegql/CHardwareComponentCollectionControllerComp.h>


// Qt includes
#include <QtCore/QUuid>


namespace prolifegql
{


// protected methods

void CHardwareComponentCollectionControllerComp::FillComponentFromRepresentation(
			const sdl::V1_0::prolife::CHardwareComponentData& representation,
			prolifedata::IHardwareComponentInfo& componentInfo)
{
	if (representation.partNumber){
		componentInfo.SetPartNumber(*representation.partNumber);
	}
	if (representation.name){
		componentInfo.SetName(*representation.name);
	}
	if (representation.description){
		componentInfo.SetDescription(*representation.description);
	}
	if (representation.componentType){
		componentInfo.SetComponentType(*representation.componentType);
	}
	if (representation.revision){
		componentInfo.SetRevision(*representation.revision);
	}
	if (representation.manufacturer){
		componentInfo.SetManufacturer(*representation.manufacturer);
	}
	if (representation.supplier){
		componentInfo.SetSupplier(*representation.supplier);
	}
	if (representation.mpn){
		componentInfo.SetMpn(*representation.mpn);
	}
	if (representation.lifecycleStatus){
		componentInfo.SetLifecycleStatus(*representation.lifecycleStatus);
	}
	if (representation.serialized){
		componentInfo.SetSerialized(*representation.serialized);
	}
	if (representation.requirements){
		componentInfo.SetRequirements(representation.requirements->ToList());
	}
	if (representation.incompatibilities){
		componentInfo.SetIncompatibilities(representation.incompatibilities->ToList());
	}

	if (representation.parameters){
		prolifedata::IHardwareComponentInfo::Parameters parameters;
		for (const sdl::V1_0::prolife::CHardwareParameter& parameterRepresentation : representation.parameters->ToList()){
			prolifedata::IHardwareComponentInfo::Parameter parameter;
			if (parameterRepresentation.key){
				parameter.key = *parameterRepresentation.key;
			}
			if (parameterRepresentation.name){
				parameter.name = *parameterRepresentation.name;
			}
			if (parameterRepresentation.value){
				parameter.value = *parameterRepresentation.value;
			}
			if (parameterRepresentation.unit){
				parameter.unit = *parameterRepresentation.unit;
			}

			parameters.append(parameter);
		}

		componentInfo.SetParameters(parameters);
	}

	if (representation.bom){
		prolifedata::IHardwareComponentInfo::Bom bom;
		for (const sdl::V1_0::prolife::CBomEntry& entryRepresentation : representation.bom->ToList()){
			prolifedata::IHardwareComponentInfo::BomEntry entry;
			if (entryRepresentation.componentId){
				entry.componentId = *entryRepresentation.componentId;
			}
			if (entryRepresentation.quantity){
				entry.quantity = *entryRepresentation.quantity;
			}
			if (entryRepresentation.optional){
				entry.optional = *entryRepresentation.optional;
			}
			if (entryRepresentation.alternativeGroup){
				entry.alternativeGroup = *entryRepresentation.alternativeGroup;
			}

			bom.append(entry);
		}

		componentInfo.SetBom(bom);
	}
}


void CHardwareComponentCollectionControllerComp::FillRepresentationFromComponent(
			const prolifedata::IHardwareComponentInfo& componentInfo,
			sdl::V1_0::prolife::CHardwareComponentData& representation)
{
	representation.partNumber = componentInfo.GetPartNumber();
	representation.name = componentInfo.GetName();
	representation.description = componentInfo.GetDescription();
	representation.componentType = componentInfo.GetComponentType();
	representation.revision = componentInfo.GetRevision();
	representation.manufacturer = componentInfo.GetManufacturer();
	representation.supplier = componentInfo.GetSupplier();
	representation.mpn = componentInfo.GetMpn();
	representation.lifecycleStatus = componentInfo.GetLifecycleStatus();
	representation.serialized = componentInfo.IsSerialized();
	representation.requirements.Emplace().FromList(componentInfo.GetRequirements());
	representation.incompatibilities.Emplace().FromList(componentInfo.GetIncompatibilities());

	imtsdl::TElementList<sdl::V1_0::prolife::CHardwareParameter> parameters;
	for (const prolifedata::IHardwareComponentInfo::Parameter& parameter : componentInfo.GetParameters()){
		sdl::V1_0::prolife::CHardwareParameter parameterRepresentation;
		parameterRepresentation.key = parameter.key;
		parameterRepresentation.name = parameter.name;
		parameterRepresentation.value = parameter.value;
		parameterRepresentation.unit = parameter.unit;

		parameters << parameterRepresentation;
	}
	representation.parameters = parameters;

	imtsdl::TElementList<sdl::V1_0::prolife::CBomEntry> bom;
	for (const prolifedata::IHardwareComponentInfo::BomEntry& entry : componentInfo.GetBom()){
		sdl::V1_0::prolife::CBomEntry entryRepresentation;
		entryRepresentation.componentId = entry.componentId;
		entryRepresentation.quantity = entry.quantity;
		entryRepresentation.optional = entry.optional;
		entryRepresentation.alternativeGroup = entry.alternativeGroup;

		bom << entryRepresentation;
	}
	representation.bom = bom;
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

	if (requestInfo.items.isManufacturerRequested){
		representationObject.manufacturer = componentInfoPtr->GetManufacturer();
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

	FillRepresentationFromComponent(*componentInfoPtr, representationPayload);

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

	return true;
}


} // namespace prolifegql


