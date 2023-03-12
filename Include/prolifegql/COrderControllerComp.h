#pragma once


// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtguigql/CObjectCollectionControllerCompBase.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>


#undef GetObject


namespace prolifegql
{


class COrderControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(COrderControllerComp)
		I_ASSIGN(m_orderPtr, "OrderFactory", "Factory used for creation of the new order instance", true, "OrderFactory");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
	I_END_COMPONENT

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

	virtual void InsertSoftwareProductToProductCollection(const imtbase::CTreeItemModel& softwareProductModel, int modelIndex, imtbase::IObjectCollection& productCollection) const;
	virtual void InsertHardwareProductToProductCollection(const imtbase::CTreeItemModel& hardwareProductModel, int modelIndex, imtbase::IObjectCollection& productCollection, const QByteArray& orderId = QByteArray()) const;

private:
	I_FACT(prolifedata::IOrderInfo, m_orderPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
};


} // namespace prolifegql


