#include <prolifegql/CWorkspaceControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtauth/IUserRecentAction.h>
#include <imtauth/CUserGroupFilter.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/ILicenseDefinition.h>

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


static QStringList s_standardColors ={
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

	const sdl::prolife::Workspace::GetLicenseCreationInfoRequestArguments arguments = getLicenseCreationInfoRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.HasValue()){
		errorMessage = QString("Unable to get license creation info. Error: GraphQL request version unsupported");
		return response;
	}

	response.Version_1_0.Emplace();

	response.Version_1_0->axes.Emplace();
	response.Version_1_0->labels.Emplace();
	response.Version_1_0->points.Emplace();

	response.Version_1_0->axes->yLabel = "Created Licenses";

	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 timeFilter;
	if (arguments.input.Version_1_0->timeFilter.HasValue()){
		timeFilter = *arguments.input.Version_1_0->timeFilter;
	}

	QByteArray customerId;
	if (arguments.input.Version_1_0->customerId.HasValue()){
		customerId = *arguments.input.Version_1_0->customerId;
	}

	iprm::CParamsSet selectionParams;

	AddFieldFilter(selectionParams, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
	AddFieldFilter(selectionParams, imtbase::IComplexCollectionFilter::FieldFilter("InUse", true));
	JoinGroupFilter(gqlRequest, selectionParams);
	if (!customerId.isEmpty()){
		AddFieldFilter(selectionParams, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
	}
	AddLicenseCreationTimeFilter(selectionParams, timeFilter);

	QMap<QDate, int> licenseCountByDateMap;
	imtbase::IObjectCollectionIterator* iteratorPtr = m_softwareCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, &selectionParams);
	if (iteratorPtr != nullptr){
		while (iteratorPtr->Next()){
			QDateTime timestamp = iteratorPtr->GetElementInfo("LicenseCreationDate").toDateTime();
			licenseCountByDateMap[timestamp.date()]++;
		}
	}

	if (!BuildLineChart(licenseCountByDateMap, getLicenseCreationInfoRequest.GetRequestedArguments().input, "Created Licenses", *response.Version_1_0)){
		errorMessage = QString("Unable to build line chart data. Internal error");
		return response;
	}

	return response;
}


sdl::prolife::Workspace::CBarChartData CWorkspaceControllerComp::OnGetSoftwareUsedBarChart(
			const sdl::prolife::Workspace::CGetSoftwareUsedBarChartGqlRequest& getSoftwareUsedBarChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return BuildProductUsageBarChart(
				*m_softwareCollectionCompPtr.GetPtr(),
				imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME,
				getSoftwareUsedBarChartRequest.GetRequestedArguments().input,
				gqlRequest,
				imtbase::IComplexCollectionFilter::FieldFilter("InUse", true),
				errorMessage);
}

sdl::prolife::Workspace::CPieChartData CWorkspaceControllerComp::OnGetSoftwareUsedPieChart(
			const sdl::prolife::Workspace::CGetSoftwareUsedPieChartGqlRequest& getSoftwareUsedPieChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return BuildProductUsagePieChart(
				*m_softwareCollectionCompPtr.GetPtr(),
				imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME,
				getSoftwareUsedPieChartRequest.GetRequestedArguments().input,
				gqlRequest,
				imtbase::IComplexCollectionFilter::FieldFilter("InUse", true),
				errorMessage);
}


sdl::prolife::Workspace::CBarChartData CWorkspaceControllerComp::OnGetHardwareUsedBarChart(
			const sdl::prolife::Workspace::CGetHardwareUsedBarChartGqlRequest& getHardwareUsedBarChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return BuildProductUsageBarChart(
				*m_hardwareCollectionCompPtr.GetPtr(),
				prolifedata::IDeviceInfo::MIT_PRODUCT_NAME,
				getHardwareUsedBarChartRequest.GetRequestedArguments().input,
				gqlRequest,
				imtbase::IComplexCollectionFilter::FieldFilter("SoftwareCount", 0, imtbase::IComplexCollectionFilter::FieldOperation::FO_GREATER),
				errorMessage);
}


sdl::prolife::Workspace::CPieChartData CWorkspaceControllerComp::OnGetHardwareUsedPieChart(
			const sdl::prolife::Workspace::CGetHardwareUsedPieChartGqlRequest& getHardwareUsedPieChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return BuildProductUsagePieChart(
				*m_hardwareCollectionCompPtr.GetPtr(),
				prolifedata::IDeviceInfo::MIT_PRODUCT_NAME,
				getHardwareUsedPieChartRequest.GetRequestedArguments().input,
				gqlRequest,
				imtbase::IComplexCollectionFilter::FieldFilter("SoftwareCount", 0, imtbase::IComplexCollectionFilter::FieldOperation::FO_GREATER),
				errorMessage);
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

	auto arguments = getHardwareStatusInfoRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.HasValue()){
		errorMessage = QString("Unable to get hardware status. Error: GraphQL version unsupported");
		return response;
	}

	QByteArray customerId;
	if (arguments.input.Version_1_0->customerId.HasValue()){
		customerId = *arguments.input.Version_1_0->customerId;
	}

	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 timeFilter;
	if (arguments.input.Version_1_0->timeFilter.HasValue()){
		timeFilter = *arguments.input.Version_1_0->timeFilter;
	}

	response.Version_1_0.Emplace();

	iprm::CParamsSet selectionParams;
	if (!customerId.isEmpty()){
		AddFieldFilter(selectionParams, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
	}
	AddTimeFilter(selectionParams, timeFilter);
	AddFieldFilter(selectionParams, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
	JoinGroupFilter(gqlRequest, selectionParams);

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

	response.Version_1_0->segments.Emplace();
	for (auto it = hardwareStatutesMap.constBegin(); it != hardwareStatutesMap.constEnd(); ++it){
		response.Version_1_0->segments->push_back(
					CreateChartSegment(
						it.value(),
						prolifedata::GetNameFromDeviceProductionStatus((prolifedata::IDeviceInfo::DeviceProductionStatus) it.key()),
						s_deviceStatusColor[it.key()]));
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

	auto arguments = getTotalSummaryInfoRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.HasValue()){
		errorMessage = QString("Unable to get hardware status. Error: GraphQL version unsupported");

		return response;
	}

	QByteArray customerId;
	if (arguments.input.Version_1_0->customerId.HasValue()){
		customerId = *arguments.input.Version_1_0->customerId;
	}

	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 timeFilter;
	if (arguments.input.Version_1_0->timeFilter.HasValue()){
		timeFilter = *arguments.input.Version_1_0->timeFilter;
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

	bool viewLicenses = m_checkPermissionCompPtr->CheckPermission(userPermissions,{"ViewLicenses"});
	if (viewLicenses || isAdmin){
		sdl::prolife::Workspace::CCollectionSummaryInfo::V1_0 softwareCollectionInfo;

		iprm::CParamsSet paramsSet;
		JoinGroupFilter(gqlRequest, paramsSet);
		if (!customerId.isEmpty()){
			AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
		}
		AddTimeFilter(paramsSet, timeFilter);

		int totalCount = m_softwareCollectionCompPtr->GetElementsCount(&paramsSet);
		softwareCollectionInfo.total = totalCount;

		AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", true));

		softwareCollectionInfo.internalUseCount = m_softwareCollectionCompPtr->GetElementsCount(&paramsSet);

		iprm::CParamsSet inUseParamsSet;
		JoinGroupFilter(gqlRequest, inUseParamsSet);
		AddFieldFilter(inUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("InUse", true));
		AddFieldFilter(inUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
		if (!customerId.isEmpty()){
			AddFieldFilter(inUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
		}
		AddTimeFilter(inUseParamsSet, timeFilter);

		softwareCollectionInfo.inUseCount = m_softwareCollectionCompPtr->GetElementsCount(&inUseParamsSet);

		iprm::CParamsSet notInUseParamsSet;
		JoinGroupFilter(gqlRequest, notInUseParamsSet);
		AddFieldFilter(notInUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("InUse", false));
		AddFieldFilter(notInUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
		if (!customerId.isEmpty()){
			AddFieldFilter(notInUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
		}
		AddTimeFilter(notInUseParamsSet, timeFilter);

		softwareCollectionInfo.notInUseCount = m_softwareCollectionCompPtr->GetElementsCount(&notInUseParamsSet);

		softwareCollectionInfo.collectionId = QByteArrayLiteral("SoftwareProducts");
		softwareCollectionInfo.title = QStringLiteral("Software");
		softwareCollectionInfo.icon = QStringLiteral("Icons/Key");
		softwareCollectionInfo.objectTypeId = QByteArrayLiteral("SoftwareProduct");
		response.Version_1_0->summaryInfos->push_back(softwareCollectionInfo);
	}

	bool viewSensors = m_checkPermissionCompPtr->CheckPermission(userPermissions,{"ViewSensors"});
	if (viewSensors || isAdmin){
		sdl::prolife::Workspace::CCollectionSummaryInfo::V1_0 hardwareCollectionInfo;

		iprm::CParamsSet paramsSet;
		JoinGroupFilter(gqlRequest, paramsSet);
		if (!customerId.isEmpty()){
			AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
		}
		AddTimeFilter(paramsSet, timeFilter);

		int totalCount = m_hardwareCollectionCompPtr->GetElementsCount(&paramsSet);
		hardwareCollectionInfo.total = totalCount;

		AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", true));

		hardwareCollectionInfo.internalUseCount = m_hardwareCollectionCompPtr->GetElementsCount(&paramsSet);

		iprm::CParamsSet inUseParamsSet;
		JoinGroupFilter(gqlRequest, inUseParamsSet);
		AddFieldFilter(inUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("SoftwareCount", 0, imtbase::IComplexCollectionFilter::FO_GREATER));
		AddFieldFilter(inUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
		if (!customerId.isEmpty()){
			AddFieldFilter(inUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
		}
		AddTimeFilter(inUseParamsSet, timeFilter);

		hardwareCollectionInfo.inUseCount = m_hardwareCollectionCompPtr->GetElementsCount(&inUseParamsSet);

		iprm::CParamsSet notInUseParamsSet;
		JoinGroupFilter(gqlRequest, notInUseParamsSet);
		AddFieldFilter(notInUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("SoftwareCount", 0, imtbase::IComplexCollectionFilter::FO_EQUAL));
		AddFieldFilter(notInUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
		if (!customerId.isEmpty()){
			AddFieldFilter(notInUseParamsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
		}
		AddTimeFilter(notInUseParamsSet, timeFilter);

		hardwareCollectionInfo.notInUseCount = m_hardwareCollectionCompPtr->GetElementsCount(&notInUseParamsSet);

		hardwareCollectionInfo.collectionId = QByteArrayLiteral("Devices");
		hardwareCollectionInfo.title = QStringLiteral("Hardware");
		hardwareCollectionInfo.icon = QStringLiteral("Icons/Sensor");
		hardwareCollectionInfo.objectTypeId = QByteArrayLiteral("Device");
		response.Version_1_0->summaryInfos->push_back(hardwareCollectionInfo);
	}

	bool viewOrders = m_checkPermissionCompPtr->CheckPermission(userPermissions,{"ViewOrders"});
	if (viewOrders || isAdmin){
		sdl::prolife::Workspace::CCollectionSummaryInfo::V1_0 orderCollectionInfo;

		iprm::CParamsSet paramsSet;
		JoinGroupFilter(gqlRequest, paramsSet);
		if (!customerId.isEmpty()){
			AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
		}
		AddTimeFilter(paramsSet, timeFilter);

		orderCollectionInfo.total = m_orderCollectionCompPtr->GetElementsCount(&paramsSet);
		orderCollectionInfo.collectionId = QByteArrayLiteral("Orders");
		orderCollectionInfo.title = QStringLiteral("Orders");
		orderCollectionInfo.icon = QStringLiteral("Icons/Order");
		orderCollectionInfo.objectTypeId = QByteArrayLiteral("Order");
		response.Version_1_0->summaryInfos->push_back(orderCollectionInfo);
	}

	return response;
}


sdl::prolife::Workspace::CPieChartData CWorkspaceControllerComp::OnGetHardwareCustomerPieChart(
			const sdl::prolife::Workspace::CGetHardwareCustomerPieChartGqlRequest& getHardwareCustomerPieChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CPieChartData response;

	if (!m_accountCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'AccountCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	if (!m_hardwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HardwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	auto arguments = getHardwareCustomerPieChartRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.HasValue()){
		errorMessage = QString("Unable to get hardware customer pie chart. Error: GraphQL version unsupported");
		return response;
	}

	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 timeFilter;
	if (arguments.input.Version_1_0->timeFilter.HasValue()){
		timeFilter = *arguments.input.Version_1_0->timeFilter;
	}

	response.Version_1_0.Emplace();
	response.Version_1_0->segments.Emplace();

	QByteArrayList elementIds = m_accountCollectionCompPtr->GetElementIds();
	for (const QByteArray& elementId : elementIds){
		iprm::CParamsSet paramsSet;
		AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", elementId));
		AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
		AddTimeFilter(paramsSet, timeFilter);
		JoinGroupFilter(gqlRequest, paramsSet);

		int count = m_hardwareCollectionCompPtr->GetElementsCount(&paramsSet);
		if (count > 0){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_accountCollectionCompPtr->GetObjectData(elementId, dataPtr)){
				const prolifedata::ICustomerInfo* accountInfoPtr = dynamic_cast<const prolifedata::ICustomerInfo*>(dataPtr.GetPtr());
				if (accountInfoPtr != nullptr){
					QString customerName = accountInfoPtr->GetName();
					response.Version_1_0->segments->push_back(CreateChartSegment(count, customerName, GenerateColorFromString(customerName), elementId));
				}
			}
		}
	}

	return response;
}


sdl::prolife::Workspace::CPieChartData CWorkspaceControllerComp::OnGetSoftwareCustomerPieChart(
			const sdl::prolife::Workspace::CGetSoftwareCustomerPieChartGqlRequest& getSoftwareCustomerPieChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CPieChartData response;

	if (!m_accountCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'AccountCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	if (!m_softwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	auto arguments = getSoftwareCustomerPieChartRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.HasValue()){
		errorMessage = QString("Unable to get software customer pie chart. Error: GraphQL version unsupported");
		return response;
	}

	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 timeFilter;
	if (arguments.input.Version_1_0->timeFilter.HasValue()){
		timeFilter = *arguments.input.Version_1_0->timeFilter;
	}

	response.Version_1_0.Emplace();
	response.Version_1_0->segments.Emplace();

	QByteArrayList elementIds = m_accountCollectionCompPtr->GetElementIds();
	for (const QByteArray& elementId : elementIds){
		iprm::CParamsSet paramsSet;
		AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", elementId));
		AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
		AddTimeFilter(paramsSet, timeFilter);
		JoinGroupFilter(gqlRequest, paramsSet);

		int count = m_softwareCollectionCompPtr->GetElementsCount(&paramsSet);
		if (count > 0){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_accountCollectionCompPtr->GetObjectData(elementId, dataPtr)){
				const prolifedata::ICustomerInfo* accountInfoPtr = dynamic_cast<const prolifedata::ICustomerInfo*>(dataPtr.GetPtr());
				if (accountInfoPtr != nullptr){
					QString customerName = accountInfoPtr->GetName();
					response.Version_1_0->segments->push_back(CreateChartSegment(count, customerName, GenerateColorFromString(customerName), elementId));
				}
			}
		}
	}

	return response;
}


sdl::prolife::Workspace::CPieChartData CWorkspaceControllerComp::OnGetHardwareConfigurationPieChart(
			const sdl::prolife::Workspace::CGetHardwareConfigurationPieChartGqlRequest& getHardwareConfigurationPieChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CPieChartData response;

	if (!m_hardwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HardwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	auto arguments = getHardwareConfigurationPieChartRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.HasValue()){
		errorMessage = QString("Unable to get hardware configuration pie chart. Error: GraphQL version unsupported");
		return response;
	}

	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 timeFilter;
	if (arguments.input.Version_1_0->timeFilter.HasValue()){
		timeFilter = *arguments.input.Version_1_0->timeFilter;
	}

	iprm::CParamsSet selectionParams;

	AddFieldFilter(selectionParams, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
	AddTimeFilter(selectionParams, timeFilter);
	JoinGroupFilter(gqlRequest, selectionParams);

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

	response.Version_1_0.Emplace();

	if (!BuildPieChart(map, *response.Version_1_0)){
		errorMessage = "Unable to build pie chart data. Internal error";
	}

	return response;
}



sdl::prolife::Workspace::CBarChartData CWorkspaceControllerComp::OnGetSoftwareCreationBarChart(
			const sdl::prolife::Workspace::CGetSoftwareCreationBarChartGqlRequest& getSoftwareCreationBarChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CBarChartData response;

	if (!m_softwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	return GetItemsCreationBarChart(
				gqlRequest,
				*m_softwareCollectionCompPtr,
				getSoftwareCreationBarChartRequest.GetRequestedArguments().input,
				imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME,
				errorMessage);
}


sdl::prolife::Workspace::CBarChartData CWorkspaceControllerComp::OnGetHardwareCreationBarChart(
			const sdl::prolife::Workspace::CGetHardwareCreationBarChartGqlRequest& getHardwareCreationBarChartRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CBarChartData response;

	if (!m_hardwareCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HardwareCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	return GetItemsCreationBarChart(
				gqlRequest,
				*m_hardwareCollectionCompPtr,
				getHardwareCreationBarChartRequest.GetRequestedArguments().input,
				prolifedata::IDeviceInfo::MIT_PRODUCT_NAME,
				errorMessage);
}


sdl::prolife::Workspace::CLineChartData CWorkspaceControllerComp::OnGetOrderCreationLineChart(
			const sdl::prolife::Workspace::CGetOrderCreationLineChartGqlRequest& request,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CLineChartData response;

	if (!m_orderCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'OrderCollection' was not set", "CWorkspaceControllerComp");
		return response;
	}

	iprm::CParamsSet selectionParams;
	const auto& input = request.GetRequestedArguments().input;
	PrepareChartFilter(selectionParams, input);

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

	response.Version_1_0.Emplace();

	if (!BuildLineChart(map, input, "Created Orders", *response.Version_1_0)){
		errorMessage = "Unable to build line chart data. Internal error";
	}
	
	return response;
}



// private methods

sdl::prolife::Workspace::CBarChartData CWorkspaceControllerComp::GetItemsCreationBarChart(
			const ::imtgql::CGqlRequest& gqlRequest,
			const imtbase::IObjectCollection& collection,
			const sdl::prolife::Workspace::CChartInput& chartInput,
			int nameMetaInfoType,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CBarChartData response;

	if (!chartInput.Version_1_0.HasValue()){
		Q_ASSERT(false);
		return response;
	}

	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 timeFilter;
	if (chartInput.Version_1_0->timeFilter.HasValue()){
		timeFilter = *chartInput.Version_1_0->timeFilter;
	}

	iprm::CParamsSet selectionParams;
	AddTimeFilter(selectionParams, timeFilter);
	AddFieldFilter(selectionParams, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
	JoinGroupFilter(gqlRequest, selectionParams);

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

	response.Version_1_0.Emplace();
	if (!BuildBarChart(resultMap, timeFilter, "Created Instances", *response.Version_1_0)){
		errorMessage = QString("Unable to build bar chart data. Internal error");
		return response;
	}

	return response;
}


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

	timeUnit = timeFilter.GetTimeUnit();
	timeMode = timeFilter.GetInterpretationMode();

	QDate currentDate = QDate::currentDate();
	if (timeUnit == imtbase::ITimeFilterParam::TU_WEEK && timeMode == imtbase::ITimeFilterParam::IM_FOR){
		endDate = currentDate;
		startDate = currentDate.addDays(-6);
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_MONTH && timeMode == imtbase::ITimeFilterParam::IM_CURRENT){
		startDate = QDate(currentDate.year(), currentDate.month(), 1);
		endDate = startDate.addMonths(1).addDays(-1);
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_MONTH && timeMode == imtbase::ITimeFilterParam::IM_LAST){
		endDate = QDate(currentDate.year(), currentDate.month(), 1).addDays(-1);
		startDate = QDate(endDate.year(), endDate.month(), 1);
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_YEAR && timeMode == imtbase::ITimeFilterParam::IM_CURRENT){
		startDate = QDate(currentDate.year(), 1, 1);
		endDate = QDate(currentDate.year(), 12, 31);
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_YEAR && timeMode == imtbase::ITimeFilterParam::IM_LAST){
		startDate = QDate(currentDate.year() - 1, 1, 1);
		endDate = QDate(currentDate.year() - 1, 12, 31);
	}
	else{
		endDate = currentDate;
		startDate = currentDate.addDays(-6);
	}

	return true;
}



bool CWorkspaceControllerComp::BuildPieChart(
			const QMap<QPair<QByteArray, QString>, int>& map,
			sdl::prolife::Workspace::CPieChartData::V1_0& pieChartData) const
{
	pieChartData.segments.Emplace();

	for (auto it = map.constBegin(); it != map.constEnd(); ++it){
		pieChartData.segments->push_back(CreateChartSegment(it.value(), it.key().second, GenerateColorFromString(it.key().second), it.key().first));
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
	imtbase::ITimeFilterParam::TimeUnit unit = imtbase::ITimeFilterParam::TU_CUSTOM;
	imtbase::ITimeFilterParam::InterpretationMode mode = imtbase::ITimeFilterParam::IM_FOR;

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
			int dailyTotal = 0;

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
			if (dailyTotal > maxCount){
				maxCount = dailyTotal;
				maxLabel = *bar.label;
			}
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
			if (weekTotal > maxCount){
				maxCount = weekTotal;
				maxLabel = *bar.label;
			}
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
				for (auto it = valueMap.constBegin(); it != valueMap.constEnd(); ++it){
					monthMap[it.key()] += it.value();
				}
			}

			sdl::prolife::Workspace::CChartBar::V1_0 bar;
			bar.segments.Emplace();
			bar.label = monthStart.toString("MMM");
			barChartData.axes->xLabel = "Months";

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
			if (monthTotal > maxCount){
				maxCount = monthTotal;
				maxLabel = *bar.label;
			}
		}
	}
	else{
		QSet<int> years;
		for (auto it = map.constBegin(); it != map.constEnd(); ++it){
			years.insert(it.key().year());
		}

		QList<int> sortedYears = years.values();
		std::sort(sortedYears.begin(), sortedYears.end());
	
		barChartData.axes->xLabel = "Years";
	
		for (int year : sortedYears){
			int yearTotal = 0;
			QMap<QString, int> yearMap;

			for (QDate d = QDate(year, 1, 1); d <= QDate(year, 12, 31); d = d.addDays(1)){
				const auto& valueMap = map.value(d);
				for (auto it = valueMap.constBegin(); it != valueMap.constEnd(); ++it){
					yearMap[it.key()] += it.value();
				}
			}

			sdl::prolife::Workspace::CChartBar::V1_0 bar;
			bar.segments.Emplace();
			bar.label = QString::number(year);

			for (auto it = yearMap.constBegin(); it != yearMap.constEnd(); ++it){
				sdl::prolife::Workspace::CChartSegment::V1_0 seg;
				seg.label = it.key();
				seg.value = it.value();
				seg.color = GenerateColorFromString(it.key());
				bar.segments->push_back(seg);
				yearTotal += it.value();
			}

			bar.total = yearTotal;
			barChartData.bars->push_back(bar);

			totalCreated += yearTotal;
			if (yearTotal > maxCount){
				maxCount = yearTotal;
				maxLabel = *bar.label;
			}
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


bool CWorkspaceControllerComp::BuildLineChart(
			const QMap<QDate, int>& map,
			const sdl::prolife::Workspace::CChartInput& chartInput,
			const QString& yLabel,
			sdl::prolife::Workspace::CLineChartData::V1_0& lineChartData) const
{
	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 timeFilter;
	if (chartInput.Version_1_0->timeFilter.HasValue()){
		timeFilter = *chartInput.Version_1_0->timeFilter;
	}

	imtbase::ITimeFilterParam::TimeUnit timeUnit = imtbase::ITimeFilterParam::TU_CUSTOM;
	imtbase::ITimeFilterParam::InterpretationMode mode = imtbase::ITimeFilterParam::IM_FOR;

	QDate startDate, endDate;
	PrepareDateFilter(timeFilter, startDate, endDate, timeUnit, mode);

	int totalCreated = 0;
	int maxCount = 0;
	QString maxLabel;

	lineChartData.points.Emplace();
	lineChartData.labels.Emplace();
	lineChartData.axes.Emplace();

	lineChartData.axes->yLabel = yLabel;

	auto addPoint = [&](int x, int y, const QString& label){
		sdl::imtbase::ImtBaseTypes::CSdlPoint::V1_0 point;
		point.x = x;
		point.y = y;
		lineChartData.points->push_back(point);
		lineChartData.labels->push_back(label);
	};

	if (timeUnit == imtbase::ITimeFilterParam::TU_WEEK && mode == imtbase::ITimeFilterParam::IM_FOR){
		lineChartData.axes->xLabel = "Days";

		int dayIndex = 0;
		for (QDate d = startDate; d <= endDate; d = d.addDays(1), ++dayIndex){
			int count = map.value(d, 0);
			addPoint(dayIndex, count, d.toString("dd MMM"));
			totalCreated += count;
			if (count > maxCount){
				maxCount = count;
				maxLabel = d.toString("dd MMM");
			}
		}
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_MONTH){
		lineChartData.axes->xLabel = "Weeks";

		int weekIndex = 0;
		QDate iter = startDate;

		while (iter <= endDate){
			QDate weekStart = iter;
			QDate weekEnd = weekStart.addDays(6);
			if (weekEnd > endDate)
				weekEnd = endDate;

			int count = 0;
			for (QDate d = weekStart; d <= weekEnd; d = d.addDays(1))
				count += map.value(d, 0);

			QString rangeLabel;
			if (weekStart.month() == weekEnd.month()){
				rangeLabel = QString("%1–%2 %3")
								 .arg(weekStart.toString("dd"))
								 .arg(weekEnd.toString("dd"))
								 .arg(weekEnd.toString("MMM"));
			}
			else{
				rangeLabel = QString("%1–%2").arg(weekStart.toString("dd MMM")).arg(weekEnd.toString("dd MMM"));
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
		lineChartData.axes->xLabel = "Months";

		for (int month = 1; month <= 12; ++month){
			QDate monthStart(startDate.year(), month, 1);
			QDate monthEnd = monthStart.addMonths(1).addDays(-1);

			int count = 0;
			for (QDate d = monthStart; d <= monthEnd; d = d.addDays(1)){
				count += map.value(d, 0);
			}

			addPoint(month - 1, count, monthStart.toString("MMM"));
			totalCreated += count;
			if (count > maxCount){
				maxCount = count;
				maxLabel = monthStart.toString("MMM");
			}
		}
	}
	else{
		lineChartData.axes->xLabel = "Years";

		QSet<int> years;
		for (auto it = map.constBegin(); it != map.constEnd(); ++it){
			years.insert(it.key().year());
		}

		QList<int> sortedYears = years.values();
		std::sort(sortedYears.begin(), sortedYears.end());

		int index = 0;
		for (int year : sortedYears){
			int yearTotal = 0;
	
			for (auto it = map.constBegin(); it != map.constEnd(); ++it){
				if (it.key().year() == year)
					yearTotal += it.value();
			}
	
			addPoint(index, yearTotal, QString::number(year));
	
			totalCreated += yearTotal;
			if (yearTotal > maxCount){
				maxCount = yearTotal;
				maxLabel = QString::number(year);
			}
	
			++index;
		}
	}

	sdl::prolife::Workspace::CChartSummary::V1_0 licenseChartSummary;
	licenseChartSummary.total = totalCreated;

	sdl::prolife::Workspace::CChartSegment::V1_0 maxChartSegment;
	maxChartSegment.value = maxCount;
	maxChartSegment.label = maxLabel;
	licenseChartSummary.maxItem = maxChartSegment;

	lineChartData.summary = licenseChartSummary;

	return true;
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


uint CWorkspaceControllerComp::fnv1a(const QByteArray& data) const
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

	uint hash = fnv1a(text.toUtf8());
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


void CWorkspaceControllerComp::AddTimeFilter(
			iprm::CParamsSet& paramsSet,
			const sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& timeFilter,
			bool /*isObligatory*/) const
{
	imtbase::CComplexCollectionFilter* complexFilterPtr = dynamic_cast<imtbase::CComplexCollectionFilter*>(paramsSet.GetEditableParameter("ComplexFilter"));
	if (complexFilterPtr == nullptr){
		complexFilterPtr = new imtbase::CComplexCollectionFilter();
		paramsSet.SetEditableParameter("ComplexFilter", complexFilterPtr, true);
	}

	if (complexFilterPtr == nullptr){
		return;
	}

	imtbase::CTimeFilterParam timeFilterParam;
	if (!m_timeFilterParamRepresentationController.GetDataModelFromSdlRepresentation(timeFilterParam, timeFilter)){
		return;
	}

	complexFilterPtr->SetTimeFilter(timeFilterParam);
}


void CWorkspaceControllerComp::AddLicenseCreationTimeFilter(iprm::CParamsSet& paramsSet, const sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& timeFilter) const
{
	imtbase::CTimeFilterParam* timeFilterParamPtr = new imtbase::CTimeFilterParam();
	timeFilterParamPtr->SetTimeUnit(imtbase::ITimeFilterParam::TU_CUSTOM, imtbase::ITimeFilterParam::IM_FOR);

	if (timeFilter.timeUnit.HasValue() && timeFilter.interpretationMode.HasValue()){
		m_timeFilterParamRepresentationController.GetDataModelFromSdlRepresentation(*timeFilterParamPtr, timeFilter);
	}

	paramsSet.SetEditableParameter("LicenseCreationTimeFilter", timeFilterParamPtr, true);
}


sdl::prolife::Workspace::CChartSegment::V1_0 CWorkspaceControllerComp::CreateChartSegment(int value, const QString& label, const QString& color, const QByteArray& segmentId) const
{
	sdl::prolife::Workspace::CChartSegment::V1_0 segment;
	segment.id = segmentId;
	segment.label = label;
	segment.value = value;
	segment.color = color;

	return segment;
}


void CWorkspaceControllerComp::PrepareChartFilter(iprm::CParamsSet& paramsSet, const sdl::prolife::Workspace::CChartInput& chartInput) const
{
	if (!chartInput.Version_1_0.HasValue()){
		return;
	}

	if (chartInput.Version_1_0->timeFilter.HasValue()){
		AddTimeFilter(paramsSet, *chartInput.Version_1_0->timeFilter);
	}

	if (chartInput.Version_1_0->customerId.HasValue()){
		QByteArray customerId = *chartInput.Version_1_0->customerId;
		if (!customerId.isEmpty()){
			AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
		}
	}
}


sdl::prolife::Workspace::CBarChartData CWorkspaceControllerComp::BuildProductUsageBarChart(
			const imtbase::IObjectCollection& collection,
			int productNameMetaInfoType,
			const sdl::prolife::Workspace::CChartInput& input,
			const ::imtgql::CGqlRequest& gqlRequest,
			imtbase::IComplexCollectionFilter::FieldFilter inUseField,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CBarChartData response;
	if (!input.Version_1_0.HasValue()){
		errorMessage = "GraphQL version unsupported";
		return response;
	}

	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 timeFilter;
	if (input.Version_1_0->timeFilter.HasValue()){
		timeFilter = *input.Version_1_0->timeFilter;
	}

	QByteArray customerId;
	if (input.Version_1_0->customerId.HasValue()){
		customerId = *input.Version_1_0->customerId;
	}

	iprm::CParamsSet selectionParams;
	AddFieldFilter(selectionParams, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
	AddFieldFilter(selectionParams, inUseField);
	JoinGroupFilter(gqlRequest, selectionParams);
	if (!customerId.isEmpty()){
		AddFieldFilter(selectionParams, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
	}
	AddLicenseCreationTimeFilter(selectionParams, timeFilter);

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

	response.Version_1_0.Emplace();
	if (!BuildBarChart(usageMap, timeFilter, "Created Licenses", *response.Version_1_0)){
		errorMessage = "Unable to build bar chart data. Internal error";
	}

	return response;
}


sdl::prolife::Workspace::CPieChartData CWorkspaceControllerComp::BuildProductUsagePieChart(
			const imtbase::IObjectCollection& collection,
			int productNameMetaInfoType,
			const sdl::prolife::Workspace::CChartInput& input,
			const ::imtgql::CGqlRequest& gqlRequest,
			imtbase::IComplexCollectionFilter::FieldFilter inUseField,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CPieChartData response;

	QByteArray customerId;
	if (input.Version_1_0->customerId.HasValue()){
		customerId = *input.Version_1_0->customerId;
	}

	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 timeFilter;
	if (input.Version_1_0->timeFilter.HasValue()){
		timeFilter = *input.Version_1_0->timeFilter;
	}

	iprm::CParamsSet paramsSet;
	AddFieldFilter(paramsSet, inUseField);
	AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("InternalUse", false));
	AddLicenseCreationTimeFilter(paramsSet, timeFilter);
	if (!customerId.isEmpty()){
		AddFieldFilter(paramsSet, imtbase::IComplexCollectionFilter::FieldFilter("CustomerId", customerId));
	}
	JoinGroupFilter(gqlRequest, paramsSet);

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

	response.Version_1_0.Emplace();
	if (!BuildPieChart(map, *response.Version_1_0)){
		errorMessage = "Unable to build pie chart data. Internal error";
	}

	return response;
}


} // namespace prolifegql


