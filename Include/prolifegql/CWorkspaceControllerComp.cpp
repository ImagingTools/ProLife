#include <prolifegql/CWorkspaceControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CTimeFilterParam.h>
#include <imtauth/IUserRecentAction.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/IProductInstanceInfo.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


// protected methods

sdl::prolife::Workspace::CLineChartData CWorkspaceControllerComp::OnGetLicenseCreationInfo(
			const sdl::prolife::Workspace::CGetLicenseCreationInfoGqlRequest& getLicenseCreationInfoRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CLineChartData response;
	if (!m_softwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	if (!m_userActionCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserActionCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	const  sdl::prolife::Workspace::GetLicenseCreationInfoRequestArguments arguments = getLicenseCreationInfoRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.HasValue()){
		errorMessage = QString("Unable to get license creation info. Error: GraphQL request version unsupported");
		return response;
	}

	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 sdlTimeFilter = *arguments.input.Version_1_0;
	imtbase::CTimeFilterParam timeFilterParam;
	if (!m_timeFilterParamRepresentationController.GetDataModelFromSdlRepresentation(timeFilterParam, sdlTimeFilter)){
		return response;
	}

	imtbase::ITimeFilterParam::TimeUnit timeUnit = timeFilterParam.GetTimeUnit();
	imtbase::ITimeFilterParam::InterpretationMode mode = timeFilterParam.GetInterpretationMode();

	response.Version_1_0.Emplace();
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetTimeFilter(timeFilterParam);

	imtbase::IComplexCollectionFilter::FieldFilter actionTypeFieldFilter;
	actionTypeFieldFilter.fieldId = "actionTypeId";
	actionTypeFieldFilter.filterValue = "CreateLicenseFile";
	actionTypeFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
	complexFilter.AddFieldFilter(actionTypeFieldFilter);

	iprm::CParamsSet selectionParams;
	selectionParams.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::ICollectionInfo::Ids elementIds = m_userActionCollectionCompPtr->GetElementIds(0, -1, &selectionParams);
	response.Version_1_0->axes.Emplace();
	response.Version_1_0->labels.Emplace();
	response.Version_1_0->points.Emplace();

	response.Version_1_0->axes->yLabel = "Created Licenses";

	QMap<QDate, int> licenseCountByDateMap;
	for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userActionCollectionCompPtr->GetObjectData(elementId, dataPtr)){
			const imtauth::IUserRecentAction* userActionPtr = dynamic_cast<imtauth::IUserRecentAction*>(dataPtr.GetPtr());
			if (userActionPtr != nullptr){
				QDateTime timestamp = userActionPtr->GetTimestamp();
				if (licenseCountByDateMap.contains(timestamp.date())){
					licenseCountByDateMap[timestamp.date()]++;
				}
				else{
					licenseCountByDateMap[timestamp.date()] = 1;
				}
			}
		}
	}

	QDate currentDate = QDate::currentDate();
	QDate startDate, endDate;
	if (timeUnit == imtbase::ITimeFilterParam::TU_WEEK && mode == imtbase::ITimeFilterParam::IM_FOR){
		endDate = currentDate;
		startDate = currentDate.addDays(-6);
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_MONTH && mode == imtbase::ITimeFilterParam::IM_CURRENT){
		startDate = QDate(currentDate.year(), currentDate.month(), 1);
		endDate = startDate.addMonths(1).addDays(-1);
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_MONTH && mode == imtbase::ITimeFilterParam::IM_LAST){
		endDate = QDate(currentDate.year(), currentDate.month(), 1).addDays(-1);
		startDate = QDate(endDate.year(), endDate.month(), 1);
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_YEAR && mode == imtbase::ITimeFilterParam::IM_CURRENT){
		startDate = QDate(currentDate.year(), 1, 1);
		endDate = QDate(currentDate.year(), 12, 31);
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_YEAR && mode == imtbase::ITimeFilterParam::IM_LAST){
		startDate = QDate(currentDate.year() - 1, 1, 1);
		endDate = QDate(currentDate.year() - 1, 12, 31);
	}
	else{
		endDate = currentDate;
		startDate = currentDate.addDays(-6);
	}

	int totalCreated = 0;
	int maxCount = 0;
	QString maxLabel;

	auto addPoint = [&](int x, int y, const QString& label){
		sdl::imtbase::ImtBaseTypes::CSdlPoint::V1_0 point;
		point.x = x;
		point.y = y;
		response.Version_1_0->points->push_back(point);
		response.Version_1_0->labels->push_back(label);
	};

	if (timeUnit == imtbase::ITimeFilterParam::TU_WEEK && mode == imtbase::ITimeFilterParam::IM_FOR){
		response.Version_1_0->axes->xLabel = "Days";

		int dayIndex = 0;
		for (QDate d = startDate; d <= endDate; d = d.addDays(1), ++dayIndex){
			int count = licenseCountByDateMap.value(d, 0);
			addPoint(dayIndex, count, d.toString("dd MMM"));
			totalCreated += count;
			if (count > maxCount){
				maxCount = count;
				maxLabel = d.toString("dd MMM");
			}
		}
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_MONTH){
		response.Version_1_0->axes->xLabel = "Weeks";

		int weekIndex = 0;
		QDate iter = startDate;

		while (iter <= endDate){
			QDate weekStart = iter;
			QDate weekEnd = weekStart.addDays(6);
			if (weekEnd > endDate)
				weekEnd = endDate;

			int count = 0;
			for (QDate d = weekStart; d <= weekEnd; d = d.addDays(1))
				count += licenseCountByDateMap.value(d, 0);

			QString rangeLabel;
			if (weekStart.month() == weekEnd.month()){
				rangeLabel = QString("%1–%2 %3")
							.arg(weekStart.toString("dd"))
							.arg(weekEnd.toString("dd"))
							.arg(weekEnd.toString("MMM"));
			}
			else{
				rangeLabel = QString("%1–%2")
							.arg(weekStart.toString("dd MMM"))
							.arg(weekEnd.toString("dd MMM"));
			}

			addPoint(weekIndex, count, rangeLabel);

			totalCreated += count;
			if (count > maxCount){
				maxCount = count;
				maxLabel = weekStart.toString("dd MMM");
			}

			iter = weekEnd.addDays(1);
			++weekIndex;
		}
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_YEAR){
		response.Version_1_0->axes->xLabel = "Months";

		for (int month = 1; month <= 12; ++month){
			QDate monthStart(startDate.year(), month, 1);
			QDate monthEnd = monthStart.addMonths(1).addDays(-1);
	
			int count = 0;
			for (QDate d = monthStart; d <= monthEnd; d = d.addDays(1))
				count += licenseCountByDateMap.value(d, 0);
	
			addPoint(month - 1, count, monthStart.toString("MMM"));
			totalCreated += count;
			if (count > maxCount){
				maxCount = count;
				maxLabel = monthStart.toString("MMM");
			}
		}
	}

	sdl::prolife::Workspace::CChartSummary::V1_0 licenseChartSummary;
	licenseChartSummary.total = totalCreated;

	sdl::prolife::Workspace::CChartSegment::V1_0 maxChartSegment;
	maxChartSegment.value = maxCount;
	maxChartSegment.label = maxLabel;
	licenseChartSummary.maxItem = maxChartSegment;

	response.Version_1_0->summary = licenseChartSummary;

	return response;
}


sdl::prolife::Workspace::CPieChartData CWorkspaceControllerComp::OnGetLicenseProductStats(
			const sdl::prolife::Workspace::CGetLicenseProductStatsGqlRequest& getLicenseProductStatsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CPieChartData response;

	if (!m_softwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	if (!m_hardwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HardwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	if (!m_productCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ProductCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	const sdl::prolife::Workspace::GetLicenseProductStatsRequestArguments arguments = getLicenseProductStatsRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.HasValue()){
		errorMessage = QString("Unable to get license product stats. Error: GraphQL version unsupported");
		return response;
	}

	sdl::prolife::Workspace::Product productType = sdl::prolife::Workspace::Product::Software;
	if (arguments.input.Version_1_0->productInfo){
		productType = *arguments.input.Version_1_0->productInfo;
	}

	response.Version_1_0.Emplace();

	iprm::CParamsSet softwareSelectionParams;
	imtbase::CComplexCollectionFilter complexFilter;

	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "CategoryId";
	fieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;

	if (productType == sdl::prolife::Workspace::Product::Software){
		fieldFilter.filterValue = "Software";
	}
	else if (productType == sdl::prolife::Workspace::Product::Hardware){
		fieldFilter.filterValue = "Hardware";
	}

	complexFilter.AddFieldFilter(fieldFilter);

	softwareSelectionParams.SetEditableParameter("ComplexFilter", &complexFilter);

	response.Version_1_0->segments.Emplace();

	imtbase::ICollectionInfo::Ids softwareProductIds = m_productCollectionCompPtr->GetElementIds(0, -1, &softwareSelectionParams);
	for (const imtbase::ICollectionInfo::Id& productId : softwareProductIds){
		sdl::prolife::Workspace::CChartSegment::V1_0 licenseProductStat;
		licenseProductStat.label = m_productCollectionCompPtr->GetElementInfo(productId, imtbase::ICollectionInfo::EIT_NAME).toString();

		int count = 0;
		iprm::CParamsSet paramsSet;
		imtbase::CComplexCollectionFilter complexCollectionFilter;
		if (productType == sdl::prolife::Workspace::Product::Software){
			imtbase::IComplexCollectionFilter::FieldFilter productUuidfieldFilter;
			productUuidfieldFilter.fieldId = "ProductUuid";
			productUuidfieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
			productUuidfieldFilter.filterValue = productId;

			imtbase::IComplexCollectionFilter::FieldFilter inUseFieldFilter;
			inUseFieldFilter.fieldId = "InUse";
			inUseFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
			inUseFieldFilter.filterValue = true;

			complexCollectionFilter.AddFieldFilter(inUseFieldFilter);
			complexCollectionFilter.AddFieldFilter(productUuidfieldFilter);

			paramsSet.SetEditableParameter("ComplexFilter", &complexCollectionFilter);

			count = m_softwareCollectionCompPtr->GetElementsCount(&paramsSet);
		}
		else{
			imtbase::IComplexCollectionFilter::FieldFilter deviceTypeFieldFilter;
			deviceTypeFieldFilter.fieldId = "DeviceType";
			deviceTypeFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
			deviceTypeFieldFilter.filterValue = productId;

			imtbase::IComplexCollectionFilter::FieldFilter softwareCountFieldFilter;
			softwareCountFieldFilter.fieldId = "SoftwareCount";
			softwareCountFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_GREATER;
			softwareCountFieldFilter.filterValue = 0;

			complexCollectionFilter.AddFieldFilter(deviceTypeFieldFilter);
			complexCollectionFilter.AddFieldFilter(softwareCountFieldFilter);

			paramsSet.SetEditableParameter("ComplexFilter", &complexCollectionFilter);

			count = m_hardwareCollectionCompPtr->GetElementsCount(&paramsSet);
		}

		licenseProductStat.value = count;
		response.Version_1_0->segments->append(licenseProductStat);
	}

	return response;
}


sdl::prolife::Workspace::CPieChartData CWorkspaceControllerComp::OnGetHardwareStatusInfo(
			const sdl::prolife::Workspace::CGetHardwareStatusInfoGqlRequest& getHardwareStatusInfoRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CPieChartData response;

	if (!m_hardwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HardwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	response.Version_1_0.Emplace();

	imtbase::ICollectionInfo::Ids elementIds = m_hardwareCollectionCompPtr->GetElementIds();
	QMap<int, int> hardwareStatutesMap;
	for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
		idoc::MetaInfoPtr metaInfoPtr = m_hardwareCollectionCompPtr->GetDataMetaInfo(elementId);
		if (metaInfoPtr.IsValid()){
			int status = metaInfoPtr->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_STATUS).toInt();
			if (hardwareStatutesMap.contains(status)){
				hardwareStatutesMap[status]++;
			}
			else{
				hardwareStatutesMap[status] = 1;
			}
		}
	}

	response.Version_1_0->segments.Emplace();
	for (auto it = hardwareStatutesMap.constBegin(); it != hardwareStatutesMap.constEnd(); ++it){
		sdl::prolife::Workspace::CChartSegment::V1_0 hardwareStatusItem;
		hardwareStatusItem.label = prolifedata::GetNameFromDeviceProductionStatus((prolifedata::IDeviceInfo::DeviceProductionStatus)it.key());
		hardwareStatusItem.value = it.value();

		response.Version_1_0->segments->push_back(hardwareStatusItem);
	}

	return response;
}


sdl::prolife::Workspace::CTotalSummaryInfo CWorkspaceControllerComp::OnGetTotalSummaryInfo(
			const sdl::prolife::Workspace::CGetTotalSummaryInfoGqlRequest& getTotalSummaryInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CTotalSummaryInfo response;

	if (!m_hardwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HardwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	if (!m_softwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	response.Version_1_0.Emplace();
	response.Version_1_0->summaryInfos.Emplace();

	sdl::prolife::Workspace::CCollectionSummaryInfo::V1_0 softwareCollectionInfo;
	softwareCollectionInfo.total = m_softwareCollectionCompPtr->GetElementsCount();
	softwareCollectionInfo.collectionId = QByteArrayLiteral("SoftwareProducts");
	softwareCollectionInfo.title = QStringLiteral("Software");
	softwareCollectionInfo.icon = QStringLiteral("Icons/Key");
	softwareCollectionInfo.objectTypeId = QByteArrayLiteral("SoftwareProduct");
	response.Version_1_0->summaryInfos->push_back(softwareCollectionInfo);

	sdl::prolife::Workspace::CCollectionSummaryInfo::V1_0 hardwareCollectionInfo;
	hardwareCollectionInfo.total = m_hardwareCollectionCompPtr->GetElementsCount();
	hardwareCollectionInfo.collectionId = QByteArrayLiteral("Devices");
	hardwareCollectionInfo.title = QStringLiteral("Hardware");
	hardwareCollectionInfo.icon = QStringLiteral("Icons/Sensor");
	hardwareCollectionInfo.objectTypeId = QByteArrayLiteral("Device");
	response.Version_1_0->summaryInfos->push_back(hardwareCollectionInfo);

	return response;
}


} // namespace prolifegql


