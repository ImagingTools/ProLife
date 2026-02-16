#pragma once


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>

// ProLife includes
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Licenses.h>


namespace prolifegql
{


class CSoftwareControllerComp: public sdl::prolife::Licenses::CGraphQlHandlerCompBase
{
public:
	typedef sdl::prolife::Licenses::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSoftwareControllerComp)
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
		I_ASSIGN(m_hardwareBindingCollectionCompPtr, "HardwareBindingCollection", "Hardware binding collection", true, "HardwareBindingCollection");
		I_ASSIGN(m_accountCollectionCompPtr, "AccountCollection", "Account collection", true, "AccountCollection");
		I_ASSIGN(m_softwareInfoFactCompPtr, "SoftwareFactory", "Factory for software instance", true, "SoftwareFactory");
		I_ASSIGN(m_softwareOperationContextControllerCompPtr, "SoftwareOperationContextController", "Software operation context controller", true, "SoftwareOperationContextController");
	I_END_COMPONENT

protected:
	// reimplemented (sdl::prolife::Licenses::CGraphQlHandlerCompBase)
	virtual sdl::prolife::Licenses::CSplitLicensePayload OnSplitLicense(
				const sdl::prolife::Licenses::CSplitLicenseGqlRequest& splitLicenseRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Licenses::CChildLicensesListPayload OnChildLicensesList(
				const sdl::prolife::Licenses::CChildLicensesListGqlRequest& childLicensesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Licenses::CRevokeLicensePayload OnRevokeLicense(
				const sdl::prolife::Licenses::CRevokeLicenseGqlRequest& revokeLicenseRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_hardwareBindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_accountCollectionCompPtr);
	I_FACT(imtlic::IProductInstanceInfo, m_softwareInfoFactCompPtr);
	I_REF(imtbase::IOperationContextController, m_softwareOperationContextControllerCompPtr);
};


} // namespace prolifegql


