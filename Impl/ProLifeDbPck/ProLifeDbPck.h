#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <prolifedb/CProductDatabaseDelegateComp.h>
#include <prolifedb/COrderDatabaseDelegateComp.h>
#include <prolifedb/CDatabaseConverterComp.h>


/**
    ProLifeDbPck package
*/
namespace ProLifeDbPck
{

typedef prolifedb::CProductDatabaseDelegateComp ProductDatabaseDelegateComp;
typedef prolifedb::COrderDatabaseDelegateComp OrderDatabaseDelegateComp;
typedef prolifedb::CDatabaseConverterComp DatabaseConverterComp;


} // namespace ProLifeDbPck


