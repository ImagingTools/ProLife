#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>


#undef GetObject


namespace prolifegql
{


class CHardwareProductBindingControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CHardwareProductBindingControllerComp)
	I_END_COMPONENT

protected:
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace prolifegql


