#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtserverapp/CTimeFilterParamRepresentationController.h>

// ProLife includes
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Workspace.h>


namespace prolifegql
{


class CWorkspaceControllerComp: public sdl::prolife::Workspace::CGraphQlHandlerCompBase
{
public:
	typedef sdl::prolife::Workspace::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CWorkspaceControllerComp)
		I_ASSIGN(m_softwareCollectionCompPtr, "SoftwareCollection", "Software collection", true, "SoftwareCollection");
		I_ASSIGN(m_hardwareCollectionCompPtr, "HardwareCollection", "Hardware collection", true, "HardwareCollection");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_userActionCollectionCompPtr, "UserActionCollection", "User action collection", true, "UserActionCollection");
	I_END_COMPONENT

protected:
	virtual sdl::prolife::Workspace::CLineChartData OnGetLicenseCreationInfo(
				const sdl::prolife::Workspace::CGetLicenseCreationInfoGqlRequest& getLicenseCreationInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CPieChartData OnGetLicenseProductStats(
				const sdl::prolife::Workspace::CGetLicenseProductStatsGqlRequest& getLicenseProductStatsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
		virtual sdl::prolife::Workspace::CPieChartData OnGetHardwareStatusInfo(
				const sdl::prolife::Workspace::CGetHardwareStatusInfoGqlRequest& getHardwareStatusInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CTotalSummaryInfo OnGetTotalSummaryInfo(
				const sdl::prolife::Workspace::CGetTotalSummaryInfoGqlRequest& getTotalSummaryInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_softwareCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_hardwareCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userActionCollectionCompPtr);

private:
	imtserverapp::CTimeFilterParamRepresentationController m_timeFilterParamRepresentationController;
};


} // namespace prolifegql


