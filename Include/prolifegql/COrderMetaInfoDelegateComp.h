#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/CJsonBasedMetaInfoDelegateComp.h>


namespace prolifegql
{


class COrderMetaInfoDelegateComp: public imtdb::CJsonBasedMetaInfoDelegateComp
{
public:
	typedef imtdb::CJsonBasedMetaInfoDelegateComp BaseClass;

	I_BEGIN_COMPONENT(COrderMetaInfoDelegateComp);
	I_END_COMPONENT;

protected:
	virtual bool FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const override;
};


} // namespace prolifegql


