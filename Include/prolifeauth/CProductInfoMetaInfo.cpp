#include <prolifeauth/CProductInfoMetaInfo.h>


// ImtCore includes
#include <prolifeauth/IProductInfo.h>


namespace prolifeauth
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


} // namespace imtauth


