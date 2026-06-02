#pragma once


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtserverapp/CTimeFilterParamRepresentationController.h>

// ProLife includes
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Workspace_fwd.h>


namespace prolifegql
{


class CWorkspaceControllerComp: public sdl::V1_0::prolife::CWorkspaceGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::prolife::CWorkspaceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CWorkspaceControllerComp)
		I_ASSIGN(m_softwareCollectionCompPtr, "SoftwareCollection", "Software collection", true, "SoftwareCollection");
		I_ASSIGN(m_hardwareCollectionCompPtr, "HardwareCollection", "Hardware collection", true, "HardwareCollection");
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_accountCollectionCompPtr, "AccountCollection", "Account collection", true, "AccountCollection");
	I_END_COMPONENT

protected:
	enum TimeFilterType
	{
		TFT_BY_CREATION,
		TFT_BY_LICENSE_CREATION,
	};

	virtual sdl::V1_0::prolife::CLineChartData OnGetLicenseCreationInfo(
				const sdl::V1_0::prolife::CGetLicenseCreationInfoGqlRequest& getLicenseCreationInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CBarChartData OnGetSoftwareUsedBarChart(
				const sdl::V1_0::prolife::CGetSoftwareUsedBarChartGqlRequest& getSoftwareUsedBarChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CPieChartData OnGetSoftwareUsedPieChart(
				const sdl::V1_0::prolife::CGetSoftwareUsedPieChartGqlRequest& getSoftwareUsedPieChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CBarChartData OnGetHardwareUsedBarChart(
				const sdl::V1_0::prolife::CGetHardwareUsedBarChartGqlRequest& getHardwareUsedBarChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CPieChartData OnGetHardwareUsedPieChart(
				const sdl::V1_0::prolife::CGetHardwareUsedPieChartGqlRequest& getHardwareUsedPieChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CPieChartData OnGetHardwareStatusInfo(
				const sdl::V1_0::prolife::CGetHardwareStatusInfoGqlRequest& getHardwareStatusInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CTotalSummaryInfo OnGetTotalSummaryInfo(
				const sdl::V1_0::prolife::CGetTotalSummaryInfoGqlRequest& getTotalSummaryInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CPieChartData OnGetHardwareCustomerPieChart(
				const sdl::V1_0::prolife::CGetHardwareCustomerPieChartGqlRequest& getHardwareCustomerPieChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CPieChartData OnGetSoftwareCustomerPieChart(
				const sdl::V1_0::prolife::CGetSoftwareCustomerPieChartGqlRequest& getSoftwareCustomerPieChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CPieChartData OnGetHardwareConfigurationPieChart(
				const sdl::V1_0::prolife::CGetHardwareConfigurationPieChartGqlRequest& getHardwareConfigurationPieChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CBarChartData OnGetSoftwareCreationBarChart(
				const sdl::V1_0::prolife::CGetSoftwareCreationBarChartGqlRequest& getSoftwareCreationBarChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CBarChartData OnGetHardwareCreationBarChart(
				const sdl::V1_0::prolife::CGetHardwareCreationBarChartGqlRequest& getHardwareCreationBarChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CLineChartData OnGetOrderCreationLineChart(
				const sdl::V1_0::prolife::CGetOrderCreationLineChartGqlRequest& getOrderCreationLineChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	sdl::V1_0::prolife::CBarChartData GetItemsCreationBarChart(
				const ::imtgql::CGqlRequest& gqlRequest,
				const imtbase::IObjectCollection& collection,
				const sdl::V1_0::prolife::CChartInput& chartInput,
				int nameMetaInfoType,
				QString& errorMessage) const;
	bool BuildPieChart(
				const QMap<QPair<QByteArray, QString>, int>& map,
				sdl::V1_0::prolife::CPieChartData& pieChartData) const;
	bool BuildBarChart(const QMap<QDate, QMap<QString, int>>& map,
				const sdl::V1_0::prolife::CChartInput& input,
				const QString& yLabel,
				sdl::V1_0::prolife::CBarChartData& barChartData) const;
	bool BuildLineChart(
				const QMap<QDate, int>& map,
				const sdl::V1_0::prolife::CChartInput& input,
				const QString& yLabel,
				sdl::V1_0::prolife::CLineChartData& lineChartData) const;
	bool JoinGroupFilter(const imtgql::IGqlRequest& gqlRequest, iprm::CParamsSet& filterParam) const;
	uint FNV1A(const QByteArray& data) const;
	QString GenerateColorFromString(const QString& text) const;
	void AddFieldFilter(iprm::CParamsSet& paramsSet, const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter) const;
	sdl::V1_0::prolife::CChartSegment CreateChartSegment(int value, const QString& label, const QString& color, const QByteArray& segmentId = QByteArray()) const;
	sdl::V1_0::prolife::CBarChartData BuildProductUsageBarChart(
				const imtbase::IObjectCollection& collection,
				int productNameMetaInfoType,
				const sdl::V1_0::prolife::CChartInput& input,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const;
	sdl::V1_0::prolife::CPieChartData BuildProductUsagePieChart(
				const imtbase::IObjectCollection& collection,
				int productNameMetaInfoType,
				const sdl::V1_0::prolife::CChartInput& input,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const;
	sdl::V1_0::prolife::CPieChartData BuildProductByCustomerPieChart(
				const imtbase::IObjectCollection& collection,
				const sdl::V1_0::prolife::CChartInput& input,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const;
	void AggregateByTime(
				const imtbase::ITimeFilterParam::TimeUnit& unit,
				const imtbase::ITimeFilterParam::InterpretationMode& timeMode,
				const std::function<void(const QDate& start, const QDate& end)>& fn) const;
	void PrepareFilters(
				iprm::CParamsSet& paramsSet,
				const sdl::V1_0::prolife::CChartInput& input,
				const ::imtgql::CGqlRequest& gqlRequest,
				bool joinGroupFilter,
				std::optional<bool> internalUse,
				std::optional<bool> inUse,
				const TimeFilterType& timeFilterType) const;
	void ExtractTimeUnitFromInput(const sdl::V1_0::prolife::CChartInput& input, imtbase::ITimeFilterParam::TimeUnit& unit, imtbase::ITimeFilterParam::InterpretationMode& timeMode) const;

private:
	I_REF(imtbase::IObjectCollection, m_softwareCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_hardwareCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_accountCollectionCompPtr);

private:
	imtserverapp::CTimeFilterParamRepresentationController m_timeFilterParamRepresentationController;
	mutable QHash<QString, QString> m_colorCache;
};


} // namespace prolifegql


