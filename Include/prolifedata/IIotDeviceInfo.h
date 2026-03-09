// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>


namespace prolifedata
{


class IIotDeviceInfo: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_SERIAL_NUMBER = idoc::IDocumentMetaInfo::MIT_USER + 100,
		MIT_MANUFACTURER,
		MIT_MODEL_ID,
		MIT_MODEL_NAME
	};

	/**
		Get the serial number of this IoT device instance.
	*/
	virtual QByteArray GetSerialNumber() const = 0;

	/**
		Set the serial number of this IoT device instance.
	*/
	virtual void SetSerialNumber(const QByteArray& serialNumber) = 0;

	/**
		Get the manufacturer of this IoT device instance.
	*/
	virtual QByteArray GetManufacturer() const = 0;

	/**
		Set the manufacturer of this IoT device instance.
	*/
	virtual void SetManufacturer(const QByteArray& manufacturer) = 0;

	/**
		Get the model-ID of this IoT device instance.
	*/
	virtual QByteArray GetModelId() const = 0;

	/**
		Set the model-ID of this IoT device instance.
	*/
	virtual void SetModelId(const QByteArray& modelId) = 0;
};


} // namespace prolifedata


