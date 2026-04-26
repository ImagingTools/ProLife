#include <prolifedata/CProcurementOrderInfo.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/IArchive.h>
#include <istd/CChangeGroup.h>
#include <istd/CChangeNotifier.h>


namespace prolifedata
{


// public methods

QByteArray CProcurementOrderInfo::GetTypeId()
{
	return "ProcurementOrderInfo";
}


CProcurementOrderInfo::CProcurementOrderInfo():
	m_status(PS_CREATED)
{
}


// reimplemented (IProcurementOrderInfo)

QString CProcurementOrderInfo::GetProcurementOrderNumber() const
{
	return m_procurementOrderNumber;
}


void CProcurementOrderInfo::SetProcurementOrderNumber(const QString& orderNumber)
{
	if (m_procurementOrderNumber != orderNumber){
		istd::CChangeNotifier changeNotifier(this);

		m_procurementOrderNumber = orderNumber;
	}
}


QByteArray CProcurementOrderInfo::GetSupplierId() const
{
	return m_supplierId;
}


void CProcurementOrderInfo::SetSupplierId(const QByteArray& supplierId)
{
	if (m_supplierId != supplierId){
		istd::CChangeNotifier changeNotifier(this);

		m_supplierId = supplierId;
	}
}


QString CProcurementOrderInfo::GetSupplierName() const
{
	return m_supplierName;
}


void CProcurementOrderInfo::SetSupplierName(const QString& supplierName)
{
	if (m_supplierName != supplierName){
		istd::CChangeNotifier changeNotifier(this);

		m_supplierName = supplierName;
	}
}


IProcurementOrderInfo::ProcurementStatus CProcurementOrderInfo::GetProcurementStatus() const
{
	return m_status;
}


void CProcurementOrderInfo::SetProcurementStatus(ProcurementStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier changeNotifier(this);

		m_status = status;
	}
}


QString CProcurementOrderInfo::GetDescription() const
{
	return m_description;
}


void CProcurementOrderInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


QByteArray CProcurementOrderInfo::GetLinkedOrderUuid() const
{
	return m_linkedOrderUuid;
}


void CProcurementOrderInfo::SetLinkedOrderUuid(const QByteArray& linkedOrderUuid)
{
	if (m_linkedOrderUuid != linkedOrderUuid){
		istd::CChangeNotifier changeNotifier(this);

		m_linkedOrderUuid = linkedOrderUuid;
	}
}


QString CProcurementOrderInfo::GetExpectedDeliveryDate() const
{
	return m_expectedDeliveryDate;
}


void CProcurementOrderInfo::SetExpectedDeliveryDate(const QString& expectedDeliveryDate)
{
	if (m_expectedDeliveryDate != expectedDeliveryDate){
		istd::CChangeNotifier changeNotifier(this);

		m_expectedDeliveryDate = expectedDeliveryDate;
	}
}


// reimplemented (iser::IObject)

QByteArray CProcurementOrderInfo::GetFactoryId() const
{
	return QByteArray();
}


// reimplemented (iser::ISerializable)

bool CProcurementOrderInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag statusTag("Status", "Procurement status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	retVal = retVal && I_SERIALIZE_ENUM(ProcurementStatus, archive, m_status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag orderNumberTag("OrderNumber", "Procurement order number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(orderNumberTag);
	retVal = retVal && archive.Process(m_procurementOrderNumber);
	retVal = retVal && archive.EndTag(orderNumberTag);

	iser::CArchiveTag supplierIdTag("SupplierId", "Supplier ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(supplierIdTag);
	retVal = retVal && archive.Process(m_supplierId);
	retVal = retVal && archive.EndTag(supplierIdTag);

	iser::CArchiveTag supplierNameTag("SupplierName", "Supplier name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(supplierNameTag);
	retVal = retVal && archive.Process(m_supplierName);
	retVal = retVal && archive.EndTag(supplierNameTag);

	iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	iser::CArchiveTag linkedOrderTag("LinkedOrderUuid", "Linked order UUID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(linkedOrderTag);
	retVal = retVal && archive.Process(m_linkedOrderUuid);
	retVal = retVal && archive.EndTag(linkedOrderTag);

	iser::CArchiveTag deliveryDateTag("ExpectedDeliveryDate", "Expected delivery date", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(deliveryDateTag);
	retVal = retVal && archive.Process(m_expectedDeliveryDate);
	retVal = retVal && archive.EndTag(deliveryDateTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CProcurementOrderInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CProcurementOrderInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CProcurementOrderInfo* sourcePtr = dynamic_cast<const CProcurementOrderInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_procurementOrderNumber = sourcePtr->m_procurementOrderNumber;
		m_supplierId = sourcePtr->m_supplierId;
		m_supplierName = sourcePtr->m_supplierName;
		m_status = sourcePtr->m_status;
		m_description = sourcePtr->m_description;
		m_linkedOrderUuid = sourcePtr->m_linkedOrderUuid;
		m_expectedDeliveryDate = sourcePtr->m_expectedDeliveryDate;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CProcurementOrderInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CProcurementOrderInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CProcurementOrderInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_procurementOrderNumber.clear();
	m_supplierId.clear();
	m_supplierName.clear();
	m_status = PS_CREATED;
	m_description.clear();
	m_linkedOrderUuid.clear();
	m_expectedDeliveryDate.clear();

	return true;
}


} // namespace prolifedata
