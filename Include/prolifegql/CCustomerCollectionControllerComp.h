#pragma once


// ImtCore includes
#include <imtauth/ICompanyInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Accounts.h>

// ProLife includes
#include <prolifedata/IGroupFilterParamJoiner.h>


namespace prolifegql
{


class CCustomerCollectionControllerComp: public sdl::prolife::Accounts::CAccountCollectionControllerCompBase
{
public:
	typedef sdl::prolife::Accounts::CAccountCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCustomerCollectionControllerComp);
		I_ASSIGN(m_accountInfoFactCompPtr, "AccountFactory", "Factory used for creation of the new account instance", true, "AccountFactory");
		I_ASSIGN(m_groupFilterParamJoinerCompPtr, "GroupFilterParamJoiner", "Group filter param joiner", true, "GroupFilterParamJoiner");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::prolife::Accounts::CAccountCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::prolife::Accounts::CAccountsListGqlRequest& accountsListRequest,
				sdl::prolife::Accounts::CAccountItem::V1_0& representationObject,
				QString& errorMessage) const override;

	// Account methods
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::prolife::Accounts::CAccountData::V1_0& accountDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::prolife::Accounts::CAccountItemGqlRequest& accountItemRequest,
				sdl::prolife::Accounts::CAccountData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::prolife::Accounts::CAccountUpdateGqlRequest& accountUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest,const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet* filterParams) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::prolife::Accounts::CAccountData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;
	const iprm::IParamsSet* CreateComplexFilter(const QByteArray& fieldId, const QByteArray& fieldValue) const;

private:
	I_FACT(imtauth::ICompanyInfo, m_accountInfoFactCompPtr);
	I_REF(prolifedata::IGroupFilterParamJoiner, m_groupFilterParamJoinerCompPtr);
};


} // namespace prolifegql


