// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace prolifedata
{


class ISoftwareTransfer: virtual public iser::ISerializable
{
public:
	virtual QByteArray GetSoftwareId() const = 0;
	virtual void SetSoftwareId(const QByteArray& softwareId) = 0;
	virtual int GetTransferCount() const = 0;
	virtual void SetTransferCount(int transferCount) = 0;
	virtual bool IsTransferLimitExceeded() const = 0;
	virtual void SetTransferLimitExceeded(bool exceeded) = 0;
};


} // namespace prolifedata


