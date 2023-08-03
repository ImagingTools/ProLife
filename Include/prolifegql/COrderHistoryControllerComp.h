#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IIdentifiable.h>
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace prolifegql
{


class COrderHistoryControllerComp: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(COrderHistoryControllerComp)
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
	I_END_COMPONENT

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	void GenerateDifferences(
				imtbase::IObjectCollection& prevOrderProducts,
				imtbase::IObjectCollection& currentOrderProducts,
				QByteArrayList& addProducts,
				QByteArrayList& removedProducts,
				QByteArrayList& updatedProducts) const;

	QByteArray GetProductId(const imtbase::IIdentifiable& identifiablePtr) const;

private:
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
};


} // namespace prolifegql


