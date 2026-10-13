#pragma once


// ImtCore includes
#include <imtgql/CGqlRequest.h>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Sensors_fwd.h>


namespace prolifegql
{


/**
	Check that the requesting user may apply every field change of \c deviceData to \c device.
	A new document may be filled freely by a user with the AddSensor permission and by nobody else.
*/
bool CheckDeviceChangePermissions(
			const prolifedata::IDeviceInfo& device,
			const QByteArray& orderId,
			const sdl::V1_0::prolife::CDeviceData& deviceData,
			const imtgql::CGqlRequest& gqlRequest,
			bool isNewDevice,
			QString& errorMessage);


} // namespace prolifegql


