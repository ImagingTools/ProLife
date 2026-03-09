// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ProLife includes
#include <prolifedata/IHardwareProductBinding.h>


namespace prolifedata
{


class CHardwareProductBinding: virtual public IHardwareProductBinding
{
public:
	// reimplemented (IHardwareProductBinding)
	virtual QByteArray GetHardwareId() const override;
	virtual void SetHardwareId(const QByteArray& hardwareId) override;
	virtual QByteArrayList GetSoftwareIds() const override;
	virtual void SetSoftwareIds(QByteArrayList softwareIds) override;
	virtual bool Bind(const QByteArray& softwareId) override;
	virtual bool Unbind(const QByteArray& softwareId) override;
	virtual bool TransferAllLicenses(IHardwareProductBinding& productBinding) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_hardwareId;
	QByteArrayList m_softwareIds;
};


} // namespace prolifedata


