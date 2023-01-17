#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>
#include <prolifedata/IProductInfo.h>

#undef GetObject

namespace prolifegql
{


class CProductControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductControllerComp);
		I_ASSIGN(m_productInfoFactCompPtr, "ProductFactory", "Factory used for creation of the new Product instance", true, "ProductFactory");
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

private:
    I_FACT(prolifedata::IProductInfo, m_productInfoFactCompPtr);
};


} // namespace imtlicgql


