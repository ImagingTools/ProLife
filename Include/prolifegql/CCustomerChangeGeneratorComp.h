#pragma once


// ImtCore includes
#include <imtbase/CDocumentChangeGeneratorCompBase.h>
#include <imtbase/CObjectCollection.h>
#include <imtauth/IAddressProvider.h>


namespace imtauth
{


class IAddress;


} // namespace imtauth


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

	/**
		Compare the address lists of both documents and record added, removed and modified addresses.
	*/
	void CompareAddresses(
				const imtauth::IAddressProvider* oldAddressProviderPtr,
				const imtauth::IAddressProvider* newAddressProviderPtr,
				imtbase::CObjectCollection& documentChangeCollection);

private:
	/**
		Record the field-by-field differences of two addresses which are known to describe the same location.
	*/
	void CompareAddressFields(
				const imtauth::IAddressProvider& oldAddressProvider,
				const QByteArray& oldAddressId,
				const imtauth::IAddressProvider& newAddressProvider,
				const QByteArray& newAddressId,
				imtbase::CObjectCollection& documentChangeCollection);

	static QString FormatAddress(const imtauth::IAddress* addressPtr);

	QString GetGroupName(const QByteArray& groupId) const;

protected:
	I_REF(imtbase::IObjectCollection, m_groupCollectionCompPtr);
};


} // namespace prolifegql


