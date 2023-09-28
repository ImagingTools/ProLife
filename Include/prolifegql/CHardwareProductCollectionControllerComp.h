#pragma once

// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>


namespace prolifegql
{


class CHardwareProductCollectionControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CHardwareProductCollectionControllerComp);
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

private:
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
};


} // namespace prolifegql


