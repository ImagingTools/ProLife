// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ImtCore includes
#include <imtbase/CDocumentChangeGeneratorCompBase.h>
#include <imtbase/CObjectCollection.h>


namespace prolifegql
{


class CCustomerChangeGeneratorComp: public imtbase::CDocumentChangeGeneratorCompBase
{
public:
	typedef imtbase::CDocumentChangeGeneratorCompBase BaseClass;

	I_BEGIN_COMPONENT(CCustomerChangeGeneratorComp)
		I_ASSIGN(m_groupCollectionCompPtr, "GroupCollection", "Group collection", true, "GroupCollection");
	I_END_COMPONENT

protected:
	virtual bool CompareDocuments(
				const istd::IChangeable& oldDocument,
				const istd::IChangeable& newDocument,
				imtbase::CObjectCollection& documentChangeCollection,
				QString& errorMessage) override;
	virtual QString CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId = QByteArray()) const override;
	void GenerateDifferences(
				imtbase::IObjectCollection& prevOrderProducts,
				imtbase::IObjectCollection& currentOrderProducts,
				QByteArrayList& addProducts,
				QByteArrayList& removedProducts,
				QByteArrayList& updatedProducts) const;

private:
	QString GetGroupName(const QByteArray& groupId) const;

protected:
	I_REF(imtbase::IObjectCollection, m_groupCollectionCompPtr);
};


} // namespace prolifegql


