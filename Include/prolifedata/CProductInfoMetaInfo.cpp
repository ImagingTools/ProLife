#include <prolifedata/CProductInfoMetaInfo.h>


// ImtCore includes
#include <prolifedata/IProductInfo.h>


namespace prolifedata
{


QString CProductInfoMetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case IProductInfo::MIT_PRODUCT_NAME:
		return QObject::tr("Name");
	case IProductInfo::MIT_PRODUCT_DESCRIPTION:
		return QObject::tr("Description");
	case IProductInfo::MIT_PRODUCT_MANUFACTURER:
		return QObject::tr("Manufacturer");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace prolifedata


