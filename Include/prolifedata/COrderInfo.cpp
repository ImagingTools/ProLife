#include <prolifedata/COrderInfo.h>


//Qt include
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtlic/CProductInstanceInfo.h>


namespace prolifedata
{


// public static methods

QByteArray COrderInfo::GetTypeId()
{
	return "OrderInfo";
}


// public methods

COrderInfo::COrderInfo():
	m_status(OS_NONE)
{
	typedef istd::TSingleFactory<istd::IChangeable, imtlic::CIdentifiableSoftwareInstanceInfo> FactorySoftwareImpl;
	m_productInstanceCollection.RegisterFactory<FactorySoftwareImpl>("Software");

	typedef istd::TSingleFactory<istd::IChangeable, imtlic::CIdentifiableHardwareInstanceInfo> FactoryHardwareImpl;
	m_productInstanceCollection.RegisterFactory<FactoryHardwareImpl>("Hardware");
}


COrderInfo::~COrderInfo()
{
}


// reimplemented (IOrdered)

QByteArray COrderInfo::GetOrderId() const
{
	return m_orderId;
}


void COrderInfo::SetOrderId(const QByteArray& orderId)
{
	if (m_orderId != orderId){
		istd::CChangeNotifier changeNotifier(this);

		m_orderId = orderId;
	}
}


// reimplemented (IOrderInfo)

QByteArray COrderInfo::GetCustomerId() const
{
	return m_customerId;
}


void COrderInfo::SetCustomerId(const QByteArray& customerId)
{
	if (m_customerId != customerId){
		istd::CChangeNotifier changeNotifier(this);

		m_customerId = customerId;
	}
}


QString COrderInfo::GetDescription() const
{
	return m_description;
}


void COrderInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


COrderInfo::OrderStatus COrderInfo::GetOrderStatus() const
{
	return m_status;
}


void COrderInfo::SetOrderStatus(OrderStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier changeNotifier(this);

		m_status = status;
	}
}


imtbase::CObjectCollection* COrderInfo::GetProducts()
{
	return  &m_productInstanceCollection;
}


QByteArray COrderInfo::GetFactoryId() const
{
	return QByteArray();
}


// reimplemented (iser::ISerializable)

bool COrderInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();
	quint32 prolifeVersion;
	if (!versionInfo.GetVersionNumber(2022, prolifeVersion)){
		prolifeVersion = 0;
	}

	bool retVal = true;

//	static iser::CArchiveTag orderTag("OrderItem", "Order item", iser::CArchiveTag::TT_GROUP);
//	retVal = retVal && archive.BeginTag(orderTag);

	static iser::CArchiveTag orderIdTag("OrderId", "Order id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(orderIdTag);
	retVal = retVal && archive.Process(m_orderId);
	retVal = retVal && archive.EndTag(orderIdTag);

	static iser::CArchiveTag orderCustomerTag("OrderCustomer", "Order Customer", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(orderCustomerTag);
	retVal = retVal && archive.Process(m_customerId);
	retVal = retVal && archive.EndTag(orderCustomerTag);

	static iser::CArchiveTag orderDescriptionTag("Description", "Order Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(orderDescriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(orderDescriptionTag);

//	static iser::CArchiveTag orderStatusTag("Status", "Order Status", iser::CArchiveTag::TT_LEAF);
//	retVal = retVal && archive.BeginTag(orderStatusTag);
//	QString status;
//	retVal = retVal && archive.Process(status);
//	retVal = retVal && archive.EndTag(orderStatusTag);

	static iser::CArchiveTag statusTag("Status", "Order status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	if (prolifeVersion >= 5902){
		retVal = retVal && I_SERIALIZE_ENUM(OrderStatus, archive, m_status);
	}
	retVal = retVal && archive.EndTag(statusTag);

	static iser::CArchiveTag productsTag("Products", "Products in the order", iser::CArchiveTag::TT_GROUP);
	if (prolifeVersion >= 5902){
		retVal = retVal && archive.BeginTag(productsTag);
	}
	retVal = retVal && m_productInstanceCollection.Serialize(archive);
	if (prolifeVersion >= 5902){
		retVal = retVal && archive.EndTag(productsTag);
	}
//	retVal = retVal && archive.EndTag(orderTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int COrderInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool COrderInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const COrderInfo* sourcePtr = dynamic_cast<const COrderInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_orderId = sourcePtr->m_orderId;
		m_customerId = sourcePtr->m_customerId;
		m_description = sourcePtr->m_description;
		m_status = sourcePtr->m_status;
		m_productInstanceCollection.CopyFrom(sourcePtr->m_productInstanceCollection);

		bool retVal = true;


		return retVal;
	}

	return false;
}


istd::IChangeable* COrderInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<COrderInfo> clonePtr(new COrderInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool COrderInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_orderId.clear();
	m_customerId.clear();
	m_productInstanceCollection.ResetData();
	m_status = OS_NONE;

	return true;
}


} // namespace prolifedata


