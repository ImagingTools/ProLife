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



namespace prolifedata
{


// public static methods

QByteArray COrderInfo::GetTypeId()
{
	return "OrderInfo";
}


// public methods

COrderInfo::COrderInfo()
{
}


COrderInfo::~COrderInfo()
{
}

// reimplemented (IOrderInfo)

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

imtlic::CProductInstanceCollection* COrderInfo::GetProducts()
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

	bool retVal = true;

	static iser::CArchiveTag orderIdTag("OrderId", "Order id", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(orderIdTag);
	retVal = retVal && archive.Process(m_orderId);
	retVal = retVal && archive.EndTag(orderIdTag);

	static iser::CArchiveTag orderCustomerTag("OrderCustomer", "Order Customer", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(orderCustomerTag);
	retVal = retVal && archive.Process(m_customerId);
	retVal = retVal && archive.EndTag(orderCustomerTag);

	static iser::CArchiveTag orderCommentTag("OrderComment", "Order Comment", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(orderCommentTag);
	retVal = retVal && archive.Process(m_comment);
	retVal = retVal && archive.EndTag(orderCommentTag);

	m_productInstanceCollection.Serialize(archive);

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
		m_productInstanceCollection.CopyFrom(sourcePtr->m_productInstanceCollection);

		bool retVal = true;


		return retVal;
	}

	return false;
}


istd::IChangeable* COrderInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<COrderInfo> clonePtr(new COrderInfo);
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

	return true;
}


} // namespace prolifedata


