#include <prolifegql/CProcurementCollectionControllerComp.h>


// ProLife includes
#include <prolifedata/CProcurementOrderInfo.h>


namespace prolifegql
{


// reimplemented (sdl::prolife::Procurement::CProcurementCollectionControllerCompBase)

bool CProcurementCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::prolife::Procurement::CProcurementListGqlRequest& /*procurementListRequest*/,
			sdl::prolife::Procurement::CProcurementItem::V1_0& representationObject,
			QString& /*errorMessage*/) const
{
	const prolifedata::IProcurementOrderInfo* procurementInfoPtr =
		dynamic_cast<const prolifedata::IProcurementOrderInfo*>(objectCollectionIterator.GetCurrentObject());

	if (procurementInfoPtr == nullptr){
		return false;
	}

	representationObject.Set_procurementOrderNumber(procurementInfoPtr->GetProcurementOrderNumber());
	representationObject.Set_supplierName(procurementInfoPtr->GetSupplierName());
	representationObject.Set_supplierId(procurementInfoPtr->GetSupplierId());
	representationObject.Set_status(
		QString::fromUtf8(prolifedata::IProcurementOrderInfo::GetProcurementStatusEnumId(
			procurementInfoPtr->GetProcurementStatus())));
	representationObject.Set_description(procurementInfoPtr->GetDescription());
	representationObject.Set_expectedDeliveryDate(procurementInfoPtr->GetExpectedDeliveryDate());
	representationObject.Set_linkedOrderUuid(procurementInfoPtr->GetLinkedOrderUuid());

	return true;
}


istd::IChangeableUniquePtr CProcurementCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::prolife::Procurement::CProcurementData::V1_0& procurementDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	istd::IChangeableUniquePtr newObjectPtr = m_procurementInfoFactCompPtr->CreateObject();

	if (!FillObjectFromRepresentation(procurementDataRepresentation, *newObjectPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	return newObjectPtr;
}


bool CProcurementCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::prolife::Procurement::CProcurementItemGqlRequest& /*procurementItemRequest*/,
			sdl::prolife::Procurement::CProcurementData::V1_0& representationPayload,
			QString& /*errorMessage*/) const
{
	const prolifedata::IProcurementOrderInfo* procurementInfoPtr =
		dynamic_cast<const prolifedata::IProcurementOrderInfo*>(&data);

	if (procurementInfoPtr == nullptr){
		return false;
	}

	representationPayload.Set_procurementOrderNumber(procurementInfoPtr->GetProcurementOrderNumber());
	representationPayload.Set_supplierName(procurementInfoPtr->GetSupplierName());
	representationPayload.Set_supplierId(procurementInfoPtr->GetSupplierId());
	representationPayload.Set_procurementStatus(
		QString::fromUtf8(prolifedata::IProcurementOrderInfo::GetProcurementStatusEnumId(
			procurementInfoPtr->GetProcurementStatus())));
	representationPayload.Set_description(procurementInfoPtr->GetDescription());
	representationPayload.Set_expectedDeliveryDate(procurementInfoPtr->GetExpectedDeliveryDate());
	representationPayload.Set_linkedOrderUuid(procurementInfoPtr->GetLinkedOrderUuid());

	return true;
}


bool CProcurementCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const ::imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::prolife::Procurement::CProcurementUpdateGqlRequest& procurementUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	QByteArray objectId;
	return FillObjectFromRepresentation(procurementUpdateRequest.Get_item(), object, objectId, errorMessage);
}


void CProcurementCollectionControllerComp::SetAdditionalFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlParamObject& viewParamsGql,
			iprm::CParamsSet* filterParams) const
{
	if (m_groupFilterParamJoinerCompPtr != nullptr && filterParams != nullptr){
		m_groupFilterParamJoinerCompPtr->JoinGroupFilter(gqlRequest, *filterParams);
	}
}


bool CProcurementCollectionControllerComp::OnBeforeRemoveElements(
			const QByteArrayList& /*elementIds*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return true;
}


// private methods

bool CProcurementCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::prolife::Procurement::CProcurementData::V1_0& representation,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& /*errorMessage*/) const
{
	prolifedata::IProcurementOrderInfo* procurementInfoPtr =
		dynamic_cast<prolifedata::IProcurementOrderInfo*>(&object);

	if (procurementInfoPtr == nullptr){
		return false;
	}

	objectId = representation.Get_id().toUtf8();

	procurementInfoPtr->SetProcurementOrderNumber(representation.Get_procurementOrderNumber());
	procurementInfoPtr->SetSupplierId(representation.Get_supplierId().toUtf8());
	procurementInfoPtr->SetSupplierName(representation.Get_supplierName());
	procurementInfoPtr->SetDescription(representation.Get_description());
	procurementInfoPtr->SetExpectedDeliveryDate(representation.Get_expectedDeliveryDate());
	procurementInfoPtr->SetLinkedOrderUuid(representation.Get_linkedOrderUuid().toUtf8());

	QString statusStr = representation.Get_procurementStatus();
	if (!statusStr.isEmpty()){
		prolifedata::IProcurementOrderInfo::ProcurementStatus status;
		if (prolifedata::IProcurementOrderInfo::ParseProcurementStatusEnum(statusStr.toUtf8(), status)){
			procurementInfoPtr->SetProcurementStatus(status);
		}
	}

	return true;
}


} // namespace prolifegql
