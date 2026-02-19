// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#include "ProLifeDbPck.h"


// ACF includes
#include <icomp/export.h>


namespace ProLifeDbPck
{


I_EXPORT_PACKAGE(
			"ProLifeLicenseDbPck",
			"Database-related license component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			OrderDatabaseDelegateComp,
			"Order info object for SQL table",
			"SQL Product Order Delegate");

I_EXPORT_COMPONENT(
			DeviceDatabaseDelegate,
			"Device info object for SQL table",
			"SQL Product Device Delegate");

I_EXPORT_COMPONENT(
			AccountDatabaseDelegate,
			"Account info object for SQL table",
			"SQL Account Delegate");

I_EXPORT_COMPONENT(
			DatabaseConverterComp,
			"Component used for conversion of database entries to JSON objects",
			"Database Converter");

I_EXPORT_COMPONENT(
			SoftwareProductDatabaseDelegate,
			"Software product database delegate",
			"Software Product Database Delegate");


} // namespace ProLifeLicenseDbPck


