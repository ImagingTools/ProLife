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
			ProductDatabaseDelegateComp,
			"Product info object for SQL table",
			"SQL Product Product Delegate");


} // namespace ProLifeLicenseDbPck


