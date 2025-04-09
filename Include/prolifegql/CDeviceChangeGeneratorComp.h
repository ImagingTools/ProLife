#pragma once


// ImtCore includes
#include <imtbase/CDocumentChangeGeneratorCompBase.h>
#include <imtbase/CObjectCollection.h>


namespace prolifegql
{


class CDeviceChangeGeneratorComp: public imtbase::CDocumentChangeGeneratorCompBase
{
public:
	typedef imtbase::CDocumentChangeGeneratorCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceChangeGeneratorComp)
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "License collection", true, "LicenseCollection");
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_hardwareBindingCollectionCompPtr, "BindingCollection", "Binding collection", true, "BindingCollection");
		I_ASSIGN(m_softwareInstanceCollectionCompPtr, "SoftwareInstanceCollection", "Software instance collection", true, "SoftwareInstanceCollection");
	I_END_COMPONENT

protected:
	virtual bool GenerateDocumentChanges(
				const QByteArray& operationTypeId,
				const QByteArray& documentId,
				const istd::IChangeable* documentPtr,
				imtbase::CObjectCollection& documentChangeCollection,
				QString& errorMessage,
				const iprm::IParamsSet* paramsPtr) override;
	virtual bool CompareDocuments(const istd::IChangeable& oldDocument, const istd::IChangeable& newDocument, imtbase::CObjectCollection& documentChangeCollection, QString& errorMessage) override;
	virtual QString CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId = QByteArray()) const override;
	virtual QString GetKeyNameForOperation(const QByteArray& key, const QByteArray& value) const override;

private:
	QString GetLicenseName(const QByteArray& productUuid) const;
	QString GetSoftwareName(const QByteArray& softwareId) const;
	QString GetProductName(const QByteArray& productId) const;
	QString GetOrderName(const QByteArray& orderId) const;
	QString GetDeviceName(const QByteArray& deviceId) const;

protected:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_hardwareBindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareInstanceCollectionCompPtr);
};


} // namespace prolifegql


