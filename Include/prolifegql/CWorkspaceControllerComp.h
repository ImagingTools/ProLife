#pragma once


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/CTimeFilterParam.h>
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
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_userActionCollectionCompPtr, "UserActionCollection", "User action collection", true, "UserActionCollection");
	I_END_COMPONENT

protected:
	virtual sdl::prolife::Workspace::CLineChartData OnGetLicenseCreationInfo(
				const sdl::prolife::Workspace::CGetLicenseCreationInfoGqlRequest& getLicenseCreationInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CBarChartData OnGetSoftwareUsedBarChart(
				const sdl::prolife::Workspace::CGetSoftwareUsedBarChartGqlRequest& getSoftwareUsedBarChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CPieChartData OnGetSoftwareUsedPieChart(
				const sdl::prolife::Workspace::CGetSoftwareUsedPieChartGqlRequest& getSoftwareUsedPieChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CBarChartData OnGetHardwareUsedBarChart(
				const sdl::prolife::Workspace::CGetHardwareUsedBarChartGqlRequest& getHardwareUsedBarChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CPieChartData OnGetHardwareUsedPieChart(
				const sdl::prolife::Workspace::CGetHardwareUsedPieChartGqlRequest& getHardwareUsedPieChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CPieChartData OnGetHardwareStatusInfo(
				const sdl::prolife::Workspace::CGetHardwareStatusInfoGqlRequest& getHardwareStatusInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CTotalSummaryInfo OnGetTotalSummaryInfo(
				const sdl::prolife::Workspace::CGetTotalSummaryInfoGqlRequest& getTotalSummaryInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	bool PrepareDateFilter(
				const sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& timeFilterSdl,
				QDate& startDate,
				QDate& endDate,
				imtbase::ITimeFilterParam::TimeUnit& unit,
				imtbase::ITimeFilterParam::InterpretationMode& mode) const;
	bool BuildBarChart(const QMap<QDate, QMap<QString, int>>& map,
					const sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& timeFilterSdl,
					const QString& yLabel,
					sdl::prolife::Workspace::CBarChartData::V1_0& barChartData) const;
	QByteArrayList GetUserActionsByCreateLicenseFile(
				const imtgql::CGqlRequest& gqlRequest,
				const sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& timeFilterSdl) const;
	QString GetProductNameForHardware(const QByteArray& hardwareId) const;
	QString GetProductNameForSoftware(const QByteArray& softwareId) const;
	bool JoinGroupFilter(const imtgql::IGqlRequest& gqlRequest, iprm::CParamsSet& filterParam) const;

private:
	I_REF(imtbase::IObjectCollection, m_softwareCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_hardwareCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userActionCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);

private:
	imtserverapp::CTimeFilterParamRepresentationController m_timeFilterParamRepresentationController;
};


} // namespace prolifegql


