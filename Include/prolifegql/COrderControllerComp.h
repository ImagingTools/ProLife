#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <imtbase/IIdentifiable.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>


#undef GetObject


namespace prolifegql
{


class COrderControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(COrderControllerComp)
		I_ASSIGN(m_orderPtr, "OrderFactory", "Factory used for creation of the new order instance", true, "OrderFactory");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Hardware product binding collection", true, "BindingCollection");
		I_ASSIGN(m_softwareInstanceCollectionCompPtr, "SoftwareInstanceCollection", "Software instance collection", true, "SoftwareInstanceCollection");
	I_END_COMPONENT

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& name, QString& description, QString& errorMessage) const override;

	virtual void InsertSoftwareProductToProductCollection(
				const imtgql::CGqlRequest& gqlRequest,
				const imtbase::CTreeItemModel& softwareProductModel,
				int modelIndex, imtbase::IObjectCollection& productCollection,
				const QByteArray& orderUuid,
				QString& errorMessage) const;
	virtual void InsertHardwareProductToProductCollection(
				const imtgql::CGqlRequest& gqlRequest,
				const imtbase::CTreeItemModel& hardwareProductModel,
				int modelIndex,
				imtbase::IObjectCollection& productCollection,
				const QByteArray& orderUuid,
				QString& errorMessage) const;

	virtual void InsertSoftwareProductToModel(const imtbase::IIdentifiable& identifiable, imtbase::CTreeItemModel& softwareProductModel) const;
	virtual void InsertHardwareProductToModel(const imtbase::IIdentifiable& identifiable, imtbase::CTreeItemModel& hardwareProductModel) const;

	void GenerateDifferences(
				prolifedata::IOrderInfo& currentOrder,
				prolifedata::IOrderInfo& newOrder,
				QByteArrayList& addedProducts,
				QByteArrayList& removedProducts,
				QByteArrayList& updatedProducts) const;

private:
	I_FACT(prolifedata::IOrderInfo, m_orderPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareInstanceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
};


} // namespace prolifegql


