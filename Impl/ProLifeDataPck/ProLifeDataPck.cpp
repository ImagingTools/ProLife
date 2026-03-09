// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#include "ProLifeDataPck.h"


// ACF includes
#include <icomp/export.h>


namespace ProLifeDataPck
{


I_EXPORT_PACKAGE(
			"ProLifeDataPck",
			"ImagingTools license management component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			OrderInfo,
			"Order information",
			"Oredr Information Authority");

I_EXPORT_COMPONENT(
			HardwareProductBindingInfo,
			"Hardware product binding info",
			"Hardware Product Binding Info");

I_EXPORT_COMPONENT(
			OrderedIdentifiableSoftwareInstanceInfo,
			"Ordered identifiable software instance info",
			"Ordered Identifiable Software Instance Info");

I_EXPORT_COMPONENT(
			DeviceInfo,
			"Device information",
			"Device Information Authority");

I_EXPORT_COMPONENT(
			IotDeviceInfo,
			"IoT device information",
			"IoT Device Information Authority");

I_EXPORT_COMPONENT(
			KeyDataProvider,
			"Key data provider",
			"License Key Provider Order Product");

I_EXPORT_COMPONENT(
			DeviceMetaInfoCreator,
			"Device meta info creator",
			"Device Meta Info Creator");

I_EXPORT_COMPONENT(
			IotDeviceMetaInfoCreator,
			"IoT device meta info creator",
			"IoT Device Meta Info Creator");

I_EXPORT_COMPONENT(
			SoftwareMetaInfoCreator,
			"Software meta info creator",
			"Software Meta Info Creator");

I_EXPORT_COMPONENT(
			OrderMetaInfoCreator,
			"Order meta info creator",
			"Order Meta Info Creator");

I_EXPORT_COMPONENT(
			CustomerMetaInfoCreator,
			"Customer meta info creator",
			"Customer Meta Info Creator");

I_EXPORT_COMPONENT(
			HardwareBindingMetaInfoCreator,
			"Hardware meta info creator",
			"Hardware Meta Info Creator");

I_EXPORT_COMPONENT(
			SoftwareTransferInfo,
			"Software transfer info",
			"Software Transfer Info");

I_EXPORT_COMPONENT(
			SplitInAction,
			"Split In Action",
			"Split In Action");

I_EXPORT_COMPONENT(
			SplitOutAction,
			"Split Out Action",
			"Split Out Action");

I_EXPORT_COMPONENT(
			RevokeInAction,
			"Revoke In Action",
			"Revoke In Action");

I_EXPORT_COMPONENT(
			RevokeOutAction,
			"Revoke Out Action",
			"Revoke Out Action");


} // namespace ProLifeDataPck


