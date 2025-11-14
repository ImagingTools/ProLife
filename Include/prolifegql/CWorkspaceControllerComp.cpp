#include <prolifegql/CWorkspaceControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CTimeFilterParam.h>
#include <imtauth/IUserRecentAction.h>
#include <imtauth/CUserGroupFilter.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/IProductInstanceInfo.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


QMap<int, QString> s_deviceStatusColor({
			{prolifedata::IDeviceInfo::DPS_NONE, "#FFC107"},
			{prolifedata::IDeviceInfo::DPS_ACCEPTED, "#4CAF50"},
			{prolifedata::IDeviceInfo::DPS_IN_PROGRESS, "#03A9F4"},
			{prolifedata::IDeviceInfo::DPS_CANCELED, "#9E9E9E"},
			{prolifedata::IDeviceInfo::DPS_ON_HOLD, "#FFC107"},
			{prolifedata::IDeviceInfo::DPS_FINISHED, "#9C27B0"},
			{prolifedata::IDeviceInfo::DPS_DEFECTED, "#F44336"},
			{prolifedata::IDeviceInfo::DPS_IN_REPAIR, "#8BC34A"},
			{prolifedata::IDeviceInfo::DPS_DECOMMISSIONED, "#795548"}});

static QStringList s_standardColors = {
			"#4CAF50", // Green
			"#2196F3", // Blue
			"#FFC107", // Amber
			"#F44336", // Red
			"#9C27B0", // Purple
			"#009688", // Teal
			"#FF9800", // Orange
			"#795548", // Brown
			"#3F51B5", // Indigo
			"#8BC34A", // Light Green
			"#00BCD4", // Cyan
			"#E91E63"  // Pink
};


// protected methods

sdl::prolife::Workspace::CLineChartData CWorkspaceControllerComp::OnGetLicenseCreationInfo(
			const sdl::prolife::Workspace::CGetLicenseCreationInfoGqlRequest& getLicenseCreationInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
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

	response.Version_1_0.Emplace();

	response.Version_1_0->axes.Emplace();
	response.Version_1_0->labels.Emplace();
	response.Version_1_0->points.Emplace();

	response.Version_1_0->axes->yLabel = "Created Licenses";

	QByteArrayList elementIds = GetUserActionsByCreateLicenseFile(gqlRequest, *arguments.input.Version_1_0);
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

	imtbase::ITimeFilterParam::TimeUnit timeUnit;
	imtbase::ITimeFilterParam::InterpretationMode mode;

	QDate startDate, endDate;
	PrepareDateFilter(*arguments.input.Version_1_0, startDate, endDate, timeUnit, mode);

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


sdl::prolife::Workspace::CBarChartData CWorkspaceControllerComp::OnGetSoftwareUsedBarChart(
			const sdl::prolife::Workspace::CGetSoftwareUsedBarChartGqlRequest& getSoftwareUsedBarChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CBarChartData response;

	if (!m_softwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	const sdl::prolife::Workspace::GetSoftwareUsedBarChartRequestArguments arguments = getSoftwareUsedBarChartRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.HasValue()){
		errorMessage = QString("Unable to get hardware used bar chart. Error: GraphQL version unsupported");
		return response;
	}

	QByteArrayList elementIds = GetUserActionsByCreateLicenseFile(gqlRequest, *arguments.input.Version_1_0);
	QMap<QDate, QMap<QString, int>> licenseTypeCountByDate;
	for (const QByteArray& id : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userActionCollectionCompPtr->GetObjectData(id, dataPtr)){
			const imtauth::IUserRecentAction* userActionPtr = dynamic_cast<const imtauth::IUserRecentAction*>(dataPtr.GetPtr());
			if (userActionPtr == nullptr){
				continue;
			}

			QDateTime timestamp = userActionPtr->GetTimestamp();
			imtauth::IUserRecentAction::TargetInfo targetInfo = userActionPtr->GetTargetInfo();
			if (targetInfo.typeId == QByteArrayLiteral("Device")){
				imtbase::IComplexCollectionFilter::FieldFilter actionTypeFieldFilter;
				actionTypeFieldFilter.fieldId = "HardwareId";
				actionTypeFieldFilter.filterValue = targetInfo.id;
				actionTypeFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;

				imtbase::CComplexCollectionFilter complexFilter;
				complexFilter.AddFieldFilter(actionTypeFieldFilter);
				AddInternalUseFieldFilter(complexFilter, false);

				iprm::CParamsSet selectionParams;
				selectionParams.SetEditableParameter("ComplexFilter", &complexFilter);

				JoinGroupFilter(gqlRequest, selectionParams);
				imtbase::ICollectionInfo::Ids softwareIds = m_softwareCollectionCompPtr->GetElementIds(0, -1, &selectionParams);
				for (const QByteArray& softwareId : softwareIds){
					QString productName = GetProductNameForSoftware(softwareId);
					licenseTypeCountByDate[timestamp.date()][productName]++;
				}
			}
		}
	}

	response.Version_1_0.Emplace();
	if (!BuildBarChart(licenseTypeCountByDate, *arguments.input.Version_1_0, "Created Licenses", *response.Version_1_0)){
		errorMessage = QString("Unable to build bar chart data. Internal error");
		return response;
	}

	return response;
}

sdl::prolife::Workspace::CPieChartData CWorkspaceControllerComp::OnGetSoftwareUsedPieChart(
			const sdl::prolife::Workspace::CGetSoftwareUsedPieChartGqlRequest& /*getSoftwareUsedPieChartRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::prolife::Workspace::CPieChartData response;
	response.Version_1_0.Emplace();
	response.Version_1_0->segments.Emplace();

	imtbase::CComplexCollectionFilter complexFilter;

	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "CategoryId";
	fieldFilter.filterValue = "Software";
	fieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
	complexFilter.AddFieldFilter(fieldFilter);

	iprm::CParamsSet selectionParams;
	selectionParams.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::ICollectionInfo::Ids elementIds = m_productCollectionCompPtr->GetElementIds(0, -1, &selectionParams);
	for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
		imtbase::IComplexCollectionFilter::FieldFilter productUuidfieldFilter;
		productUuidfieldFilter.fieldId = "ProductUuid";
		productUuidfieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
		productUuidfieldFilter.filterValue = elementId;

		imtbase::IComplexCollectionFilter::FieldFilter inUseFieldFilter;
		inUseFieldFilter.fieldId = "InUse";
		inUseFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
		inUseFieldFilter.filterValue = true;

		imtbase::CComplexCollectionFilter complexCollectionFilter;
		complexCollectionFilter.AddFieldFilter(inUseFieldFilter);
		complexCollectionFilter.AddFieldFilter(productUuidfieldFilter);
		AddInternalUseFieldFilter(complexCollectionFilter, false);

		iprm::CParamsSet paramsSet;
		paramsSet.SetEditableParameter("ComplexFilter", &complexCollectionFilter);

		JoinGroupFilter(gqlRequest, paramsSet);

		sdl::prolife::Workspace::CChartSegment::V1_0 chartSegment;
		chartSegment.value = m_softwareCollectionCompPtr->GetElementsCount(&paramsSet);
		chartSegment.label = m_productCollectionCompPtr->GetElementInfo(elementId, imtbase::ICollectionInfo::EIT_NAME).toString();
		chartSegment.color = GenerateColorFromString(*chartSegment.label);
		response.Version_1_0->segments->push_back(chartSegment);
	}

	return response;
}


sdl::prolife::Workspace::CBarChartData CWorkspaceControllerComp::OnGetHardwareUsedBarChart(
			const sdl::prolife::Workspace::CGetHardwareUsedBarChartGqlRequest& getHardwareUsedBarChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CBarChartData response;

	const sdl::prolife::Workspace::GetHardwareUsedBarChartRequestArguments arguments = getHardwareUsedBarChartRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.HasValue()){
		errorMessage = QString("Unable to get hardware used bar chart. Error: GraphQL version unsupported");
		return response;
	}

	QByteArrayList elementIds = GetUserActionsByCreateLicenseFile(gqlRequest, *arguments.input.Version_1_0);
	QMap<QDate, QMap<QString, int>> licenseTypeCountByDate;
	for (const QByteArray& id : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userActionCollectionCompPtr->GetObjectData(id, dataPtr)){
			const imtauth::IUserRecentAction* userActionPtr = dynamic_cast<const imtauth::IUserRecentAction*>(dataPtr.GetPtr());
			if (userActionPtr == nullptr){
				continue;
			}

			QDateTime timestamp = userActionPtr->GetTimestamp();
			imtauth::IUserRecentAction::TargetInfo targetInfo = userActionPtr->GetTargetInfo();
			if (targetInfo.typeId == QByteArrayLiteral("Device")){
				QString productName = GetProductNameForHardware(targetInfo.id);
				licenseTypeCountByDate[timestamp.date()][productName]++;
			}
		}
	}

	response.Version_1_0.Emplace();
	if (!BuildBarChart(licenseTypeCountByDate, *arguments.input.Version_1_0, "Created Licenses", *response.Version_1_0)){
		errorMessage = QString("Unable to build bar chart data. Internal error");
		return response;
	}

	return response;
}


sdl::prolife::Workspace::CPieChartData CWorkspaceControllerComp::OnGetHardwareUsedPieChart(
			const sdl::prolife::Workspace::CGetHardwareUsedPieChartGqlRequest& /*getHardwareUsedPieChartRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::prolife::Workspace::CPieChartData response;
	response.Version_1_0.Emplace();
	response.Version_1_0->segments.Emplace();


	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "CategoryId";
	fieldFilter.filterValue = "Hardware";
	fieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.AddFieldFilter(fieldFilter);

	iprm::CParamsSet selectionParams;
	selectionParams.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::ICollectionInfo::Ids elementIds = m_productCollectionCompPtr->GetElementIds(0, -1, &selectionParams);
	for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
		imtbase::IComplexCollectionFilter::FieldFilter deviceTypeFieldFilter;
		deviceTypeFieldFilter.fieldId = "DeviceType";
		deviceTypeFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
		deviceTypeFieldFilter.filterValue = elementId;

		imtbase::IComplexCollectionFilter::FieldFilter softwareCountFieldFilter;
		softwareCountFieldFilter.fieldId = "SoftwareCount";
		softwareCountFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_GREATER;
		softwareCountFieldFilter.filterValue = 0;

		imtbase::CComplexCollectionFilter complexCollectionFilter;
		complexCollectionFilter.AddFieldFilter(deviceTypeFieldFilter);
		complexCollectionFilter.AddFieldFilter(softwareCountFieldFilter);
		AddInternalUseFieldFilter(complexCollectionFilter, false);

		iprm::CParamsSet paramsSet;
		paramsSet.SetEditableParameter("ComplexFilter", &complexCollectionFilter);

		JoinGroupFilter(gqlRequest, paramsSet);

		sdl::prolife::Workspace::CChartSegment::V1_0 chartSegment;
		chartSegment.value = m_hardwareCollectionCompPtr->GetElementsCount(&paramsSet);
		chartSegment.label = m_productCollectionCompPtr->GetElementInfo(elementId, imtbase::ICollectionInfo::EIT_NAME).toString();
		chartSegment.color = GenerateColorFromString(*chartSegment.label);

		response.Version_1_0->segments->push_back(chartSegment);
	}

	return response;
}


sdl::prolife::Workspace::CPieChartData CWorkspaceControllerComp::OnGetHardwareStatusInfo(
			const sdl::prolife::Workspace::CGetHardwareStatusInfoGqlRequest& getHardwareStatusInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CPieChartData response;

	if (!m_hardwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HardwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	response.Version_1_0.Emplace();

	iprm::CParamsSet selectionParams;
	JoinGroupFilter(gqlRequest, selectionParams);

	imtbase::ICollectionInfo::Ids elementIds = m_hardwareCollectionCompPtr->GetElementIds(0, -1, &selectionParams);
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
		hardwareStatusItem.color = s_deviceStatusColor[it.key()];

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

	if (!m_orderCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'OrderCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	bool isAdmin = false;
	QByteArrayList userPermissions;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			isAdmin = userInfoPtr->IsAdmin();
			userPermissions = userInfoPtr->GetPermissions();
		}
	}

	response.Version_1_0.Emplace();
	response.Version_1_0->summaryInfos.Emplace();

	bool viewLicenses = m_checkPermissionCompPtr->CheckPermission(userPermissions, {"ViewLicenses"});
	if (viewLicenses || isAdmin){
		sdl::prolife::Workspace::CCollectionSummaryInfo::V1_0 softwareCollectionInfo;

		iprm::CParamsSet paramsSet;
		JoinGroupFilter(gqlRequest, paramsSet);

		int totalCount = m_softwareCollectionCompPtr->GetElementsCount(&paramsSet);
		softwareCollectionInfo.total = totalCount;

		imtbase::CComplexCollectionFilter complexCollectionFilter;
		AddInternalUseFieldFilter(complexCollectionFilter, true);
		paramsSet.SetEditableParameter("ComplexFilter", &complexCollectionFilter);

		int internalUseCount = m_softwareCollectionCompPtr->GetElementsCount(&paramsSet);
		softwareCollectionInfo.internalUseCount = internalUseCount;
		softwareCollectionInfo.inProductionCount = totalCount - internalUseCount;

		softwareCollectionInfo.collectionId = QByteArrayLiteral("SoftwareProducts");
		softwareCollectionInfo.title = QStringLiteral("Software");
		softwareCollectionInfo.icon = QStringLiteral("Icons/Key");
		softwareCollectionInfo.objectTypeId = QByteArrayLiteral("SoftwareProduct");
		response.Version_1_0->summaryInfos->push_back(softwareCollectionInfo);
	}

	bool viewSensors = m_checkPermissionCompPtr->CheckPermission(userPermissions, {"ViewSensors"});
	if (viewSensors || isAdmin){
		sdl::prolife::Workspace::CCollectionSummaryInfo::V1_0 hardwareCollectionInfo;

		iprm::CParamsSet paramsSet;
		JoinGroupFilter(gqlRequest, paramsSet);

		int totalCount = m_hardwareCollectionCompPtr->GetElementsCount(&paramsSet);
		hardwareCollectionInfo.total = totalCount;

		imtbase::CComplexCollectionFilter complexCollectionFilter;
		AddInternalUseFieldFilter(complexCollectionFilter, true);
		paramsSet.SetEditableParameter("ComplexFilter", &complexCollectionFilter);

		int internalUseCount = m_hardwareCollectionCompPtr->GetElementsCount(&paramsSet);
		hardwareCollectionInfo.internalUseCount = internalUseCount;
		hardwareCollectionInfo.inProductionCount = totalCount - internalUseCount;

		hardwareCollectionInfo.collectionId = QByteArrayLiteral("Devices");
		hardwareCollectionInfo.title = QStringLiteral("Hardware");
		hardwareCollectionInfo.icon = QStringLiteral("Icons/Sensor");
		hardwareCollectionInfo.objectTypeId = QByteArrayLiteral("Device");
		response.Version_1_0->summaryInfos->push_back(hardwareCollectionInfo);
	}

	bool viewOrders = m_checkPermissionCompPtr->CheckPermission(userPermissions, {"ViewOrders"});
	if (viewOrders || isAdmin){
		sdl::prolife::Workspace::CCollectionSummaryInfo::V1_0 orderCollectionInfo;

		iprm::CParamsSet paramsSet;
		JoinGroupFilter(gqlRequest, paramsSet);

		orderCollectionInfo.total = m_orderCollectionCompPtr->GetElementsCount(&paramsSet);
		orderCollectionInfo.collectionId = QByteArrayLiteral("Orders");
		orderCollectionInfo.title = QStringLiteral("Orders");
		orderCollectionInfo.icon = QStringLiteral("Icons/Order");
		orderCollectionInfo.objectTypeId = QByteArrayLiteral("Order");
		response.Version_1_0->summaryInfos->push_back(orderCollectionInfo);
	}

	return response;
}


// private methods

bool CWorkspaceControllerComp::PrepareDateFilter(
			const sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& timeFilterSdl,
			QDate& startDate,
			QDate& endDate,
			imtbase::ITimeFilterParam::TimeUnit& timeUnit,
			imtbase::ITimeFilterParam::InterpretationMode& timeMode) const
{
	imtbase::CTimeFilterParam timeFilter;
	if (!m_timeFilterParamRepresentationController.GetDataModelFromSdlRepresentation(timeFilter, timeFilterSdl)){
		return false;
	}

	imtbase::ITimeFilterParam::TimeUnit unit = timeFilter.GetTimeUnit();
	timeUnit = unit;

	imtbase::ITimeFilterParam::InterpretationMode mode = timeFilter.GetInterpretationMode();
	timeMode = mode;

	QDate currentDate = QDate::currentDate();
	if (unit == imtbase::ITimeFilterParam::TU_WEEK && mode == imtbase::ITimeFilterParam::IM_FOR){
		endDate = currentDate;
		startDate = currentDate.addDays(-6);
	}
	else if (unit == imtbase::ITimeFilterParam::TU_MONTH && mode == imtbase::ITimeFilterParam::IM_CURRENT){
		startDate = QDate(currentDate.year(), currentDate.month(), 1);
		endDate = startDate.addMonths(1).addDays(-1);
	}
	else if (unit == imtbase::ITimeFilterParam::TU_MONTH && mode == imtbase::ITimeFilterParam::IM_LAST){
		endDate = QDate(currentDate.year(), currentDate.month(), 1).addDays(-1);
		startDate = QDate(endDate.year(), endDate.month(), 1);
	}
	else if (unit == imtbase::ITimeFilterParam::TU_YEAR && mode == imtbase::ITimeFilterParam::IM_CURRENT){
		startDate = QDate(currentDate.year(), 1, 1);
		endDate = QDate(currentDate.year(), 12, 31);
	}
	else if (unit == imtbase::ITimeFilterParam::TU_YEAR && mode == imtbase::ITimeFilterParam::IM_LAST){
		startDate = QDate(currentDate.year() - 1, 1, 1);
		endDate = QDate(currentDate.year() - 1, 12, 31);
	}
	else{
		endDate = currentDate;
		startDate = currentDate.addDays(-6);
	}

	return true;
}


bool CWorkspaceControllerComp::BuildBarChart(
			const QMap<QDate, QMap<QString, int>>& map,
			const sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& timeFilterSdl,
			const QString& yLabel,
			sdl::prolife::Workspace::CBarChartData::V1_0& barChartData) const
{
	QDate startDate, endDate;
	imtbase::ITimeFilterParam::TimeUnit unit;
	imtbase::ITimeFilterParam::InterpretationMode mode;

	PrepareDateFilter(timeFilterSdl, startDate, endDate, unit, mode);

	int totalCreated = 0;
	int maxCount = 0;
	QString maxLabel;

	barChartData.bars.Emplace();
	barChartData.axes.Emplace();
	barChartData.axes->yLabel = yLabel;

	if (unit == imtbase::ITimeFilterParam::TU_WEEK){
		for (QDate d = startDate; d <= endDate; d = d.addDays(1)){
			sdl::prolife::Workspace::CChartBar::V1_0 bar;
			bar.label = d.toString("dd MMM");
			bar.segments.Emplace();

			const auto& typeMap = map.value(d);
			int dailyTotal = 0, colorIndex = 0;
	
			for (auto it = typeMap.constBegin(); it != typeMap.constEnd(); ++it){
				sdl::prolife::Workspace::CChartSegment::V1_0 seg;
				seg.label = it.key();
				seg.value = it.value();
				seg.color = GenerateColorFromString(it.key());
				bar.segments->push_back(seg);
				dailyTotal += it.value();
			}

			bar.total = dailyTotal;
			barChartData.bars->push_back(bar);
			barChartData.axes->xLabel = "Days";
			totalCreated += dailyTotal;
			if (dailyTotal > maxCount){ maxCount = dailyTotal; maxLabel = *bar.label; }
		}
	}
	else if (unit == imtbase::ITimeFilterParam::TU_MONTH){
		for (QDate iter = startDate; iter <= endDate; iter = iter.addDays(7)){
			QDate weekStart = iter;
			QDate weekEnd = std::min(weekStart.addDays(6), endDate);
	
			int weekTotal = 0;
			QMap<QString, int> weeklyMap;
	
			for (QDate d = weekStart; d <= weekEnd; d = d.addDays(1)){
				const auto& valueMap = map.value(d);
				for (auto it = valueMap.constBegin(); it != valueMap.constEnd(); ++it)
					weeklyMap[it.key()] += it.value();
			}
	
			sdl::prolife::Workspace::CChartBar::V1_0 bar;
			bar.segments.Emplace();
			bar.label = (weekStart.month() == weekEnd.month())
				? QString("%1–%2 %3").arg(weekStart.toString("dd")).arg(weekEnd.toString("dd")).arg(weekEnd.toString("MMM"))
				: QString("%1–%2").arg(weekStart.toString("dd MMM")).arg(weekEnd.toString("dd MMM"));
	
			barChartData.axes->xLabel = "Weeks";
			int colorIndex = 0;
			for (auto it = weeklyMap.constBegin(); it != weeklyMap.constEnd(); ++it){
				sdl::prolife::Workspace::CChartSegment::V1_0 seg;
				seg.label = it.key();
				seg.value = it.value();
				seg.color = GenerateColorFromString(it.key());
				bar.segments->push_back(seg);
				weekTotal += it.value();
			}
	
			bar.total = weekTotal;
			barChartData.bars->push_back(bar);
			totalCreated += weekTotal;
			if (weekTotal > maxCount){ maxCount = weekTotal; maxLabel = *bar.label; }
		}
	}
	else if (unit == imtbase::ITimeFilterParam::TU_YEAR){
		for (int month = 1; month <= 12; ++month){
			QDate monthStart(startDate.year(), month, 1);
			QDate monthEnd = monthStart.addMonths(1).addDays(-1);
	
			int monthTotal = 0;
			QMap<QString, int> monthMap;
	
			for (QDate d = monthStart; d <= monthEnd; d = d.addDays(1)){
				const auto& valueMap = map.value(d);
				for (auto it = valueMap.constBegin(); it != valueMap.constEnd(); ++it)
					monthMap[it.key()] += it.value();
			}
	
			sdl::prolife::Workspace::CChartBar::V1_0 bar;
			bar.segments.Emplace();
			bar.label = monthStart.toString("MMM");
			barChartData.axes->xLabel = "Month";

			int colorIndex = 0;
			for (auto it = monthMap.constBegin(); it != monthMap.constEnd(); ++it){
				sdl::prolife::Workspace::CChartSegment::V1_0 seg;
				seg.label = it.key();
				seg.value = it.value();
				seg.color = GenerateColorFromString(it.key());
				bar.segments->push_back(seg);
				monthTotal += it.value();
			}

			bar.total = monthTotal;
			barChartData.bars->push_back(bar);
			totalCreated += monthTotal;
			if (monthTotal > maxCount){ maxCount = monthTotal; maxLabel = *bar.label; }
		}
	}

	// --- Summary ---
	sdl::prolife::Workspace::CChartSummary::V1_0 summary;
	summary.total = totalCreated;

	sdl::prolife::Workspace::CChartSegment::V1_0 maxSeg;
	maxSeg.label = maxLabel;
	maxSeg.value = maxCount;
	summary.maxItem = maxSeg;

	barChartData.summary = summary;

	return true;
}


QByteArrayList CWorkspaceControllerComp::GetUserActionsByCreateLicenseFile(
			const imtgql::CGqlRequest& gqlRequest,
			const sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& timeFilterSdl) const
{
	if (!m_userActionCollectionCompPtr.IsValid()){
		return QByteArrayList();
	}

	imtbase::CTimeFilterParam timeFilterParam;
	if (!m_timeFilterParamRepresentationController.GetDataModelFromSdlRepresentation(timeFilterParam, timeFilterSdl)){
		return QByteArrayList();
	}

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetTimeFilter(timeFilterParam);

	imtbase::IComplexCollectionFilter::FieldFilter actionTypeFieldFilter;
	actionTypeFieldFilter.fieldId = "actionTypeId";
	actionTypeFieldFilter.filterValue = "CreateLicenseFile";
	actionTypeFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
	complexFilter.AddFieldFilter(actionTypeFieldFilter);

	iprm::CParamsSet selectionParams;
	selectionParams.SetEditableParameter("ComplexFilter", &complexFilter);

	JoinGroupFilter(gqlRequest, selectionParams);

	return m_userActionCollectionCompPtr->GetElementIds(0, -1, &selectionParams);
}


QString CWorkspaceControllerComp::GetProductNameForHardware(const QByteArray& hardwareId) const
{
	if (!m_hardwareCollectionCompPtr.IsValid()){
		return QString();
	}

	idoc::MetaInfoPtr metaInfoPtr = m_hardwareCollectionCompPtr->GetDataMetaInfo(hardwareId);
	if (!metaInfoPtr.IsValid()){
		return QString();
	}

	return metaInfoPtr->GetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_NAME).toString();
}


QString CWorkspaceControllerComp::GetProductNameForSoftware(const QByteArray& softwareId) const
{
	if (!m_softwareCollectionCompPtr.IsValid()){
		return QString();
	}

	idoc::MetaInfoPtr metaInfoPtr = m_softwareCollectionCompPtr->GetDataMetaInfo(softwareId);
	if (!metaInfoPtr.IsValid()){
		return QString();
	}

	return metaInfoPtr->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME).toString();
}


bool CWorkspaceControllerComp::JoinGroupFilter(const imtgql::IGqlRequest& gqlRequest, iprm::CParamsSet& filterParam) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return false;
	}

	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		return false;
	}

	QByteArray userId = userInfoPtr->GetId();
	QByteArrayList userGroupIds = userInfoPtr->GetGroups();

	if (!userInfoPtr->IsAdmin()){
		istd::TDelPtr<imtauth::CUserGroupFilter> groupFilterPtr = new imtauth::CUserGroupFilter();
		groupFilterPtr->SetUserId(userId);
		groupFilterPtr->SetGroupIds(userGroupIds);
		filterParam.SetEditableParameter("GroupFilter", groupFilterPtr.PopPtr(), true);
	}

	return true;
}


QString CWorkspaceControllerComp::GenerateColorFromString(const QString& text) const
{
	if (text.isEmpty()){
		return QString("#CCCCCC");
	}

	uint hash = qHash(text);
	int index = static_cast<int>(hash % s_standardColors.size());
	return s_standardColors[index];
}


void CWorkspaceControllerComp::AddInternalUseFieldFilter(imtbase::CComplexCollectionFilter& collectionFilter, bool internalUse) const
{
	imtbase::IComplexCollectionFilter::FieldFilter internalUseFieldFilter;
	internalUseFieldFilter.fieldId = "InternalUse";
	internalUseFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
	internalUseFieldFilter.filterValue = internalUse;

	collectionFilter.AddFieldFilter(internalUseFieldFilter);
}


} // namespace prolifegql


