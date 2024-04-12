#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>


namespace prolifegql
{


class CCustomerCollectionControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCustomerCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool SetupGqlItem(
				const imtgql::CGqlRequest& gqlRequest,
				imtbase::CTreeItemModel& model,
				int itemIndex,
				const imtbase::IObjectCollectionIterator* objectCollectionIterator,
				QString& errorMessage) const override;
};


} // namespace prolifegql


