#include <prolifegql/CCustomerChangeGeneratorComp.h>


// ImtCore includes
#include <imtauth/IUserGroupInfo.h>
#include <imtauth/IAddress.h>

// ProLife includes
#include <prolifedata/CCustomerInfo.h>


namespace prolifegql
{


// protected methods

bool CCustomerChangeGeneratorComp::CompareDocuments(
	const istd::IChangeable& oldDocument,
	const istd::IChangeable& newDocument,
	imtbase::CObjectCollection& documentChangeCollection,
	QString& errorMessage)
{
	const prolifedata::CCustomerInfo* oldCustomerInfoPtr = dynamic_cast<const prolifedata::CCustomerInfo*>(&oldDocument);
	if (oldCustomerInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. Old document is invalid");
		return false;
	}

	const prolifedata::CCustomerInfo* newCustomerInfoPtr = dynamic_cast<const prolifedata::CCustomerInfo*>(&newDocument);
	if (newCustomerInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. New document is invalid");
		return false;
	}

	InsertChange(documentChangeCollection, "CustomerId", QT_TRANSLATE_NOOP("Attribute", "Customer-ID"), oldCustomerInfoPtr->GetCustomerId(), newCustomerInfoPtr->GetCustomerId());
	InsertTextChange(documentChangeCollection, "Name", QT_TRANSLATE_NOOP("Attribute", "Customer Name"), oldCustomerInfoPtr->GetName(), newCustomerInfoPtr->GetName());
	InsertTextChange(documentChangeCollection, "Description", QT_TRANSLATE_NOOP("Attribute", "Description"), oldCustomerInfoPtr->GetDescription(), newCustomerInfoPtr->GetDescription());
	InsertTextChange(documentChangeCollection, "Email", QT_TRANSLATE_NOOP("Attribute", "Email"), oldCustomerInfoPtr->GetEmail(), newCustomerInfoPtr->GetEmail());

	InsertListChanges(
				documentChangeCollection,
				"AddGroup",
				"RemoveGroup",
				"GroupId",
				QT_TRANSLATE_NOOP("Attribute", "Group"),
				oldCustomerInfoPtr->GetGroups(),
				newCustomerInfoPtr->GetGroups());

	CompareAddresses(oldCustomerInfoPtr->GetAddresses(), newCustomerInfoPtr->GetAddresses(), documentChangeCollection);

	return true;
}


void CCustomerChangeGeneratorComp::CompareAddresses(
	const imtauth::IAddressProvider* oldAddressProviderPtr,
	const imtauth::IAddressProvider* newAddressProviderPtr,
	imtbase::CObjectCollection& documentChangeCollection)
{
	const QByteArrayList oldAddressIds = (oldAddressProviderPtr != nullptr) ? oldAddressProviderPtr->GetAddressList().GetElementIds() : QByteArrayList();
	const QByteArrayList newAddressIds = (newAddressProviderPtr != nullptr) ? newAddressProviderPtr->GetAddressList().GetElementIds() : QByteArrayList();

	// Address-IDs are regenerated every time the customer is rebuilt from a request, so they cannot be
	// used to match an address of the stored document against one of the incoming document.
	if (oldAddressIds.size() == 1 && newAddressIds.size() == 1){
		CompareAddressFields(
					*oldAddressProviderPtr,
					oldAddressIds.first(),
					*newAddressProviderPtr,
					newAddressIds.first(),
					documentChangeCollection);

		return;
	}

	// With several addresses only their rendered text can tell which one was really added or removed.
	QStringList oldAddresses;
	for (const QByteArray& addressId : oldAddressIds){
		const QString address = FormatAddress(oldAddressProviderPtr->GetAddress(addressId));
		if (!address.isEmpty()){
			oldAddresses << address;
		}
	}

	QStringList newAddresses;
	for (const QByteArray& addressId : newAddressIds){
		const QString address = FormatAddress(newAddressProviderPtr->GetAddress(addressId));
		if (!address.isEmpty()){
			newAddresses << address;
		}
	}

	QStringList remainingOldAddresses = oldAddresses;
	for (const QString& address : std::as_const(newAddresses)){
		if (!remainingOldAddresses.removeOne(address)){
			InsertOperationDescription(
						documentChangeCollection,
						"AddAddress",
						"Address",
						QT_TRANSLATE_NOOP("Attribute", "Address"),
						QByteArray(),
						address.toUtf8());
		}
	}

	for (const QString& address : std::as_const(remainingOldAddresses)){
		InsertOperationDescription(
					documentChangeCollection,
					"RemoveAddress",
					"Address",
					QT_TRANSLATE_NOOP("Attribute", "Address"),
					address.toUtf8(),
					QByteArray());
	}
}


void CCustomerChangeGeneratorComp::CompareAddressFields(
	const imtauth::IAddressProvider& oldAddressProvider,
	const QByteArray& oldAddressId,
	const imtauth::IAddressProvider& newAddressProvider,
	const QByteArray& newAddressId,
	imtbase::CObjectCollection& documentChangeCollection)
{
	const imtauth::IAddress* oldAddressPtr = oldAddressProvider.GetAddress(oldAddressId);
	const imtauth::IAddress* newAddressPtr = newAddressProvider.GetAddress(newAddressId);
	if (oldAddressPtr == nullptr || newAddressPtr == nullptr){
		return;
	}

	InsertTextChange(documentChangeCollection, "Country", QT_TRANSLATE_NOOP("Attribute", "Country"), oldAddressPtr->GetCountry(), newAddressPtr->GetCountry());
	InsertTextChange(documentChangeCollection, "City", QT_TRANSLATE_NOOP("Attribute", "City"), oldAddressPtr->GetCity(), newAddressPtr->GetCity());
	InsertTextChange(documentChangeCollection, "Street", QT_TRANSLATE_NOOP("Attribute", "Street"), oldAddressPtr->GetStreet(), newAddressPtr->GetStreet());
	InsertNumberChange(documentChangeCollection, "PostalCode", QT_TRANSLATE_NOOP("Attribute", "Postal Code"), oldAddressPtr->GetPostalCode(), newAddressPtr->GetPostalCode());
	InsertEnumChange(
				documentChangeCollection,
				"AddressCategory",
				QT_TRANSLATE_NOOP("Attribute", "Address Category"),
				oldAddressProvider.GetAddressCategory(oldAddressId),
				newAddressProvider.GetAddressCategory(newAddressId),
				imtauth::IAddressProvider::AddressCategoryGetStrings());
}


QString CCustomerChangeGeneratorComp::FormatAddress(const imtauth::IAddress* addressPtr)
{
	if (addressPtr == nullptr){
		return QString();
	}

	QStringList parts;
	if (!addressPtr->GetStreet().isEmpty()){
		parts << addressPtr->GetStreet();
	}
	if (addressPtr->GetPostalCode() > 0){
		parts << QString::number(addressPtr->GetPostalCode());
	}
	if (!addressPtr->GetCity().isEmpty()){
		parts << addressPtr->GetCity();
	}
	if (!addressPtr->GetCountry().isEmpty()){
		parts << addressPtr->GetCountry();
	}

	return parts.join(", ");
}


QString CCustomerChangeGeneratorComp::CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId) const
{
	static const QByteArray translationContext = QByteArrayLiteral("prolifegql::CCustomerChangeGeneratorComp");

	const QByteArray typeId = operationDescription.GetOperationTypeId();
	const QByteArray oldValue = operationDescription.GetOldValue();
	const QByteArray newValue = operationDescription.GetNewValue();

	if (typeId == "AddGroup"){
		return Translate(QT_TR_NOOP("Added group '%1'"), languageId, translationContext).arg(GetGroupName(newValue).toHtmlEscaped());
	}

	if (typeId == "RemoveGroup"){
		return Translate(QT_TR_NOOP("Removed group '%1'"), languageId, translationContext).arg(GetGroupName(oldValue).toHtmlEscaped());
	}

	if (typeId == "AddAddress"){
		return Translate(QT_TR_NOOP("Added address '%1'"), languageId, translationContext).arg(QString::fromUtf8(newValue).toHtmlEscaped());
	}

	if (typeId == "RemoveAddress"){
		return Translate(QT_TR_NOOP("Removed address '%1'"), languageId, translationContext).arg(QString::fromUtf8(oldValue).toHtmlEscaped());
	}

	return QString();
}


// private methods

QString CCustomerChangeGeneratorComp::GetGroupName(const QByteArray& groupId) const
{
	if (!IsUuid(groupId)){
		return groupId;
	}

	if (m_groupCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_groupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
			const imtauth::IUserGroupInfo* groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
			if (groupInfoPtr != nullptr){
				return groupInfoPtr->GetName();
			}
		}
	}

	// Falling back to the raw ID keeps a deleted group traceable in the history.
	return groupId;
}


} // namespace prolifegql


