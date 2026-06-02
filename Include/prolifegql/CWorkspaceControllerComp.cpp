#include <prolifegql/CWorkspaceControllerComp.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Workspace.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtauth/CUserGroupFilter.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/IProductInstanceInfo.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/ICustomerInfo.h>


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
	{prolifedata::IDeviceInfo::DPS_DECOMMISSIONED, "#795548"}
});


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
	"#E91E63", // Pink
	
	"#673AB7", // Deep Purple
	"#CDDC39", // Lime
	"#FF5722", // Deep Orange
	"#607D8B", // Blue Grey
	"#FFEB3B", // Yellow
	"#9E9E9E", // Grey
	"#03A9F4", // Light Blue
	"#8BC34A", // Light Green
	"#FF4081", // Pink A200
	"#7C4DFF", // Deep Purple A200
	
	"#18FFFF", // Cyan A100
	"#64FFDA", // Teal A100
	"#69F0AE", // Green A100
	"#B2FF59", // Light Green A100
	"#EEFF41", // Lime A100
	"#FFFF00", // Yellow A100
	"#FFD740", // Amber A100
	"#FF9E80", // Deep Orange A100
	"#FF80AB", // Pink A100
	"#EA80FC", // Purple A100
	
	"#80DEEA", // Light Blue 100
	"#80CBC4", // Teal 100
	"#C5E1A5", // Light Green 200
	"#E6EE9C", // Lime 200
	"#FFF59D", // Yellow 200
	"#FFE082", // Amber 200
	"#EF9A9A", // Red 200
	"#CE93D8", // Purple 200
	"#9FA8DA", // Indigo 200
	"#90CAF9", // Blue 200
	
	"#26A69A", // Teal 400
	"#42A5F5", // Blue 400
	"#7E57C2", // Deep Purple 400
	"#AB47BC", // Purple 400
	"#EC407A", // Pink 400
	"#D4E157", // Lime 400
	"#FFCA28", // Amber 400
	"#FF7043", // Deep Orange 400
	"#5C6BC0", // Indigo 400
	"#66BB6A"  // Green 400
};


// protected methods

sdl::V1_0::prolife::CLineChartData CWorkspaceControllerComp::OnGetLicenseCreationInfo(
			const sdl::V1_0::prolife::CGetLicenseCreationInfoGqlRequest& getLicenseCreationInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::prolife::CLineChartData response;
	if (!m_softwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	const sdl::V1_0::prolife::GetLicenseCreationInfoRequestArguments arguments = getLicenseCreationInfoRequest.GetRequestedArguments();
	if (!arguments.input.HasValue()){
		errorMessage = QString("Unable to get license creation info. Error: GraphQL request version unsupported");
		return response;
	}

	response.axes.Emplace();
	response.labels.Emplace();
	response.points.Emplace();
	response.axes->yLabel = "Created Licenses";

	iprm::CParamsSet selectionParams;
	PrepareFilters(selectionParams, *arguments.input, gqlRequest, true, false, true, TFT_BY_LICENSE_CREATION);

	QMap<QDate, int> licenseCountByDateMap;
	imtbase::IObjectCollectionIterator* iteratorPtr = m_softwareCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, &selectionParams);
	if (iteratorPtr != nullptr){
		while (iteratorPtr->Next()){
			QDateTime timestamp = iteratorPtr->GetElementInfo("LicenseCreationDate").toDateTime();
			licenseCountByDateMap[timestamp.date()]++;
		}
	}

	if (!BuildLineChart(licenseCountByDateMap, *getLicenseCreationInfoRequest.GetRequestedArguments().input, "Created Licenses", response)){
		errorMessage = QString("Unable to build line chart data. Internal error");
		return response;
	}

	return response;
}


sdl::V1_0::prolife::CBarChartData CWorkspaceControllerComp::OnGetSoftwareUsedBarChart(
			const sdl::V1_0::prolife::CGetSoftwareUsedBarChartGqlRequest& getSoftwareUsedBarChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return BuildProductUsageBarChart(
				*m_softwareCollectionCompPtr.GetPtr(),
				imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME,
				*getSoftwareUsedBarChartRequest.GetRequestedArguments().input,
				gqlRequest,
				errorMessage);
}


sdl::V1_0::prolife::CPieChartData CWorkspaceControllerComp::OnGetSoftwareUsedPieChart(
			const sdl::V1_0::prolife::CGetSoftwareUsedPieChartGqlRequest& getSoftwareUsedPieChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return BuildProductUsagePieChart(
				*m_softwareCollectionCompPtr.GetPtr(),
				imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME,
				*getSoftwareUsedPieChartRequest.GetRequestedArguments().input,
				gqlRequest,
				errorMessage);
}


sdl::V1_0::prolife::CBarChartData CWorkspaceControllerComp::OnGetHardwareUsedBarChart(
			const sdl::V1_0::prolife::CGetHardwareUsedBarChartGqlRequest& getHardwareUsedBarChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return BuildProductUsageBarChart(
				*m_hardwareCollectionCompPtr.GetPtr(),
				prolifedata::IDeviceInfo::MIT_PRODUCT_NAME,
				*getHardwareUsedBarChartRequest.GetRequestedArguments().input,
				gqlRequest,
				errorMessage);
}


sdl::V1_0::prolife::CPieChartData CWorkspaceControllerComp::OnGetHardwareUsedPieChart(
			const sdl::V1_0::prolife::CGetHardwareUsedPieChartGqlRequest& getHardwareUsedPieChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return BuildProductUsagePieChart(
				*m_hardwareCollectionCompPtr.GetPtr(),
				prolifedata::IDeviceInfo::MIT_PRODUCT_NAME,
				*getHardwareUsedPieChartRequest.GetRequestedArguments().input,
				gqlRequest,
				errorMessage);
}


sdl::V1_0::prolife::CPieChartData CWorkspaceControllerComp::OnGetHardwareStatusInfo(
			const sdl::V1_0::prolife::CGetHardwareStatusInfoGqlRequest& getHardwareStatusInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::prolife::CPieChartData response;

	if (!m_hardwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HardwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	auto arguments = getHardwareStatusInfoRequest.GetRequestedArguments();
	if (!arguments.input.HasValue()){
		errorMessage = QString("Unable to get hardware status. Error: GraphQL version unsupported");
		return response;
	}

	iprm::CParamsSet selectionParams;
	PrepareFilters(selectionParams, *arguments.input, gqlRequest, true, false, std::nullopt, TFT_BY_CREATION);

	imtbase::ICollectionInfo::Ids elementIds = m_hardwareCollectionCompPtr->GetElementIds(0, -1, &selectionParams);
	QMap<int, int> hardwareStatutesMap;
	for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
		idoc::MetaInfoPtr metaInfoPtr = m_hardwareCollectionCompPtr->GetDataMetaInfo(elementId);
		if (metaInfoPtr.IsValid()){
			int status = metaInfoPtr->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_STATUS).toInt();
			QString statusName = prolifedata::GetNameFromDeviceProductionStatus((prolifedata::IDeviceInfo::DeviceProductionStatus) status);
			hardwareStatutesMap[status]++;
		}
	}

	response.segments.Emplace();
	for (auto it = hardwareStatutesMap.constBegin(); it != hardwareStatutesMap.constEnd(); ++it){
		response.segments->push_back(
					CreateChartSegment(
						it.value(),
						prolifedata::GetNameFromDeviceProductionStatus((prolifedata::IDeviceInfo::DeviceProductionStatus) it.key()),
						s_deviceStatusColor[it.key()]));
	}

	return response;
}


sdl::V1_0::prolife::CTotalSummaryInfo CWorkspaceControllerComp::OnGetTotalSummaryInfo(
			const sdl::V1_0::prolife::CGetTotalSummaryInfoGqlRequest& getTotalSummaryInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::prolife::CTotalSummaryInfo response;

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

	auto arguments = getTotalSummaryInfoRequest.GetRequestedArguments();
	if (!arguments.input.HasValue()){
		errorMessage = QString("Unable to get hardware status. Error: GraphQL version unsupported");
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

	response.summaryInfos.Emplace();

	bool viewLicenses = m_checkPermissionCompPtr->CheckPermission(userPermissions,{"ViewLicenses"});
	if (viewLicenses || isAdmin){
		sdl::V1_0::prolife::CCollectionSummaryInfo softwareCollectionInfo;

		iprm::CParamsSet paramsSet;
		PrepareFilters(paramsSet, *arguments.input, gqlRequest, true, std::nullopt, std::nullopt, TFT_BY_LICENSE_CREATION);

		int totalCount = m_softwareCollectionCompPtr->GetElementsCount(&paramsSet);
		softwareCollectionInfo.total = totalCount;

		iprm::CParamsSet internalUseParamsSet;
		PrepareFilters(internalUseParamsSet, *arguments.input, gqlRequest, true, true, std::nullopt, TFT_BY_LICENSE_CREATION);

		softwareCollectionInfo.internalUseCount = m_softwareCollectionCompPtr->GetElementsCount(&internalUseParamsSet);

		iprm::CParamsSet inUseParamsSet;
		PrepareFilters(inUseParamsSet, *arguments.input, gqlRequest, true, false, true, TFT_BY_LICENSE_CREATION);

		softwareCollectionInfo.inUseCount = m_softwareCollectionCompPtr->GetElementsCount(&inUseParamsSet);

		iprm::CParamsSet notInUseParamsSet;
		PrepareFilters(notInUseParamsSet, *arguments.input, gqlRequest, true, false, false, TFT_BY_LICENSE_CREATION);

		softwareCollectionInfo.notInUseCount = m_softwareCollectionCompPtr->GetElementsCount(&notInUseParamsSet);
		
		softwareCollectionInfo.collectionId = QByteArrayLiteral("SoftwareProducts");
		softwareCollectionInfo.title = QStringLiteral("Software");
		softwareCollectionInfo.icon = QStringLiteral("Icons/Key");
		softwareCollectionInfo.objectTypeId = QByteArrayLiteral("SoftwareProduct");
		response.summaryInfos->push_back(softwareCollectionInfo);
	}

	bool viewSensors = m_checkPermissionCompPtr->CheckPermission(userPermissions,{"ViewSensors"});
	if (viewSensors || isAdmin){
		sdl::V1_0::prolife::CCollectionSummaryInfo hardwareCollectionInfo;

		iprm::CParamsSet paramsSet;
		PrepareFilters(paramsSet, *arguments.input, gqlRequest, true, std::nullopt, std::nullopt, TFT_BY_LICENSE_CREATION);

		int totalCount = m_hardwareCollectionCompPtr->GetElementsCount(&paramsSet);
		hardwareCollectionInfo.total = totalCount;

		iprm::CParamsSet internalUseParamsSet;
		PrepareFilters(internalUseParamsSet, *arguments.input, gqlRequest, true, true, std::nullopt, TFT_BY_LICENSE_CREATION);

		hardwareCollectionInfo.internalUseCount = m_hardwareCollectionCompPtr->GetElementsCount(&internalUseParamsSet);

		iprm::CParamsSet inUseParamsSet;
		PrepareFilters(inUseParamsSet, *arguments.input, gqlRequest, true, false, true, TFT_BY_LICENSE_CREATION);

		hardwareCollectionInfo.inUseCount = m_hardwareCollectionCompPtr->GetElementsCount(&inUseParamsSet);

		iprm::CParamsSet notInUseParamsSet;
		PrepareFilters(notInUseParamsSet, *arguments.input, gqlRequest, true, false, false, TFT_BY_LICENSE_CREATION);

		hardwareCollectionInfo.notInUseCount = m_hardwareCollectionCompPtr->GetElementsCount(&notInUseParamsSet);

		hardwareCollectionInfo.collectionId = QByteArrayLiteral("Devices");
		hardwareCollectionInfo.title = QStringLiteral("Hardware");
		hardwareCollectionInfo.icon = QStringLiteral("Icons/Sensor");
		hardwareCollectionInfo.objectTypeId = QByteArrayLiteral("Device");
		response.summaryInfos->push_back(hardwareCollectionInfo);
	}

	bool viewOrders = m_checkPermissionCompPtr->CheckPermission(userPermissions,{"ViewOrders"});
	if (viewOrders || isAdmin){
		sdl::V1_0::prolife::CCollectionSummaryInfo orderCollectionInfo;

		iprm::CParamsSet paramsSet;
		PrepareFilters(paramsSet, *arguments.input, gqlRequest, true, std::nullopt, std::nullopt, TFT_BY_CREATION);

		orderCollectionInfo.total = m_orderCollectionCompPtr->GetElementsCount(&paramsSet);
		orderCollectionInfo.collectionId = QByteArrayLiteral("Orders");
		orderCollectionInfo.title = QStringLiteral("Orders");
		orderCollectionInfo.icon = QStringLiteral("Icons/Order");
		orderCollectionInfo.objectTypeId = QByteArrayLiteral("Order");
		response.summaryInfos->push_back(orderCollectionInfo);
	}

	return response;
}


sdl::V1_0::prolife::CPieChartData CWorkspaceControllerComp::OnGetHardwareCustomerPieChart(
			const sdl::V1_0::prolife::CGetHardwareCustomerPieChartGqlRequest& getHardwareCustomerPieChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_hardwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HardwareCollection' was not set", "CWorkspaceControllerComp");
		return sdl::V1_0::prolife::CPieChartData();
	}

	return BuildProductByCustomerPieChart(
				*m_hardwareCollectionCompPtr.GetPtr(),
				*getHardwareCustomerPieChartRequest.GetRequestedArguments().input,
				gqlRequest,
				errorMessage);
}


sdl::V1_0::prolife::CPieChartData CWorkspaceControllerComp::OnGetSoftwareCustomerPieChart(
			const sdl::V1_0::prolife::CGetSoftwareCustomerPieChartGqlRequest& getSoftwareCustomerPieChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_softwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareCollection' was not set", "CWorkspaceControllerComp");
		return sdl::V1_0::prolife::CPieChartData();
	}

	return BuildProductByCustomerPieChart(
				*m_softwareCollectionCompPtr.GetPtr(),
				*getSoftwareCustomerPieChartRequest.GetRequestedArguments().input,
				gqlRequest,
				errorMessage);
}


sdl::V1_0::prolife::CPieChartData CWorkspaceControllerComp::OnGetHardwareConfigurationPieChart(
			const sdl::V1_0::prolife::CGetHardwareConfigurationPieChartGqlRequest& getHardwareConfigurationPieChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::prolife::CPieChartData response;
	
	if (!m_hardwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HardwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	auto arguments = getHardwareConfigurationPieChartRequest.GetRequestedArguments();
	if (!arguments.input.HasValue()){
		errorMessage = QString("Unable to get hardware configuration pie chart. Error: GraphQL version unsupported");
		return response;
	}

	iprm::CParamsSet selectionParams;
	PrepareFilters(selectionParams, *arguments.input, gqlRequest, true, false, std::nullopt, TFT_BY_CREATION);

	QMap<QPair<QByteArray, QString>, int> map;
	imtbase::IObjectCollectionIterator* iteratorPtr = m_hardwareCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, &selectionParams);
	if (iteratorPtr != nullptr){
		while (iteratorPtr->Next()){
			idoc::MetaInfoPtr dataMetaInfoPtr = iteratorPtr->GetDataMetaInfo();
			if (dataMetaInfoPtr.IsValid()){
				QString licenseUuid = dataMetaInfoPtr->GetMetaInfo(prolifedata::IDeviceInfo::MIT_CONFIGURATION_TYPE).toString();
				QString licenseName = dataMetaInfoPtr->GetMetaInfo(prolifedata::IDeviceInfo::MIT_LICENSE_NAME).toString();
				map[qMakePair(licenseUuid.toUtf8(), licenseName)]++;
			}
		}
	}

	if (!BuildPieChart(map, response)){
		errorMessage = "Unable to build pie chart data. Internal error";
	}

	return response;
}



sdl::V1_0::prolife::CBarChartData CWorkspaceControllerComp::OnGetSoftwareCreationBarChart(
			const sdl::V1_0::prolife::CGetSoftwareCreationBarChartGqlRequest& getSoftwareCreationBarChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::prolife::CBarChartData response;

	if (!m_softwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	return GetItemsCreationBarChart(
				gqlRequest,
				*m_softwareCollectionCompPtr,
				*getSoftwareCreationBarChartRequest.GetRequestedArguments().input,
				imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME,
				errorMessage);
}


sdl::V1_0::prolife::CBarChartData CWorkspaceControllerComp::OnGetHardwareCreationBarChart(
			const sdl::V1_0::prolife::CGetHardwareCreationBarChartGqlRequest& getHardwareCreationBarChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::prolife::CBarChartData response;

	if (!m_hardwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HardwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	return GetItemsCreationBarChart(
				gqlRequest,
				*m_hardwareCollectionCompPtr,
				*getHardwareCreationBarChartRequest.GetRequestedArguments().input,
				prolifedata::IDeviceInfo::MIT_PRODUCT_NAME,
				errorMessage);
}


sdl::V1_0::prolife::CLineChartData CWorkspaceControllerComp::OnGetOrderCreationLineChart(
			const sdl::V1_0::prolife::CGetOrderCreationLineChartGqlRequest& request,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::prolife::CLineChartData response;

	if (!m_orderCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'OrderCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	iprm::CParamsSet selectionParams;
	const auto& input = request.GetRequestedArguments().input;
	PrepareFilters(selectionParams, *input, gqlRequest, true, std::nullopt, std::nullopt, TFT_BY_CREATION);

	QMap<QDate, int> map;

	const imtbase::ICollectionInfo::Ids elementIds = m_orderCollectionCompPtr->GetElementIds(0, -1, &selectionParams);
	for (const imtbase::ICollectionInfo::Id& id : elementIds){
		idoc::MetaInfoPtr elementMetaInfoPtr = m_orderCollectionCompPtr->GetElementMetaInfo(id);
		if (!elementMetaInfoPtr.IsValid()){
			continue;
		}

		const QDate date = elementMetaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
							.toDateTime()
							.date();

		QString dateStr = date.toString();
		map[date] += 1;
	}

	if (!BuildLineChart(map, *input, "Created Orders", response)){
		errorMessage = "Unable to build line chart data. Internal error";
	}

	return response;
}


// private methods

sdl::V1_0::prolife::CBarChartData CWorkspaceControllerComp::GetItemsCreationBarChart(
				const ::imtgql::CGqlRequest& gqlRequest,
				const imtbase::IObjectCollection& collection,
				const sdl::V1_0::prolife::CChartInput& chartInput,
				int nameMetaInfoType,
				QString& errorMessage) const
{
	sdl::V1_0::prolife::CBarChartData response;

	iprm::CParamsSet selectionParams;
	PrepareFilters(selectionParams, chartInput, gqlRequest, true, false, std::nullopt, TFT_BY_CREATION);

	QMap<QDate, QMap<QString, int>> resultMap;
	imtbase::ICollectionInfo::Ids elementIds = collection.GetElementIds(0, -1, &selectionParams);
	for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
		idoc::MetaInfoPtr dataMetaInfoPtr = collection.GetDataMetaInfo(elementId);
		idoc::MetaInfoPtr elementMetaInfoPtr = collection.GetElementMetaInfo(elementId);
		if ( dataMetaInfoPtr.IsValid() && elementMetaInfoPtr.IsValid()){
			QString productName = dataMetaInfoPtr->GetMetaInfo(nameMetaInfoType).toString();
			QDateTime insertionTime = elementMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME).toDateTime();
			resultMap[insertionTime.date()][productName]++;
		}
	}

	if (!BuildBarChart(resultMap, chartInput, "Created Instances", response)){
		errorMessage = QString("Unable to build bar chart data. Internal error");
		return response;
	}

	return response;
}


bool CWorkspaceControllerComp::BuildPieChart(
			const QMap<QPair<QByteArray, QString>, int>& map,
			sdl::V1_0::prolife::CPieChartData& pieChartData) const
{
	pieChartData.segments.Emplace();

	for (auto it = map.constBegin(); it != map.constEnd(); ++it){
		pieChartData.segments->push_back(CreateChartSegment(it.value(), it.key().second, GenerateColorFromString(it.key().second), it.key().first));
	}

	return true;
}


bool CWorkspaceControllerComp::BuildBarChart(
			const QMap<QDate, QMap<QString, int>>& map,
			const sdl::V1_0::prolife::CChartInput& input,
			const QString& yLabel,
			sdl::V1_0::prolife::CBarChartData& barChartData) const
{
	imtbase::ITimeFilterParam::TimeUnit unit = imtbase::ITimeFilterParam::TU_CUSTOM;
	imtbase::ITimeFilterParam::InterpretationMode mode = imtbase::ITimeFilterParam::IM_FOR;

	ExtractTimeUnitFromInput(input, unit, mode);

	int totalCreated = 0;
	int maxCount = 0;
	QString maxLabel;

	barChartData.bars.Emplace();
	barChartData.axes.Emplace();
	barChartData.axes->yLabel = yLabel;

	auto addBar = [&](const QDate& periodStart, const QDate& periodEnd){
		sdl::V1_0::prolife::CChartBar bar;
		bar.segments.Emplace();

		QMap<QString, int> aggregatedMap;
		for (QDate d = periodStart; d <= periodEnd; d = d.addDays(1)){
			const auto& valueMap = map.value(d);
			for (auto it = valueMap.constBegin(); it != valueMap.constEnd(); ++it){
				aggregatedMap[it.key()] += it.value();
			}
		}

		int periodTotal = 0;
		for (auto it = aggregatedMap.constBegin(); it != aggregatedMap.constEnd(); ++it){
			sdl::V1_0::prolife::CChartSegment seg;
			seg.label = it.key();
			seg.value = it.value();
			seg.color = GenerateColorFromString(it.key());
			bar.segments->push_back(seg);
			periodTotal += it.value();
		}

		bar.total = periodTotal;
		totalCreated += periodTotal;

		if (unit == imtbase::ITimeFilterParam::TU_WEEK){
			bar.label = periodStart.toString("dd MMM");
			barChartData.axes->xLabel = "Days";
		}
		else if (unit == imtbase::ITimeFilterParam::TU_MONTH){
			bar.label = (periodStart.month() == periodEnd.month())
						? QString("%1–%2 %3").arg(periodStart.toString("dd")).arg(periodEnd.toString("dd")).arg(periodEnd.toString("MMM"))
						: QString("%1–%2").arg(periodStart.toString("dd MMM")).arg(periodEnd.toString("dd MMM"));
			barChartData.axes->xLabel = "Weeks";
		}
		else if (unit == imtbase::ITimeFilterParam::TU_YEAR){
			bar.label = periodStart.toString("MMM");
			barChartData.axes->xLabel = "Months";
		}
		else{
			bar.label = QString::number(periodStart.year());
			barChartData.axes->xLabel = "Years";
		}

		if (periodTotal > maxCount){
			maxCount = periodTotal;
			maxLabel = *bar.label;
		}

		barChartData.bars->push_back(bar);
	};

	AggregateByTime(unit, mode, addBar);

	// --- Summary ---
	sdl::V1_0::prolife::CChartSummary summary;
	summary.total = totalCreated;

	sdl::V1_0::prolife::CChartSegment maxSeg;
	maxSeg.label = maxLabel;
	maxSeg.value = maxCount;
	summary.maxItem = maxSeg;

	barChartData.summary = summary;

	return true;
}


bool CWorkspaceControllerComp::BuildLineChart(
			const QMap<QDate, int>& map,
			const sdl::V1_0::prolife::CChartInput& input,
			const QString& yLabel,
			sdl::V1_0::prolife::CLineChartData& lineChartData) const
{
	imtbase::ITimeFilterParam::TimeUnit unit = imtbase::ITimeFilterParam::TU_CUSTOM;
	imtbase::ITimeFilterParam::InterpretationMode mode = imtbase::ITimeFilterParam::IM_FOR;

	ExtractTimeUnitFromInput(input, unit, mode);

	lineChartData.points.Emplace();
	lineChartData.labels.Emplace();
	lineChartData.axes.Emplace();
	lineChartData.axes->yLabel = yLabel;

	int totalCreated = 0;
	int maxCount = 0;
	QString maxLabel;
	int xIndex = 0;

	auto addPoint = [&](const QDate& periodStart, const QDate& periodEnd){
		int count = 0;
		QString label;

		if (unit == imtbase::ITimeFilterParam::TU_WEEK){
			count = map.value(periodStart, 0);
			label = periodStart.toString("dd MMM");
			lineChartData.axes->xLabel = "Days";
		}
		else if (unit == imtbase::ITimeFilterParam::TU_MONTH){
			for (QDate d = periodStart; d <= periodEnd; d = d.addDays(1)){
				count += map.value(d, 0);
			}

			label = (periodStart.month() == periodEnd.month())
					? QString("%1–%2 %3")
						.arg(periodStart.toString("dd"))
						.arg(periodEnd.toString("dd"))
						.arg(periodEnd.toString("MMM"))
					: QString("%1–%2").arg(periodStart.toString("dd MMM")).arg(periodEnd.toString("dd MMM"));
			lineChartData.axes->xLabel = "Weeks";
		}
		else if (unit == imtbase::ITimeFilterParam::TU_YEAR){
			for (QDate d = periodStart; d <= periodEnd; d = d.addDays(1)){
				count += map.value(d, 0);
			}

			label = periodStart.toString("MMM");
			lineChartData.axes->xLabel = "Months";
		}
		else{
			for (QDate d = periodStart; d <= periodEnd; d = d.addDays(1)){
				count += map.value(d, 0);
			}

			label = QString::number(periodStart.year());
			lineChartData.axes->xLabel = "Years";
		}

		sdl::V1_0::imtbase::CSdlPoint point;
		point.x = xIndex;
		point.y = count;
		lineChartData.points->push_back(point);
		lineChartData.labels->push_back(label);

		totalCreated += count;
		if (count > maxCount){
			maxCount = count;
			maxLabel = label;
		}

		++xIndex;
	};

	AggregateByTime(unit, mode, addPoint);

	// --- Summary ---
	sdl::V1_0::prolife::CChartSummary summary;
	summary.total = totalCreated;

	sdl::V1_0::prolife::CChartSegment maxSeg;
	maxSeg.value = maxCount;
	maxSeg.label = maxLabel;
	summary.maxItem = maxSeg;

	lineChartData.summary = summary;

	return true;
}


bool CWorkspaceControllerComp::JoinGroupFilter(const imtgql::IGqlRequest& gqlRequest, iprm::CParamsSet& filterParam) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
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


uint CWorkspaceControllerComp::FNV1A(const QByteArray& data) const
{
	uint hash = 2166136261u;
	for (uchar c : data){
		hash ^= c;
		hash *= 16777619u;
	}

	return hash;
}


QString CWorkspaceControllerComp::GenerateColorFromString(const QString& text) const
{
	if (text.isEmpty()){
		return QStringLiteral("#CCCCCC");
	}

	auto it = m_colorCache.find(text);
	if (it != m_colorCache.end()){
		return it.value();
	}

	uint hash = FNV1A(text.toUtf8());
	int index = hash % s_standardColors.size();
	QString color = s_standardColors[index];

	m_colorCache[text] = color;

	return color;
}


void CWorkspaceControllerComp::AddFieldFilter(iprm::CParamsSet& paramsSet, const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter) const
{
	imtbase::CComplexCollectionFilter* complexFilterPtr = dynamic_cast<imtbase::CComplexCollectionFilter*>(paramsSet.GetEditableParameter("ComplexFilter"));
	if (complexFilterPtr == nullptr){
		complexFilterPtr = new imtbase::CComplexCollectionFilter();
		paramsSet.SetEditableParameter("ComplexFilter", complexFilterPtr, true);
	}

	if (complexFilterPtr == nullptr){
		return;
	}

	complexFilterPtr->AddFieldFilter(fieldFilter);
}


sdl::V1_0::prolife::CChartSegment CWorkspaceControllerComp::CreateChartSegment(int value, const QString& label, const QString& color, const QByteArray& segmentId) const
{
	sdl::V1_0::prolife::CChartSegment segment;
	segment.id = segmentId;
	segment.label = label;
	segment.value = value;
	segment.color = color;

	return segment;
}


sdl::V1_0::prolife::CBarChartData CWorkspaceControllerComp::BuildProductUsageBarChart(
			const imtbase::IObjectCollection& collection,
			int productNameMetaInfoType,
			const sdl::V1_0::prolife::CChartInput& input,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::prolife::CBarChartData response;

	iprm::CParamsSet selectionParams;
	PrepareFilters(selectionParams, input, gqlRequest, true, false, true, TFT_BY_LICENSE_CREATION);

	QMap<QDate, QMap<QString, int>> usageMap;
	imtbase::IObjectCollectionIterator* iteratorPtr = collection.CreateObjectCollectionIterator(QByteArray(), 0, -1, &selectionParams);
	if (iteratorPtr != nullptr){
		while (iteratorPtr->Next()){
			QDateTime timestamp = iteratorPtr->GetElementInfo("LicenseCreationDate").toDateTime();
			idoc::MetaInfoPtr dataMetaInfoPtr = iteratorPtr->GetDataMetaInfo();
			if (dataMetaInfoPtr.IsValid()){
				QString name = dataMetaInfoPtr->GetMetaInfo(productNameMetaInfoType).toString();
				usageMap[timestamp.date()][name]++;
			}
		}
	}

	if (!BuildBarChart(usageMap, input, "Created Licenses", response)){
		errorMessage = "Unable to build bar chart data. Internal error";
	}

	return response;
}


sdl::V1_0::prolife::CPieChartData CWorkspaceControllerComp::BuildProductUsagePieChart(
			const imtbase::IObjectCollection& collection,
			int productNameMetaInfoType,
			const sdl::V1_0::prolife::CChartInput& input,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::prolife::CPieChartData response;

	iprm::CParamsSet paramsSet;
	PrepareFilters(paramsSet, input, gqlRequest, true, false, true, TFT_BY_LICENSE_CREATION);

	QMap<QPair<QByteArray, QString>, int> map;
	imtbase::IObjectCollectionIterator* iteratorPtr = collection.CreateObjectCollectionIterator(QByteArray(), 0, -1, &paramsSet);
	if (iteratorPtr != nullptr){
		while (iteratorPtr->Next()){
			QDateTime timestamp = iteratorPtr->GetElementInfo("LicenseCreationDate").toDateTime();
			idoc::MetaInfoPtr dataMetaInfoPtr = iteratorPtr->GetDataMetaInfo();
			if (dataMetaInfoPtr.IsValid()){
				QString name = dataMetaInfoPtr->GetMetaInfo(productNameMetaInfoType).toString();
				map[qMakePair("", name)]++;
			}
		}
	}

	if (!BuildPieChart(map, response)){
		errorMessage = "Unable to build pie chart data. Internal error";
	}

	return response;
}


sdl::V1_0::prolife::CPieChartData CWorkspaceControllerComp::BuildProductByCustomerPieChart(
			const imtbase::IObjectCollection& collection,
			const sdl::V1_0::prolife::CChartInput& input,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::prolife::CPieChartData response;

	if (!m_accountCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'AccountCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	response.segments.Emplace();

	QByteArrayList elementIds = m_accountCollectionCompPtr->GetElementIds();
	for (const QByteArray& elementId : elementIds){
		iprm::CParamsSet paramsSet;
		AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", elementId));
		PrepareFilters(paramsSet, input, gqlRequest, true, false, std::nullopt, TFT_BY_CREATION);

		int count = collection.GetElementsCount(&paramsSet);
		if (count > 0){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_accountCollectionCompPtr->GetObjectData(elementId, dataPtr)){
				const prolifedata::ICustomerInfo* accountInfoPtr = dynamic_cast<const prolifedata::ICustomerInfo*>(dataPtr.GetPtr());
				if (accountInfoPtr != nullptr){
					QString customerName = accountInfoPtr->GetName();
					response.segments->push_back(CreateChartSegment(count, customerName, GenerateColorFromString(customerName), elementId));
				}
			}
		}
	}

	return response;
}


void CWorkspaceControllerComp::AggregateByTime(
			const imtbase::ITimeFilterParam::TimeUnit& unit,
			const imtbase::ITimeFilterParam::InterpretationMode& mode,
			const std::function<void(const QDate& periodStart, const QDate& periodEnd)>& fn) const
{
	QDate currentDate = QDate::currentDate();

	if (unit == imtbase::ITimeFilterParam::TU_WEEK && mode == imtbase::ITimeFilterParam::IM_FOR){
		QDate startDate = currentDate.addDays(-6);
		QDate endDate = currentDate;
		for (QDate d = startDate; d <= endDate; d = d.addDays(1)){
			fn(d, d);
		}
	}
	else if (unit == imtbase::ITimeFilterParam::TU_MONTH){
		QDate startDate, endDate;
		if (mode == imtbase::ITimeFilterParam::IM_CURRENT){
			startDate = QDate(currentDate.year(), currentDate.month(), 1);
			endDate = startDate.addMonths(1).addDays(-1);
		}
		else if (mode == imtbase::ITimeFilterParam::IM_LAST){
			endDate = QDate(currentDate.year(), currentDate.month(), 1).addDays(-1);
			startDate = QDate(endDate.year(), endDate.month(), 1);
		}

		for (QDate iter = startDate; iter <= endDate; iter = iter.addDays(7)){
			QDate weekStart = iter;
			QDate weekEnd = std::min(weekStart.addDays(6), endDate);
			fn(weekStart, weekEnd);
		}
	}
	else if (unit == imtbase::ITimeFilterParam::TU_YEAR){
		QDate startDate, endDate;
		if (mode == imtbase::ITimeFilterParam::IM_CURRENT){
			startDate = QDate(currentDate.year(), 1, 1);
			endDate = QDate(currentDate.year(), 12, 31);
		}
		else if (mode == imtbase::ITimeFilterParam::IM_LAST){
			startDate = QDate(currentDate.year() - 1, 1, 1);
			endDate = QDate(currentDate.year() - 1, 12, 31);
		}

		for (int month = 1; month <= 12; ++month){
			QDate monthStart(startDate.year(), month, 1);
			QDate monthEnd = monthStart.addMonths(1).addDays(-1);
			fn(monthStart, monthEnd);
		}
	}
	else{
		// Default last 5 years
		int startYear = currentDate.year() - 4;
		int endYear = currentDate.year();
		for (int year = startYear; year <= endYear; ++year){
			fn(QDate(year, 1, 1), QDate(year, 12, 31));
		}
	}
}


void CWorkspaceControllerComp::PrepareFilters(
			iprm::CParamsSet& paramsSet,
			const sdl::V1_0::prolife::CChartInput& input,
			const ::imtgql::CGqlRequest& gqlRequest,
			bool joinGroupFilter,
			std::optional<bool> internalUse,
			std::optional<bool> inUse,
			const TimeFilterType& timeFilterType) const
{
	imtbase::CComplexCollectionFilter* complexFilterPtr = dynamic_cast<imtbase::CComplexCollectionFilter*>(paramsSet.GetEditableParameter("ComplexFilter"));
	if (complexFilterPtr == nullptr){
		complexFilterPtr = new imtbase::CComplexCollectionFilter();
		paramsSet.SetEditableParameter("ComplexFilter", complexFilterPtr, true);
	}

	if (complexFilterPtr == nullptr){
		return;
	}

	if (input.timeFilter.HasValue()){
		imtbase::CTimeFilterParam timeFilterParam;
		if (!m_timeFilterParamRepresentationController.GetDataModelFromSdlRepresentation(timeFilterParam, *input.timeFilter)){
			return;
		}

		if (timeFilterType == TFT_BY_CREATION){
			complexFilterPtr->SetTimeFilter(timeFilterParam);
		}
		else if (timeFilterType == TFT_BY_LICENSE_CREATION){
			istd::TDelPtr<imtbase::CTimeFilterParam> timeFilterParamPtr;
			istd::IChangeableUniquePtr clonedObjectPtr = timeFilterParam.CloneMe();
			if (clonedObjectPtr.IsValid()){
				timeFilterParamPtr.SetCastedOrRemove(clonedObjectPtr.PopInterfacePtr());
				paramsSet.SetEditableParameter("LicenseCreationTimeFilter", timeFilterParamPtr.PopPtr(), true);
			}
		}
	}

	if (input.customerId.HasValue()){
		QByteArray customerId = *input.customerId;
		if (!customerId.isEmpty()){
			AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
		}
	}

	if (internalUse.has_value()){
		AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", *internalUse));
	}

	if (inUse.has_value()){
		AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("InUse", *inUse));
	}

	if (joinGroupFilter){
		JoinGroupFilter(gqlRequest, paramsSet);
	}
}


void CWorkspaceControllerComp::ExtractTimeUnitFromInput(
			const sdl::V1_0::prolife::CChartInput& input,
			imtbase::ITimeFilterParam::TimeUnit& unit,
			imtbase::ITimeFilterParam::InterpretationMode& timeMode) const
{
	sdl::V1_0::imtbase::CTimeFilter timeFilter;
	if (!input.timeFilter.HasValue()){
		return;
	}
	timeFilter = *input.timeFilter;

	imtbase::CTimeFilterParam timeFilterParam;
	if (!m_timeFilterParamRepresentationController.GetDataModelFromSdlRepresentation(timeFilterParam, timeFilter)){
		return;
	}

	unit = timeFilterParam.GetTimeUnit();
	timeMode = timeFilterParam.GetInterpretationMode();
}


} // namespace prolifegql


