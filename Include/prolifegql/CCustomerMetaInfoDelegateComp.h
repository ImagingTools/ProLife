#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdbgql/TSdlBasedMetaInfoDelegate.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Accounts.h>


namespace prolifegql
{


class CCustomerMetaInfoDelegateComp:
			public ilog::CLoggerComponentBase,
			public imtdbgql::TSdlBasedMetaInfoDelegate<sdl::prolife::Accounts::CAccountData::V1_0>
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCustomerMetaInfoDelegateComp);
		I_REGISTER_INTERFACE(imtdb::IJsonBasedMetaInfoDelegate);
	I_END_COMPONENT;

protected:
	virtual bool FillRepresentation(sdl::prolife::Accounts::CAccountData::V1_0& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const sdl::prolife::Accounts::CAccountData::V1_0& metaInfoRepresentation) const override;
};


} // namespace prolifegql


