// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace prolifedata
{


class IGroupFilter: virtual public iser::ISerializable
{
public:
	virtual QByteArray GetUserId() const = 0;
	virtual void SetUserId(const QByteArray& userId) = 0;
	virtual QByteArrayList GetGroupIds() const = 0;
	virtual void SetGroupIds(const QByteArrayList& groupIds) = 0;
};


} // namespace prolifedata


