#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <prolifedb/COrderDatabaseDelegateComp.h>
#include <prolifedb/CDeviceDatabaseDelegateComp.h>
#include <prolifedb/CAccountDatabaseDelegateComp.h>
#include <prolifedb/CDatabaseConverterComp.h>


/**
	ProLifeDbPck package
*/
namespace ProLifeDbPck
{


typedef prolifedb::COrderDatabaseDelegateComp OrderDatabaseDelegateComp;
typedef prolifedb::CDeviceDatabaseDelegateComp DeviceDatabaseDelegate;
typedef prolifedb::CAccountDatabaseDelegateComp AccountDatabaseDelegate;
typedef prolifedb::CDatabaseConverterComp DatabaseConverterComp;


} // namespace ProLifeDbPck


