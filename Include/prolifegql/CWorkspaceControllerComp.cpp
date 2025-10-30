#include <prolifegql/CWorkspaceControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CTimeFilterParam.h>
#include <imtauth/IUserRecentAction.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/IProductInstanceInfo.h>


namespace prolifegql
{


// protected methods

sdl::prolife::Workspace::CLicenseCreationInfo CWorkspaceControllerComp::OnGetLicenseCreationInfo(
			const sdl::prolife::Workspace::CGetLicenseCreationInfoGqlRequest& getLicenseCreationInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CLicenseCreationInfo response;
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

	sdl::prolife::Workspace::CLicenseCreationSummary::V1_0 licenseCreationSummary;
	imtbase::ICollectionInfo::Ids elementIds = m_userActionCollectionCompPtr->GetElementIds(0, -1, &selectionParams);
	sdl::prolife::Workspace::CLicenseCreationChart::V1_0 licenseCreationChart;
	licenseCreationChart.axes.Emplace();
	licenseCreationChart.labels.Emplace();
	licenseCreationChart.points.Emplace();

	licenseCreationChart.axes->xLabel = "Days";
	licenseCreationChart.axes->yLabel = "Created Licenses";

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

	QDate startDate = QDate::currentDate();
	QDate endDate = QDate::currentDate();
	if (timeUnit == imtbase::ITimeFilterParam::TU_WEEK){
		startDate = endDate.addDays(-6);
	}
	else if (timeUnit == imtbase::ITimeFilterParam::TU_MONTH){
		startDate = endDate.addDays(-30);
	}

	int dayIndex = 1;
	int totalCreated = 0;
	int maxCount = 0;
	QDate maxDate;

	for (QDate date = startDate; date <= endDate; date = date.addDays(1), ++dayIndex) {
		int count = licenseCountByDateMap.contains(date) ? licenseCountByDateMap[date] : 0;

		sdl::imtbase::ImtBaseTypes::CSdlPoint::V1_0 sdlPoint;
		sdlPoint.x = dayIndex;
		sdlPoint.y = count;

		licenseCreationChart.points->push_back(sdlPoint);
		licenseCreationChart.labels->push_back(date.toString("dd MMM"));

		totalCreated += count;
		if (count > maxCount) {
			maxCount = count;
			maxDate = date;
		}
	}

	sdl::prolife::Workspace::CLicenseCreationPoint::V1_0 licenseCreationPoint;
	licenseCreationPoint.date = maxDate.toString("dd MMM");
	licenseCreationPoint.count = maxCount;

	licenseCreationSummary.maxDay = licenseCreationPoint;
	licenseCreationSummary.totalCreated = totalCreated;

	response.Version_1_0->chart = licenseCreationChart;
	response.Version_1_0->summary = licenseCreationSummary;

	return response;
}


sdl::prolife::Workspace::CLicenseProductStats CWorkspaceControllerComp::OnGetLicenseProductStats(
			const sdl::prolife::Workspace::CGetLicenseProductStatsGqlRequest& getLicenseProductStatsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Workspace::CLicenseProductStats response;

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

	response.Version_1_0->productStats.Emplace();

	imtbase::ICollectionInfo::Ids softwareProductIds = m_productCollectionCompPtr->GetElementIds(0, -1, &softwareSelectionParams);
	for (const imtbase::ICollectionInfo::Id& productId : softwareProductIds){
		sdl::prolife::Workspace::CLicenseProductStat::V1_0 licenseProductStat;
		licenseProductStat.productId = productId;
		licenseProductStat.productName = m_productCollectionCompPtr->GetElementInfo(productId, imtbase::ICollectionInfo::EIT_NAME).toString();

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

		licenseProductStat.totalLicenses = count;
		response.Version_1_0->productStats->append(licenseProductStat);
	}

	return response;
}


// private methods

QDateTime CWorkspaceControllerComp::GetLicenseFileCreationDate(const QByteArray& softwareId) const
{
	if (!m_userActionCollectionCompPtr.IsValid()){
		return QDateTime();
	}

	if (!m_softwareCollectionCompPtr.IsValid()){
		return QDateTime();
	}

	idoc::MetaInfoPtr metaInfoPtr = m_softwareCollectionCompPtr->GetDataMetaInfo(softwareId);
	if (!metaInfoPtr.IsValid()){
		return QDateTime();
	}

	QByteArray deviceId = metaInfoPtr->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_ID).toByteArray();

	iprm::CParamsSet paramsSet;

	imtbase::CComplexCollectionFilter complexFilter;
	imtbase::IComplexCollectionFilter::FieldFilter targetIdFieldFilter;
	targetIdFieldFilter.fieldId = "targetId";
	targetIdFieldFilter.filterValue = deviceId;
	targetIdFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
	complexFilter.AddFieldFilter(targetIdFieldFilter);

	imtbase::IComplexCollectionFilter::FieldFilter actionTypeFieldFilter;
	actionTypeFieldFilter.fieldId = "actionTypeId";
	actionTypeFieldFilter.filterValue = "CreateLicenseFile";
	actionTypeFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
	complexFilter.AddFieldFilter(actionTypeFieldFilter);

	paramsSet.SetEditableParameter("ComplexFilter", &complexFilter);
	imtbase::ICollectionInfo::Ids elementIds = m_userActionCollectionCompPtr->GetElementIds(0, -1, &paramsSet);
	if (elementIds.isEmpty()){
		return QDateTime();
	}

	return m_userActionCollectionCompPtr->GetElementInfo(elementIds[0], imtbase::ICollectionInfo::MIT_INSERTION_TIME).toDateTime();
}


} // namespace prolifegql


