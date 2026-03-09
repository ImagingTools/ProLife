// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ProLife includes
#include <prolifedata/CDeviceInfo.h>


namespace prolifedata
{


class CDeviceInfoComp:
		public icomp::CComponentBase,
		virtual public COrderedIdentifiableDeviceInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceInfoComp);
		I_REGISTER_INTERFACE(IDeviceInfo);
	I_END_COMPONENT;
};


} // namespace prolifedata


