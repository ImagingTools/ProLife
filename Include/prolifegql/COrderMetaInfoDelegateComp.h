#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdbgql/TSdlBasedMetaInfoDelegate.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Orders.h>


namespace prolifegql
{


class COrderMetaInfoDelegateComp:
			public ilog::CLoggerComponentBase,
			public imtdbgql::TSdlBasedMetaInfoDelegate<sdl::prolife::Orders::COrderData::V1_0>
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(COrderMetaInfoDelegateComp);
		I_REGISTER_INTERFACE(imtdb::IJsonBasedMetaInfoDelegate);
	I_END_COMPONENT;

protected:
	virtual bool FillRepresentation(sdl::prolife::Orders::COrderData::V1_0& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const sdl::prolife::Orders::COrderData::V1_0& metaInfoRepresentation) const override;
};


} // namespace prolifegql


