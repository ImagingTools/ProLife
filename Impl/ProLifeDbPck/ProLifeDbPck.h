#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <prolifedb/COrderDatabaseDelegateComp.h>
#include <prolifedb/CDeviceDatabaseDelegateComp.h>
#include <prolifedb/CAccountDatabaseDelegateComp.h>
#include <prolifedb/CDatabaseConverterComp.h>
#include <prolifedb/CSoftwareProductDatabaseDelegateComp.h>
#include <prolifedb/CProcurementOrderDatabaseDelegateComp.h>
#include <prolifedb/CIqcRunDatabaseDelegateComp.h>


/**
	ProLifeDbPck package
*/
namespace ProLifeDbPck
{


typedef prolifedb::COrderDatabaseDelegateComp OrderDatabaseDelegateComp;
typedef prolifedb::CDeviceDatabaseDelegateComp DeviceDatabaseDelegate;
typedef prolifedb::CAccountDatabaseDelegateComp AccountDatabaseDelegate;
typedef prolifedb::CDatabaseConverterComp DatabaseConverterComp;
typedef prolifedb::CSoftwareProductDatabaseDelegateComp SoftwareProductDatabaseDelegate;
typedef prolifedb::CProcurementOrderDatabaseDelegateComp ProcurementOrderDatabaseDelegate;
typedef prolifedb::CIqcRunDatabaseDelegateComp IqcRunDatabaseDelegate;


} // namespace ProLifeDbPck


