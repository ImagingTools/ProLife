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
	I_END_COMPONENT

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

private:
	I_FACT(prolifedata::IDeviceInfo, m_deviceCompPtr);
};


} // namespace prolifegql


