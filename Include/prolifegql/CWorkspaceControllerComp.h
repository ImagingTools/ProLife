// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#pragma once


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtbase/CComplexCollectionFilter.h>
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
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_accountCollectionCompPtr, "AccountCollection", "Account collection", true, "AccountCollection");
	I_END_COMPONENT

protected:
	enum TimeFilterType
	{
		TFT_BY_CREATION,
		TFT_BY_LICENSE_CREATION,
	};

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
	virtual sdl::prolife::Workspace::CPieChartData OnGetHardwareCustomerPieChart(
				const sdl::prolife::Workspace::CGetHardwareCustomerPieChartGqlRequest& getHardwareCustomerPieChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CPieChartData OnGetSoftwareCustomerPieChart(
				const sdl::prolife::Workspace::CGetSoftwareCustomerPieChartGqlRequest& getSoftwareCustomerPieChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CPieChartData OnGetHardwareConfigurationPieChart(
				const sdl::prolife::Workspace::CGetHardwareConfigurationPieChartGqlRequest& getHardwareConfigurationPieChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CBarChartData OnGetSoftwareCreationBarChart(
				const sdl::prolife::Workspace::CGetSoftwareCreationBarChartGqlRequest& getSoftwareCreationBarChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CBarChartData OnGetHardwareCreationBarChart(
				const sdl::prolife::Workspace::CGetHardwareCreationBarChartGqlRequest& getHardwareCreationBarChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Workspace::CLineChartData OnGetOrderCreationLineChart(
				const sdl::prolife::Workspace::CGetOrderCreationLineChartGqlRequest& getOrderCreationLineChartRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	sdl::prolife::Workspace::CBarChartData GetItemsCreationBarChart(
				const ::imtgql::CGqlRequest& gqlRequest,
				const imtbase::IObjectCollection& collection,
				const sdl::prolife::Workspace::CChartInput& chartInput,
				int nameMetaInfoType,
				QString& errorMessage) const;
	bool BuildPieChart(
				const QMap<QPair<QByteArray, QString>, int>& map,
				sdl::prolife::Workspace::CPieChartData::V1_0& pieChartData) const;
	bool BuildBarChart(const QMap<QDate, QMap<QString, int>>& map,
				const sdl::prolife::Workspace::CChartInput& input,
				const QString& yLabel,
				sdl::prolife::Workspace::CBarChartData::V1_0& barChartData) const;
	bool BuildLineChart(
				const QMap<QDate, int>& map,
				const sdl::prolife::Workspace::CChartInput& input,
				const QString& yLabel,
				sdl::prolife::Workspace::CLineChartData::V1_0& lineChartData) const;
	bool JoinGroupFilter(const imtgql::IGqlRequest& gqlRequest, iprm::CParamsSet& filterParam) const;
	uint FNV1A(const QByteArray& data) const;
	QString GenerateColorFromString(const QString& text) const;
	void AddFieldFilter(iprm::CParamsSet& paramsSet, const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter) const;
	sdl::prolife::Workspace::CChartSegment::V1_0 CreateChartSegment(int value, const QString& label, const QString& color, const QByteArray& segmentId = QByteArray()) const;
	sdl::prolife::Workspace::CBarChartData BuildProductUsageBarChart(
				const imtbase::IObjectCollection& collection,
				int productNameMetaInfoType,
				const sdl::prolife::Workspace::CChartInput& input,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const;
	sdl::prolife::Workspace::CPieChartData BuildProductUsagePieChart(
				const imtbase::IObjectCollection& collection,
				int productNameMetaInfoType,
				const sdl::prolife::Workspace::CChartInput& input,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const;
	sdl::prolife::Workspace::CPieChartData BuildProductByCustomerPieChart(
				const imtbase::IObjectCollection& collection,
				const sdl::prolife::Workspace::CChartInput& input,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const;
	void AggregateByTime(
				const imtbase::ITimeFilterParam::TimeUnit& unit,
				const imtbase::ITimeFilterParam::InterpretationMode& timeMode,
				const std::function<void(const QDate& start, const QDate& end)>& fn) const;
	void PrepareFilters(
				iprm::CParamsSet& paramsSet,
				const sdl::prolife::Workspace::CChartInput& input,
				const ::imtgql::CGqlRequest& gqlRequest,
				bool joinGroupFilter,
				std::optional<bool> internalUse,
				std::optional<bool> inUse,
				const TimeFilterType& timeFilterType) const;
	void ExtractTimeUnitFromInput(const sdl::prolife::Workspace::CChartInput& input, imtbase::ITimeFilterParam::TimeUnit& unit, imtbase::ITimeFilterParam::InterpretationMode& timeMode) const;

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


