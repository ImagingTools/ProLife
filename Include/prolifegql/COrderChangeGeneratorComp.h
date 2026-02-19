// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#pragma once


// ImtCore includes
#include <imtbase/CDocumentChangeGeneratorCompBase.h>
#include <imtbase/CObjectCollection.h>


namespace prolifegql
{


class COrderChangeGeneratorComp: public imtbase::CDocumentChangeGeneratorCompBase
{
public:
	typedef imtbase::CDocumentChangeGeneratorCompBase BaseClass;

	I_BEGIN_COMPONENT(COrderChangeGeneratorComp)
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "License collection", true, "LicenseCollection");
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_accountCollectionCompPtr, "AccountCollection", "Account collection", true, "AccountCollection");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
		I_ASSIGN(m_softwareCollectionCompPtr, "SoftwareProductCollection", "Software collection", true, "SoftwareProductCollection");
	I_END_COMPONENT

protected:
	virtual bool CompareDocuments(
				const istd::IChangeable& oldDocument,
				const istd::IChangeable& newDocument,
				imtbase::CObjectCollection& documentChangeCollection,
				QString& errorMessage) override;
	virtual QString CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId = QByteArray()) const override;
	virtual QString GetKeyNameForOperation(const QByteArray& key, const QByteArray& value) const override;

private:
	QString GetAccountName(const QByteArray& accountId) const;
	QString GetProductName(const QByteArray& productId) const;
	void GenerateDifferences(
				imtbase::IObjectCollection& prevOrderProducts,
				imtbase::IObjectCollection& currentOrderProducts,
				QByteArrayList& addProducts,
				QByteArrayList& removedProducts,
				QByteArrayList& updatedProducts) const;
protected:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_accountCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareCollectionCompPtr);
};


} // namespace prolifegql


