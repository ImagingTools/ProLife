#pragma once


// ImtCore includes
#include <imtauth/ICompanyInfo.h>
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/prolifesdl/SDL/CPP/Accounts/AccountsAPIv1_0.h>


namespace prolifegql
{


class CCustomerCollectionControllerComp: public prolife::sdl::Accounts::CAccountCollectionControllerCompBase
{
public:
	typedef prolife::sdl::Accounts::CAccountCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCustomerCollectionControllerComp);
		I_ASSIGN(m_permissionIdAttrPtr, "PermissionId", "Permission-ID for show all accounts", true, "ViewAllAccounts");
		I_ASSIGN(m_accountInfoFactCompPtr, "AccountFactory", "Factory used for creation of the new account instance", true, "AccountFactory");
	I_END_COMPONENT;

protected:

	// reimplemented (prolife::sdl::Accounts::CAccountCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const prolife::sdl::Accounts::CAccountsListGqlRequest& accountsListRequest,
				prolife::sdl::Accounts::CAccountItem& representationObject,
				QString& errorMessage) const override;

	// Account methods
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const prolife::sdl::Accounts::CAccountData& accountDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const prolife::sdl::Accounts::CAccountItemGqlRequest& accountItemRequest,
				prolife::sdl::Accounts::CAccountDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual void SetObjectFilter(const imtgql::CGqlRequest& gqlRequest, const imtbase::CTreeItemModel& objectFilterModel, iprm::CParamsSet& filterParams) const override;

private:
	I_FACT(imtauth::ICompanyInfo, m_accountInfoFactCompPtr);
	I_ATTR(QByteArray, m_permissionIdAttrPtr);
};


} // namespace prolifegql


