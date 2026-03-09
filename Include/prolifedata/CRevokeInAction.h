// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace prolifedata
{


class CRevokeInAction: virtual public iser::ISerializable
{
public:
	CRevokeInAction();
	CRevokeInAction(const QByteArray& childLicenseId, int remainingCount);

	QByteArray GetChildLicenseId() const;
	void SetChildLicenseId(const QByteArray& childLicenseId);
	int GetRemainingCount() const;
	void SetRemainingCount(int remainingCount);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_childLicenseId;
	int m_remainingCount;
};


} // namespace prolifedata


