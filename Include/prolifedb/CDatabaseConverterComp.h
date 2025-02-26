#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdb/CMigrationControllerCompBase.h>


namespace prolifedb
{


class CDatabaseConverterComp: virtual public imtdb::CMigrationControllerCompBase
{
public:
	typedef imtdb::CMigrationControllerCompBase BaseClass;
	
	I_BEGIN_COMPONENT(CDatabaseConverterComp)
	I_ASSIGN(m_softwareInstanceCollectionCompPtr, "SoftwareInstanceCollection", "SoftwareInstanceCollection", true, "SoftwareInstanceCollection");
	I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
	I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
	I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Lisa product collection", true, "ProductCollection");
	I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "Lisa license collection", true, "LicenseCollection");
	I_END_COMPONENT
		
protected:
	// reimplemented (imtdb::IMigrationController)
	virtual bool DoMigration(int& resultRevision, const istd::CIntRange& subRange = istd::CIntRange()) const override;
	
private:
	I_REF(imtbase::IObjectCollection, m_softwareInstanceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
};


} // namespace prolifedb


