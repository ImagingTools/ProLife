// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>

// ProLife includes
#include <prolifedata/IIotDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>


namespace prolifedata
{


class CIotDeviceInfo: virtual public IIotDeviceInfo
{
public:
	CIotDeviceInfo();

	// reimplemented (IIotDeviceInfo)
	virtual QByteArray GetSerialNumber() const override;
	virtual void SetSerialNumber(const QByteArray& serialNumber) override;
	virtual QByteArray GetManufacturer() const override;
	virtual void SetManufacturer(const QByteArray& manufacturer) override;
	virtual QByteArray GetModelId() const override;
	virtual void SetModelId(const QByteArray& modelId) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_serialNumber;
	QByteArray m_manufacturer;
	QByteArray m_modelId;
};


typedef imtbase::TIdentifiableWrap<CIotDeviceInfo> CIdentifiableIotDeviceInfo;
typedef prolifedata::TOrderedWrap<CIdentifiableIotDeviceInfo> COrderedIdentifiableIotDeviceInfo;


} // namespace prolifedata


