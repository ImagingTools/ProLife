#pragma once

// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>


namespace prolifegql
{


class COrderCollectionControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(COrderCollectionControllerComp);
		I_ASSIGN(m_accountCollectionCompPtr, "AccountCollection", "Account collection", true, "AccountCollection");
		I_ASSIGN(m_permissionIdAttrPtr, "PermissionId", "Permission ID for show all orders", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual bool SetupGqlItem(
				const imtgql::CGqlRequest& gqlRequest,
				imtbase::CTreeItemModel& model,
				int itemIndex,
				const imtbase::IObjectCollectionIterator* objectCollectionIterator,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_accountCollectionCompPtr);
	I_ATTR(QByteArray, m_permissionIdAttrPtr);
};


} // namespace prolifegql


