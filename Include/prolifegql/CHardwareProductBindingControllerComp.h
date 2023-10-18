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
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
	I_END_COMPONENT

protected:
	virtual istd::IChangeable* CreateObject(const imtgql::CGqlRequest& gqlRequest, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
};


} // namespace prolifegql


