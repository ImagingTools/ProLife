#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


class CDeviceControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceControllerComp)
		I_ASSIGN(m_deviceCompPtr, "DeviceFactory", "Factory used for creation of the new device instance", true, "DeviceFactory");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Remote product collection", true, "ProductCollection");
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_orderOperationContextControllerCompPtr, "OrderOperationContextController", "Order operation context controller", true, "OrderOperationContextController");
	I_END_COMPONENT

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const imtgql::CGqlRequest& gqlRequest, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

private:
	I_FACT(prolifedata::IDeviceInfo, m_deviceCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtgql::IOperationContextController, m_orderOperationContextControllerCompPtr);
};


} // namespace prolifegql


