#pragma once


// ImtCore includes
#include <imtauth/ICompanyInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Accounts_fwd.h>

// ProLife includes
#include <prolifedata/IGroupFilterParamJoiner.h>


namespace prolifegql
{


class CCustomerCollectionControllerComp: public sdl::V1_0::prolife::CAccountCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::prolife::CAccountCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCustomerCollectionControllerComp);
		I_ASSIGN(m_accountInfoFactCompPtr, "AccountFactory", "Factory used for creation of the new account instance", true, "AccountFactory");
		I_ASSIGN(m_groupFilterParamJoinerCompPtr, "GroupFilterParamJoiner", "Group filter param joiner", true, "GroupFilterParamJoiner");
		I_ASSIGN(m_softwareCollectionCompPtr, "SoftwareCollection", "Software collection", false, "SoftwareCollection");
	I_END_COMPONENT;

protected:
	virtual void OnAfterSetObjectDescription(
				const QByteArray& objectId,
				const QString& description,
				const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool IsDescriptionStoredInDocument() const override;
	virtual bool OnBeforeRemoveElements(
				const QByteArrayList& elementIds,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::V1_0::prolife::CAccountCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::prolife::CAccountsListGqlRequest& accountsListRequest,
				sdl::V1_0::prolife::CAccountItem& representationObject,
				QString& errorMessage) const override;

	// Account methods
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::prolife::CAccountData& accountDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::prolife::CAccountItemGqlRequest& accountItemRequest,
				sdl::V1_0::prolife::CAccountData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::prolife::CAccountUpdateGqlRequest& accountUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest,const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet* filterParams) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::V1_0::prolife::CAccountData& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;
	const iprm::IParamsSet* CreateComplexFilter(const QByteArray& fieldId, const QByteArray& fieldValue) const;

private:
	I_FACT(imtauth::ICompanyInfo, m_accountInfoFactCompPtr);
	I_REF(prolifedata::IGroupFilterParamJoiner, m_groupFilterParamJoinerCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareCollectionCompPtr);
};


} // namespace prolifegql


