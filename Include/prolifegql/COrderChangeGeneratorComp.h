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

	/**
		Compare the customer role collections of both documents and record added and removed roles.
	*/
	void CompareCustomerRoles(
				const imtbase::IObjectCollection* oldRolesPtr,
				const imtbase::IObjectCollection* newRolesPtr,
				imtbase::CObjectCollection& documentChangeCollection);

private:
	/// Role type-ID and customer-ID of an assigned customer role.
	typedef QPair<QByteArray, QByteArray> CustomerRole;
	typedef QList<CustomerRole> CustomerRoles;

	static CustomerRoles GetCustomerRoles(const imtbase::IObjectCollection* rolesPtr);
	static QByteArrayList GetProductUuids(const imtbase::IObjectCollection* productCollectionPtr);

	QString GetAccountName(const QByteArray& accountId) const;
	QString GetProductName(const QByteArray& productId) const;

protected:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_accountCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareCollectionCompPtr);
};


} // namespace prolifegql


