#pragma once


// ImtCore includes
#include <imtbase/CDocumentChangeGeneratorCompBase.h>
#include <imtbase/CObjectCollection.h>


namespace prolifegql
{


class CLicenseChangeGeneratorComp: public imtbase::CDocumentChangeGeneratorCompBase
{
public:
	typedef imtbase::CDocumentChangeGeneratorCompBase BaseClass;

	I_BEGIN_COMPONENT(CLicenseChangeGeneratorComp)
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "License collection", true, "LicenseCollection");
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
	I_END_COMPONENT

protected:
	virtual bool GenerateDocumentChanges(
				const QByteArray& operationTypeId,
				const QByteArray& documentId,
				const istd::IChangeable& document,
				imtbase::CObjectCollection& documentChangeCollection,
				QString& errorMessage,
				const iprm::IParamsSet* paramsPtr) override;
	virtual bool CompareDocuments(
				const istd::IChangeable& oldDocument,
				const istd::IChangeable& newDocument,
				imtbase::CObjectCollection& documentChangeCollection,
				QString& errorMessage) override;
	virtual QString GetOperationDescription(
				imtbase::CObjectCollection& documentChangeCollection,
				const QByteArray& languageId = QByteArray()) override;

protected:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
};


} // namespace prolifegql


