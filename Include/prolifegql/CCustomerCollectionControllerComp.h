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
		I_ASSIGN(m_permissionIdAttrPtr, "PermissionId", "Permission-ID for show all accounts", true, "ViewAllAccounts");
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
	virtual void SetObjectFilter(const imtgql::CGqlRequest& gqlRequest, const imtbase::CTreeItemModel& objectFilterModel, iprm::CParamsSet& filterParams) const override;

private:
	I_ATTR(QByteArray, m_permissionIdAttrPtr);
};


} // namespace prolifegql


