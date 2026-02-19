// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace prolifedata
{


class IGroupFilterParamJoiner: virtual public istd::IPolymorphic
{
public:
	virtual bool JoinGroupFilterParam(const imtgql::IGqlRequest& gqlRequest, iprm::IParamsSet& filterParam) const = 0;
};


} // namespace prolifedata


