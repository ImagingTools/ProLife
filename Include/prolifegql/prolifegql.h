#pragma once


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/IDocumentShareManager.h>

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

/**
	Check that a product may be put into the order \c newOrderId: putting it there changes the order,
	so an order other than \c currentOrderId must be changeable by the organization of the request.
*/
bool CheckOrderReference(
			const imtbase::IObjectCollection& orderCollection,
			const imtauth::IDocumentShareManager* shareManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& currentOrderId,
			const QByteArray& newOrderId,
			QString& errorMessage);


} // namespace prolifegql


