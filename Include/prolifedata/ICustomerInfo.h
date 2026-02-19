// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#pragma once


// ImtCore includes
#include <imtauth/ICompanyInfo.h>


namespace prolifedata
{


class ICustomerInfo: virtual public imtauth::ICompanyInfo
{
public:
	enum MetaInfoTypes
	{
		MIT_CUSTOMER_ID = IContactBaseInfo::MIT_GROUPS + 1
	};
	
	/**
		Get customer-ID.
	*/
	virtual QByteArray GetCustomerId() const = 0;
};


} // namespace prolifedata


