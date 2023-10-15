#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace prolifedb
{


class CDatabaseConverterComp: public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseConverterComp)
		I_ASSIGN(m_softwareInstanceCollectionCompPtr, "SoftwareInstanceCollection", "SoftwareInstanceCollection", true, "SoftwareInstanceCollection");
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Lisa product collection", true, "ProductCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "Lisa license collection", true, "LicenseCollection");
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	virtual QByteArray GetLicenseUuidByLicenseId(const QByteArray& licenseId) const;
	virtual QByteArray GetProductUuidByProductId(const QByteArray& productId) const;

private:
	I_REF(imtbase::IObjectCollection, m_softwareInstanceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
};


} // namespace prolifedb


