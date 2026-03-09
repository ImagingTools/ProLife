// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

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
	virtual bool FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo, const QByteArray& typeId) const override;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation, const QByteArray& typeId) const override;
};


} // namespace prolifegql


