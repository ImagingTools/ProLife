#pragma once


// ImtCore includes
#include <imtaccount/ICompanyInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Accounts_fwd.h>

// ProLife includes
#include <prolifedata/IGroupFilterParamJoiner.h>


namespace prolifegql
{


/**
	GraphQL access to the companies of one page: the customers or the vendors (manufacturers and suppliers).
	All companies are stored together; CompanyRole tells which of them this collection shows.
*/
class CCustomerCollectionControllerComp: public sdl::V1_0::prolife::CAccountCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::prolife::CAccountCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCustomerCollectionControllerComp);
		I_ASSIGN(m_accountInfoFactCompPtr, "AccountFactory", "Factory used for creation of the new account instance", true, "AccountFactory");
		I_ASSIGN(m_groupFilterParamJoinerCompPtr, "GroupFilterParamJoiner", "Group filter param joiner", true, "GroupFilterParamJoiner");
		I_ASSIGN(m_softwareCollectionCompPtr, "SoftwareCollection", "Software collection", false, "SoftwareCollection");
		I_ASSIGN(m_companyRoleAttrPtr, "CompanyRole", "Companies of the collection: 'Customer' (customers) or 'Vendor' (manufacturers and suppliers)", true, "Customer");
	I_END_COMPONENT;

protected:
	bool IsVendorCollection() const;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual void AddFixedFilters(const imtgql::CGqlRequest& gqlRequest, imtbase::IComplexCollectionFilter::FilterExpression& fixedExpression) const override;
	virtual QByteArray GetObjectTypeIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CVisualStatus OnGetObjectVisualStatus(
				const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

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
	I_FACT(imtaccount::ICompanyInfo, m_accountInfoFactCompPtr);
	I_REF(prolifedata::IGroupFilterParamJoiner, m_groupFilterParamJoinerCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareCollectionCompPtr);
	I_ATTR(QByteArray, m_companyRoleAttrPtr);
};


} // namespace prolifegql


