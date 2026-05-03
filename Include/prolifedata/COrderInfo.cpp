#include <prolifedata/COrderInfo.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/IArchive.h>
#include <istd/CChangeGroup.h>
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtcore/Version.h>
#include <imtbase/CObjectLink.h>
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtlic/CProductInstanceInfo.h>

// ProLife include
#include <prolife/Version.h>
#include <prolifedata/COrderCustomerRole.h>


namespace prolifedata
{


// public methods

QByteArray COrderInfo::GetTypeId()
{
	return "OrderInfo";
}


// public methods

COrderInfo::COrderInfo():
	m_status(OS_CREATED)
{
	typedef istd::TSingleFactory<istd::IChangeable, imtlic::CIdentifiableSoftwareInstanceInfo> FactorySoftwareImpl;
	m_productInstanceCollection.RegisterFactory<FactorySoftwareImpl>("Software");

	typedef istd::TSingleFactory<istd::IChangeable, imtlic::CIdentifiableHardwareInstanceInfo> FactoryHardwareImpl;
	m_productInstanceCollection.RegisterFactory<FactoryHardwareImpl>("Hardware");

	typedef istd::TSingleFactory<istd::IChangeable, imtbase::CObjectLink> FactorySoftwareInfoImpl;
	m_productInstanceCollection.RegisterFactory<FactorySoftwareInfoImpl>("SoftwareInfo");

	typedef istd::TSingleFactory<istd::IChangeable, imtbase::CObjectLink> FactoryHardwareInfoImpl;
	m_productInstanceCollection.RegisterFactory<FactoryHardwareInfoImpl>("HardwareInfo");

	typedef istd::TSingleFactory<istd::IChangeable, COrderCustomerRole> FactoryCustomerRoleImpl;
	m_customerRoles.RegisterFactory<FactoryCustomerRoleImpl>(COrderCustomerRole::GetTypeId());
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


QByteArray COrderInfo::GetPurchaseOrderId() const
{
	return m_purchaseId;
}


void COrderInfo::SetPurchaseOrderId(const QByteArray& purchaseOrderId)
{
	if (m_purchaseId != purchaseOrderId){
		istd::CChangeNotifier changeNotifier(this);

		m_purchaseId = purchaseOrderId;
	}
}


// reimplemented (IOrderInfo)

QByteArray COrderInfo::GetCustomerId() const
{
	// Proxy: return the customerId of the RT_ORDERING_PARTY role
	imtbase::ICollectionInfo::Ids roleIds = m_customerRoles.GetElementIds();
	for (const imtbase::ICollectionInfo::Id& roleId : roleIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_customerRoles.GetObjectData(roleId, dataPtr)){
			const COrderCustomerRole* rolePtr = dynamic_cast<const COrderCustomerRole*>(dataPtr.GetPtr());
			if (rolePtr != nullptr && rolePtr->GetRoleType() == IOrderCustomerRole::RT_ORDERING_PARTY){
				return rolePtr->GetCustomerId();
			}
		}
	}

	return m_customerId;
}


void COrderInfo::SetCustomerId(const QByteArray& customerId)
{
	// Proxy: set the customerId of the RT_ORDERING_PARTY role
	imtbase::ICollectionInfo::Ids roleIds = m_customerRoles.GetElementIds();
	for (const imtbase::ICollectionInfo::Id& roleId : roleIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_customerRoles.GetObjectData(roleId, dataPtr)){
			COrderCustomerRole* rolePtr = dynamic_cast<COrderCustomerRole*>(dataPtr.GetPtr());
			if (rolePtr != nullptr && rolePtr->GetRoleType() == IOrderCustomerRole::RT_ORDERING_PARTY){
				rolePtr->SetCustomerId(customerId);
				if (m_customerId != customerId){
					istd::CChangeNotifier changeNotifier(this);
					m_customerId = customerId;
				}
				return;
			}
		}
	}

	// No ordering party role exists yet — create one
	COrderCustomerRole* newRole = new COrderCustomerRole();
	newRole->SetCustomerId(customerId);
	newRole->SetRoleType(IOrderCustomerRole::RT_ORDERING_PARTY);

	istd::TDelPtr<COrderCustomerRole> newRolePtr(newRole);
	m_customerRoles.InsertNewObject(
		COrderCustomerRole::GetTypeId(), "", "",
		newRolePtr.GetPtr(),
		QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8());

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


imtbase::CObjectCollection* COrderInfo::GetCustomerRoles()
{
	return &m_customerRoles;
}


const imtbase::CObjectCollection* COrderInfo::GetCustomerRoles() const
{
	return &m_customerRoles;
}


QByteArray COrderInfo::GetFactoryId() const
{
	return QByteArray();
}


// reimplemented (iser::ISerializable)

bool COrderInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	// Get version of ProLife:
	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();
	quint32 prolifeVersion;
	if (!versionInfo.GetVersionNumber(prolife::VI_PROLIFE, prolifeVersion)){
		prolifeVersion = 5902;
	}

	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	bool retVal = true;

	// Serialize order with the new format:
	iser::CArchiveTag statusTag("Status", "Order status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	if (prolifeVersion >= 5902){
		retVal = retVal && I_SERIALIZE_ENUM(OrderStatus, archive, m_status);
	}
	// Serialize order with the old format:
	else{
		QString status;
		retVal = retVal && archive.Process(status);
	}
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag orderIdTag("OrderId", "User-defined order-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(orderIdTag);
	retVal = retVal && archive.Process(m_orderId);
	retVal = retVal && archive.EndTag(orderIdTag);

	if (imtCoreVersion > 6630){
		iser::CArchiveTag purchaseOrderIdTag("PurchaseId", "User-defined purchase order-ID", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(purchaseOrderIdTag);
		retVal = retVal && archive.Process(m_purchaseId);
		retVal = retVal && archive.EndTag(purchaseOrderIdTag);
	}

	// Legacy field: always write for backward compatibility, always read
	iser::CArchiveTag orderCustomerTag("OrderCustomer", "Order Customer", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(orderCustomerTag);
	retVal = retVal && archive.Process(m_customerId);
	retVal = retVal && archive.EndTag(orderCustomerTag);

	iser::CArchiveTag orderDescriptionTag("Description", "Order Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(orderDescriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(orderDescriptionTag);

	iser::CArchiveTag productsTag("Products", "Products in the order", iser::CArchiveTag::TT_GROUP);
	if (prolifeVersion >= 5902){
		retVal = retVal && archive.BeginTag(productsTag);
		retVal = retVal && m_productInstanceCollection.Serialize(archive);
		retVal = retVal && archive.EndTag(productsTag);
	}

	// CustomerRoles: new section (prolifeVersion >= 6000)
	iser::CArchiveTag customerRolesTag("CustomerRoles", "Customer roles for the order", iser::CArchiveTag::TT_GROUP);
	if (prolifeVersion >= 6000){
		if (archive.IsStoring()){
			// Ensure at least one RT_ORDERING_PARTY role exists when storing
			bool hasOrderingParty = false;
			imtbase::ICollectionInfo::Ids roleIds = m_customerRoles.GetElementIds();
			for (const imtbase::ICollectionInfo::Id& roleId : roleIds){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_customerRoles.GetObjectData(roleId, dataPtr)){
					const COrderCustomerRole* rolePtr = dynamic_cast<const COrderCustomerRole*>(dataPtr.GetPtr());
					if (rolePtr != nullptr && rolePtr->GetRoleType() == IOrderCustomerRole::RT_ORDERING_PARTY){
						hasOrderingParty = true;
						break;
					}
				}
			}

			if (!hasOrderingParty && !m_customerId.isEmpty()){
				COrderCustomerRole* newRole = new COrderCustomerRole();
				newRole->SetCustomerId(m_customerId);
				newRole->SetRoleType(IOrderCustomerRole::RT_ORDERING_PARTY);

				istd::TDelPtr<COrderCustomerRole> newRolePtr(newRole);
				m_customerRoles.InsertNewObject(
					COrderCustomerRole::GetTypeId(), "", "",
					newRolePtr.GetPtr(),
					QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8());
			}
		}

		retVal = retVal && archive.BeginTag(customerRolesTag);
		retVal = retVal && m_customerRoles.Serialize(archive);
		retVal = retVal && archive.EndTag(customerRolesTag);

		if (!archive.IsStoring()){
			// Sync m_customerId from the RT_ORDERING_PARTY role after loading
			imtbase::ICollectionInfo::Ids roleIds = m_customerRoles.GetElementIds();
			for (const imtbase::ICollectionInfo::Id& roleId : roleIds){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_customerRoles.GetObjectData(roleId, dataPtr)){
					const COrderCustomerRole* rolePtr = dynamic_cast<const COrderCustomerRole*>(dataPtr.GetPtr());
					if (rolePtr != nullptr && rolePtr->GetRoleType() == IOrderCustomerRole::RT_ORDERING_PARTY){
						m_customerId = rolePtr->GetCustomerId();
						break;
					}
				}
			}
		}
	}
	else if (!archive.IsStoring()){
		// Old format: create a single RT_ORDERING_PARTY role from the legacy m_customerId
		if (!m_customerId.isEmpty()){
			COrderCustomerRole* newRole = new COrderCustomerRole();
			newRole->SetCustomerId(m_customerId);
			newRole->SetRoleType(IOrderCustomerRole::RT_ORDERING_PARTY);

			istd::TDelPtr<COrderCustomerRole> newRolePtr(newRole);
			m_customerRoles.InsertNewObject(
				COrderCustomerRole::GetTypeId(), "", "",
				newRolePtr.GetPtr(),
				QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8());
		}
	}

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
		m_purchaseId = sourcePtr->m_purchaseId;
		m_customerId = sourcePtr->m_customerId;
		m_description = sourcePtr->m_description;
		m_status = sourcePtr->m_status;
		m_productInstanceCollection.CopyFrom(sourcePtr->m_productInstanceCollection);
		m_customerRoles.CopyFrom(sourcePtr->m_customerRoles);

		bool retVal = true;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr COrderInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new COrderInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool COrderInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_orderId.clear();
	m_purchaseId.clear();
	m_customerId.clear();
	m_productInstanceCollection.ResetData();
	m_customerRoles.ResetData();
	m_status = OS_CREATED;

	return true;
}


} // namespace prolifedata


