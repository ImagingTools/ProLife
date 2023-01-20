#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>
#include <prolifedata/IOrderInfo.h>
#include <imtbase/IMetaInfoCreator.h>


namespace prolifegql
{


class COrderControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(COrderControllerComp)
		I_ASSIGN(m_orderPtr, "OrderFactory", "Factory used for creation of the new order instance", true, "OrderFactory");
//		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
//		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Meta information creator for the product instance", true, "MetaInfoCreator");
	I_END_COMPONENT

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

private:
	I_FACT(prolifedata::IOrderInfo, m_orderPtr);
//	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
//	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);
};


} // namespace prolifegql


