#include <prolifedata/COrderCustomerRole.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/IArchive.h>
#include <istd/CChangeGroup.h>
#include <istd/CChangeNotifier.h>


namespace prolifedata
{


// public methods

QByteArray COrderCustomerRole::GetTypeId()
{
	return "OrderCustomerRole";
}


COrderCustomerRole::COrderCustomerRole():
	m_roleType(RT_ORDERING_PARTY)
{
}


// reimplemented (IOrderCustomerRole)

QByteArray COrderCustomerRole::GetCustomerId() const
{
	return m_customerId;
}


void COrderCustomerRole::SetCustomerId(const QByteArray& customerId)
{
	if (m_customerId != customerId){
		istd::CChangeNotifier changeNotifier(this);

		m_customerId = customerId;
	}
}


IOrderCustomerRole::RoleType COrderCustomerRole::GetRoleType() const
{
	return m_roleType;
}


void COrderCustomerRole::SetRoleType(RoleType roleType)
{
	if (m_roleType != roleType){
		istd::CChangeNotifier changeNotifier(this);

		m_roleType = roleType;
	}
}


// reimplemented (iser::IObject)

QByteArray COrderCustomerRole::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (iser::ISerializable)

bool COrderCustomerRole::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag roleTypeTag("RoleType", "Role Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleTypeTag);
	retVal = retVal && I_SERIALIZE_ENUM(RoleType, archive, m_roleType);
	retVal = retVal && archive.EndTag(roleTypeTag);

	iser::CArchiveTag customerIdTag("CustomerId", "Customer ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(customerIdTag);
	retVal = retVal && archive.Process(m_customerId);
	retVal = retVal && archive.EndTag(customerIdTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int COrderCustomerRole::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool COrderCustomerRole::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const COrderCustomerRole* sourcePtr = dynamic_cast<const COrderCustomerRole*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_customerId = sourcePtr->m_customerId;
		m_roleType = sourcePtr->m_roleType;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr COrderCustomerRole::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new COrderCustomerRole());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool COrderCustomerRole::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_customerId.clear();
	m_roleType = RT_ORDERING_PARTY;

	return true;
}


} // namespace prolifedata


