// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ImtCore includes
#include <imtdb/CJsonBasedMetaInfoDelegateComp.h>


namespace prolifegql
{


class CCustomerMetaInfoDelegateComp: public imtdb::CJsonBasedMetaInfoDelegateComp
{
public:
	typedef imtdb::CJsonBasedMetaInfoDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CCustomerMetaInfoDelegateComp);
	I_END_COMPONENT;

protected:
	virtual bool FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo, const QByteArray& typeId) const override;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation, const QByteArray& typeId) const override;
};


} // namespace prolifegql


