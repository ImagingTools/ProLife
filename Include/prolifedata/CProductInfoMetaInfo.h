#pragma once


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>


namespace prolifedata
{


/**
	Meta-info creator for account information object.
*/
class CProductInfoMetaInfo: public idoc::CStandardDocumentMetaInfo
{
public:
	typedef idoc::CStandardDocumentMetaInfo BaseClass;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual QString GetMetaInfoName(int metaInfoType) const override;
};


} // namespace prolifedata


