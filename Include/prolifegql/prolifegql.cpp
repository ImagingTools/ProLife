#include <prolifegql/prolifegql.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Sensors.h>


// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtgql/IGqlContext.h>

// ProLife includes
#include <prolifedata/prolifedata.h>


namespace prolifegql
{


bool CheckDeviceChangePermissions(
			const prolifedata::IDeviceInfo& device,
			const QByteArray& orderId,
			const sdl::V1_0::prolife::CDeviceData& deviceData,
			const imtgql::CGqlRequest& gqlRequest,
			bool isNewDevice,
			QString& errorMessage)
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr != nullptr ? gqlContextPtr->GetUserInfo() : nullptr;
	if (userInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to update the hardware. Error: User is unknown");
		return false;
	}

	if (userInfoPtr->IsAdmin()){
		return true;
	}

	const QByteArrayList permissions = userInfoPtr->GetPermissions();
	if (isNewDevice){
		if (permissions.contains(QByteArrayLiteral("AddSensor"))){
			return true;
		}

		errorMessage = QStringLiteral("No permission to create new hardware");
		return false;
	}

	QStringList deniedFields;
	auto checkField = [&permissions, &deniedFields](bool isChanged, const QByteArray& permissionId, const QString& fieldName){
		if (isChanged && !permissions.contains(permissionId)){
			deniedFields << fieldName;
		}
	};

	const QByteArray macAddress = deviceData.macAddress ? deviceData.macAddress->toUtf8() : QByteArray();
	checkField(macAddress != device.GetMacAddress(), "ChangeMacAddress", QStringLiteral("MAC-Address"));

	const QByteArray serialNumber = deviceData.serialNumber ? deviceData.serialNumber->toUtf8() : QByteArray();
	checkField(serialNumber != device.GetSerialNumber(), "ChangeSerialNumberForSensor", QStringLiteral("Serial Number"));

	const QByteArray configurationType = deviceData.licenseName ? deviceData.licenseName->toUtf8() : QByteArray();
	checkField(configurationType != device.GetConfigurationType(), "ChangeHardwareConfiguration", QStringLiteral("Hardware Configuration"));

	const QByteArray deviceType = deviceData.deviceType ? deviceData.deviceType->toUtf8() : QByteArray();
	checkField(deviceType != device.GetDeviceType(), "ChangeDeviceType", QStringLiteral("Device Type"));

	if (deviceData.project){
		checkField(deviceData.project->toUtf8() != device.GetProject(), "ChangeProjectForSensor", QStringLiteral("Project"));
	}

	if (deviceData.orderId){
		checkField(deviceData.orderId->toUtf8() != orderId, "ChangeOrderForSensor", QStringLiteral("Order"));
	}

	if (deviceData.description){
		checkField(*deviceData.description != device.GetDescription(), "ChangeDescriptionForSensor", QStringLiteral("Description"));
	}

	if (deviceData.productionStatus){
		const prolifedata::IDeviceInfo::DeviceProductionStatus status = prolifedata::GetProductionStatusFromId(deviceData.productionStatus->toUtf8());
		checkField(status != device.GetDeviceProductionStatus(), "ChangeProductionStatus", QStringLiteral("Production Status"));
	}

	if (!deniedFields.isEmpty()){
		errorMessage = QStringLiteral("No permission to change: %1").arg(deniedFields.join(QStringLiteral(", ")));
		return false;
	}

	return true;
}


} // namespace prolifegql


