#include <prolifedb/CDatabaseConverterComp.h>


// Qt includes
#include <QtCore/QDebug>

// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtlic/IProductInfo.h>
#include <imtbase/CObjectCollection.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CDeviceInfo.h>


namespace prolifedb
{


// protected methods

// reimplemented (imtdb::IMigrationController)

bool CDatabaseConverterComp::DoMigration(int& resultRevision, const istd::CIntRange& subRange) const
{
	
	
	return true;
}


} // namespace prolifedb


