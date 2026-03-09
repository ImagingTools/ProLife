// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ProLife includes
#include <prolifedata/ISoftwareTransfer.h>


namespace prolifedata
{


class CSoftwareTransferInfo: virtual public ISoftwareTransfer
{
public:
	CSoftwareTransferInfo();

	// reimplemented (prolifedata::ISoftwareTransfer)
	virtual QByteArray GetSoftwareId() const override;
	virtual void SetSoftwareId(const QByteArray& softwareId) override;
	virtual int GetTransferCount() const override;
	virtual void SetTransferCount(int transferCount) override;
	virtual bool IsTransferLimitExceeded() const override;
	virtual void SetTransferLimitExceeded(bool exceeded) override;
	
	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_softwareId;
	int m_transferCount;
	bool m_exceeded;
};


} // namespace prolifedata


