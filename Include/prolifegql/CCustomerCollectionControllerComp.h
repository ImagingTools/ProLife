#pragma once


// ImtCore includes
#include <imtauth/ICompanyInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Accounts.h>


namespace prolifegql
{


class CCustomerCollectionControllerComp: public sdl::prolife::Accounts::V1_0::CAccountCollectionControllerCompBase
{
public:
	typedef sdl::prolife::Accounts::V1_0::CAccountCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCustomerCollectionControllerComp);
		I_ASSIGN(m_permissionIdAttrPtr, "PermissionId", "Permission-ID for show all accounts", true, "ViewAllAccounts");
		I_ASSIGN(m_accountInfoFactCompPtr, "AccountFactory", "Factory used for creation of the new account instance", true, "AccountFactory");
	I_END_COMPONENT;

protected:

	// reimplemented (sdl::prolife::Accounts::V1_0::CAccountCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::prolife::Accounts::V1_0::CAccountsListGqlRequest& accountsListRequest,
				sdl::prolife::Accounts::V1_0::CAccountItem& representationObject,
				QString& errorMessage) const override;

	// Account methods
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::prolife::Accounts::V1_0::CAccountData& accountDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::prolife::Accounts::V1_0::CAccountItemGqlRequest& accountItemRequest,
				sdl::prolife::Accounts::V1_0::CAccountDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual void SetObjectFilter(const imtgql::CGqlRequest& gqlRequest, const imtbase::CTreeItemModel& objectFilterModel, iprm::CParamsSet& filterParams) const override;

private:
	I_FACT(imtauth::ICompanyInfo, m_accountInfoFactCompPtr);
	I_ATTR(QByteArray, m_permissionIdAttrPtr);
};


} // namespace prolifegql


