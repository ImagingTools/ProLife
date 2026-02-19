// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#pragma once


// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/IOrderInfo.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>

// Generated includes
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Licenses.h>

// Qt includes
#include <QString>


namespace prolifedata
{


QString GetNameFromDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus status);
QByteArray GetIdFromDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus status);
prolifedata::IDeviceInfo::DeviceProductionStatus GetProductionStatusFromId(const QByteArray& statusId);

QString GetNameFromOrderStatus(prolifedata::IOrderInfo::OrderStatus status);
QByteArray GetIdFromOrderStatus(prolifedata::IOrderInfo::OrderStatus status);
prolifedata::IOrderInfo::OrderStatus GetOrderStatusFromId(const QByteArray& statusId);

bool CheckDeviceMacAddressExists(const QByteArray& deviceUuid, const QByteArray& macAddress, const imtbase::IObjectCollection& collection);
bool CheckDeviceSerialNumberExists(const QByteArray& deviceUuid, const QByteArray& serialNumber, const imtbase::IObjectCollection& collection);

bool CheckSoftwareSerialNumberExists(const QByteArray& deviceUuid, const QByteArray& serialNumber, const imtbase::IObjectCollection& collection);

// Build complete license tree from a given license ID
// Automatically finds root and builds full hierarchy
std::optional<sdl::prolife::Licenses::CLicenseTreeNode::V1_0> BuildLicenseTree(
	const QByteArray& licenseId,
	const imtbase::IObjectCollection& softwareProductCollection,
	QString& errorMessage);


} // namespace prolifedata


