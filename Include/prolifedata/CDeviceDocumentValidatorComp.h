// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentValidator.h>


namespace prolifedata
{


class CDeviceDocumentValidatorComp: public ilog::CLoggerComponentBase, virtual public imtdoc::IDocumentValidator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceDocumentValidatorComp);
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Device object collection for uniqueness validation", true, "ObjectCollection");
	I_END_COMPONENT;

	// reimplemented (imtdoc::IDocumentValidator)
	virtual bool ValidateDocumentData(const QByteArray& objectId, const istd::IChangeable& document, QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace prolifedata


