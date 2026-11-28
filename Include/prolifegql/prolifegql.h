#pragma once


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/IDocumentShareManager.h>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/ICustomerInfo.h>
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
	Check that the requesting user may apply every field change from \c oldDevice to \c newDevice,
	for a device received as a whole document.
*/
bool CheckDeviceObjectChangePermissions(
			const prolifedata::IDeviceInfo& oldDevice,
			const prolifedata::IDeviceInfo& newDevice,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage);

/**
	Check that the requesting user may apply every field change from \c oldOrder to \c newOrder.
	\param productsChanged The request links, unlinks or creates products of the order.
	Each changed field needs its own permission below /OrderManagement/Edit/Change.
*/
bool CheckOrderChangePermissions(
			const prolifedata::IOrderInfo& oldOrder,
			const prolifedata::IOrderInfo& newOrder,
			bool productsChanged,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage);

/**
	Check that the requesting user may apply every field change from \c oldLicense to \c newLicense.
	Each changed field needs its own permission below /LicenseManagement/Edit/Change.
	The flag of internal use is not checked, like in the editor.
*/
bool CheckSoftwareChangePermissions(
			const imtlic::IProductInstanceInfo& oldLicense,
			const imtlic::IProductInstanceInfo& newLicense,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage);

/**
	Check that the requesting user may apply every field change from \c oldAccount to \c newAccount.
	Each changed field needs its own permission below /AccountManagement/Edit/Change.
*/
bool CheckAccountChangePermissions(
			const prolifedata::ICustomerInfo& oldAccount,
			const prolifedata::ICustomerInfo& newAccount,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage);

/**
	Check that the requesting user holds \c permissionId, directly or through a permission above it.
	The administrator holds every permission; an unknown user holds none.
*/
bool HoldsPermission(const imtgql::CGqlRequest& gqlRequest, const QByteArray& permissionId);

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


