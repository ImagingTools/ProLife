#pragma once

// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace prolifegql
{


class CDeviceCollectionControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtguigql::CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace prolifegql


