#include <prolifegql/COrderChangeGeneratorComp.h>


// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <imtbase/imtbase.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/ILicenseDefinition.h>
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtauth/CCompanyInfo.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


// protected methods

bool COrderChangeGeneratorComp::CompareDocuments(
				const istd::IChangeable& oldDocument,
				const istd::IChangeable& newDocument,
				imtbase::CObjectCollection& documentChangeCollection,
				QString& errorMessage)
{
	prolifedata::CIdentifiableOrderInfo* oldOrderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(const_cast<istd::IChangeable*>(&oldDocument));
	if (oldOrderInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. Old document is invalid");

		return false;
	}

	prolifedata::CIdentifiableOrderInfo* newOrderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(const_cast<istd::IChangeable*>(&newDocument));
	if (newOrderInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. New document is invalid");

		return false;
	}

	QByteArray oldOrderId = oldOrderInfoPtr->GetOrderId();
	QByteArray newOrderId = newOrderInfoPtr->GetOrderId();
	if (oldOrderId != newOrderId){
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("", "OrderId", QT_TRANSLATE_NOOP("Attribute", "Order-ID"), oldOrderId, newOrderId), "OrderId");
	}

	QByteArray oldPurchaseOrderId = oldOrderInfoPtr->GetPurchaseOrderId();
	QByteArray newPurchaseOrderId = newOrderInfoPtr->GetPurchaseOrderId();
	if (oldPurchaseOrderId != newPurchaseOrderId){
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("", "PurchaseId", QT_TRANSLATE_NOOP("Attribute", "Purchase Order-ID"), oldPurchaseOrderId, newPurchaseOrderId), "PurchaseId");
	}

	QByteArray oldCustomerId = oldOrderInfoPtr->GetCustomerId();
	QByteArray newCustomerId = newOrderInfoPtr->GetCustomerId();
	if (oldCustomerId != newCustomerId){
		if (m_accountCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr oldDataPtr;
			if (m_accountCollectionCompPtr->GetObjectData(oldCustomerId, oldDataPtr)){
				const imtauth::CIdentifiableCompanyInfo* companyInfoPtr = dynamic_cast<const imtauth::CIdentifiableCompanyInfo*>(oldDataPtr.GetPtr());
				if (companyInfoPtr != nullptr){
					oldCustomerId = companyInfoPtr->GetName().toUtf8();
				}
			}

			imtbase::IObjectCollection::DataPtr newDataPtr;
			if (m_accountCollectionCompPtr->GetObjectData(newCustomerId, newDataPtr)){
				const imtauth::CIdentifiableCompanyInfo* companyInfoPtr = dynamic_cast<const imtauth::CIdentifiableCompanyInfo*>(newDataPtr.GetPtr());
				if (companyInfoPtr != nullptr){
					newCustomerId = companyInfoPtr->GetName().toUtf8();
				}
			}
		}

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("", "OrderCustomer", QT_TRANSLATE_NOOP("Attribute", "Order Customer"), oldCustomerId, newCustomerId), "OrderCustomer");
	}

	prolifedata::IOrderInfo::OrderStatus oldStatus = oldOrderInfoPtr->GetOrderStatus();
	prolifedata::IOrderInfo::OrderStatus newStatus = newOrderInfoPtr->GetOrderStatus();
	if (oldStatus != newStatus){
		QStringList statuses = oldOrderInfoPtr->OrderStatusGetStrings();
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("", "Status", QT_TRANSLATE_NOOP("Attribute", "Status"), statuses[oldStatus].toUtf8(), statuses[newStatus].toUtf8()), "Status");
	}

	QString oldDescription = oldOrderInfoPtr->GetDescription();
	QString newDescription = newOrderInfoPtr->GetDescription();
	if (oldDescription != newDescription){
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("", "Description", QT_TRANSLATE_NOOP("Attribute", "Description"), oldDescription.toUtf8(), newDescription.toUtf8()), "Description");
	}

	QByteArrayList addedProducts;
	QByteArrayList removedProducts;
	QByteArrayList updatedProducts;

	imtbase::IObjectCollection* oldProductCollectionPtr = oldOrderInfoPtr->GetProducts();
	imtbase::IObjectCollection* newProductCollectionPtr = newOrderInfoPtr->GetProducts();

	if (oldProductCollectionPtr != nullptr && newProductCollectionPtr != nullptr){
		GenerateDifferences(*oldProductCollectionPtr, *newProductCollectionPtr, addedProducts, removedProducts, updatedProducts);
	}

	for (const QByteArray& productObjectId : std::as_const(addedProducts)){
		QByteArray productUuid;
		QByteArray internalId;

		imtbase::IObjectCollection::DataPtr productDataPtr;
		if (m_softwareCollectionCompPtr->GetObjectData(productObjectId, productDataPtr)){
			const imtlic::IProductInstanceInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
			if (productInfoPtr != nullptr){
				productUuid = productInfoPtr->GetProductId();
				internalId = productInfoPtr->GetSerialNumber();
			}
		}
		else{
			if (m_deviceCollectionCompPtr->GetObjectData(productObjectId, productDataPtr)){
				const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(productDataPtr.GetPtr());
				if (deviceInfoPtr != nullptr){
					productUuid = deviceInfoPtr->GetDeviceType();
					internalId = deviceInfoPtr->GetMacAddress();
				}
			}
		}

		QByteArray productId;
		if (m_productCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
				const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(dataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					productId = productInfoPtr->GetName().toUtf8();
				}
			}
		}

		QString name = productId;
		if (!internalId.isEmpty()){
			name = name + " (" + internalId + ")";
		}

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("AddProduct", "ProductId", name, "", productId));
	}

	for (const QByteArray& productObjectId : std::as_const(removedProducts)){
		QByteArray productId;
		QByteArray internalId;

		imtbase::IObjectCollection::DataPtr productDataPtr;
		if (m_softwareCollectionCompPtr->GetObjectData(productObjectId, productDataPtr)){
			const imtlic::IProductInstanceInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
			if (productInfoPtr != nullptr){
				productId = productInfoPtr->GetProductId();

				internalId = productInfoPtr->GetSerialNumber();
			}
		}
		else{
			if (m_deviceCollectionCompPtr->GetObjectData(productObjectId, productDataPtr)){
				const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(productDataPtr.GetPtr());
				if (deviceInfoPtr != nullptr){
					productId = deviceInfoPtr->GetDeviceType();

					internalId = deviceInfoPtr->GetMacAddress();
				}
			}
		}

		if (m_productCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_productCollectionCompPtr->GetObjectData(productId, dataPtr)){
				const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(dataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					productId = productInfoPtr->GetName().toUtf8();
				}
			}
		}

		QString name = productId;
		if (!internalId.isEmpty()){
			name = name + " (" + internalId + ")";
		}

		documentChangeCollection.InsertNewObject(
			"OperationInfo",
			"",
			"",
			CreateOperationDescription("RemoveProduct", "ProductId", name, productId, ""));
	}

	return true;
}


QString COrderChangeGeneratorComp::GetOperationDescription(imtbase::CObjectCollection& documentChangeCollection, const QByteArray& languageId)
{
	QString retVal = BaseClass::GetOperationDescription(documentChangeCollection, languageId);

	if (retVal.isEmpty()){
		imtbase::ICollectionInfo::Ids elementIds = documentChangeCollection.GetElementIds();
		for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (documentChangeCollection.GetObjectData(elementId, dataPtr)){
				const imtbase::COperationDescription* operationDescriptionPtr = dynamic_cast<const imtbase::COperationDescription*>(dataPtr.GetPtr());
				if (operationDescriptionPtr != nullptr){
					QByteArray type = operationDescriptionPtr->GetOperationTypeId();

					QString keyName = operationDescriptionPtr->GetKeyName();
					keyName = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), keyName.toUtf8(), languageId, "Attribute");

					if (type == "AddProduct"){
						QString change = imtbase::GetTranslation(
									m_translationManagerCompPtr.GetPtr(),
									QString(QT_TR_NOOP("Added the product '%1'")).toUtf8(),
									languageId,
									"prolifegql::COrderChangeGeneratorComp");

						change = change.arg(keyName);

						retVal += change + "\n";
					}
					else if (type == "RemoveProduct"){
						QString change = imtbase::GetTranslation(
									m_translationManagerCompPtr.GetPtr(),
									QString(QT_TR_NOOP("Removed the product '%1'")).toUtf8(),
									languageId,
									"prolifegql::COrderChangeGeneratorComp");

						change = change.arg(keyName);

						retVal += change + "\n";
					}
				}
			}
		}
	}

	return retVal;
}


void COrderChangeGeneratorComp::GenerateDifferences(
			imtbase::IObjectCollection& prevOrderProducts,
			imtbase::IObjectCollection& currentOrderProducts,
			QByteArrayList& addProducts,
			QByteArrayList& removedProducts,
			QByteArrayList& /*updatedProducts*/) const
{
	imtbase::ICollectionInfo::Ids prevProductIds = prevOrderProducts.GetElementIds();
	imtbase::ICollectionInfo::Ids currentProductIds = currentOrderProducts.GetElementIds();

	QByteArrayList oldProductUuids;
	QByteArrayList newProductUuids;

	for (const imtbase::ICollectionInfo::Id& productId : currentProductIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (currentOrderProducts.GetObjectData(productId, dataPtr)){
			const imtbase::CObjectLink* objectLinkPtr = dynamic_cast<const imtbase::CObjectLink*>(dataPtr.GetPtr());
			if (objectLinkPtr != nullptr){
				newProductUuids << objectLinkPtr->GetObjectUuid();
			}
		}
	}

	for (const imtbase::ICollectionInfo::Id& productId : prevProductIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (prevOrderProducts.GetObjectData(productId, dataPtr)){
			const imtbase::CObjectLink* objectLinkPtr = dynamic_cast<const imtbase::CObjectLink*>(dataPtr.GetPtr());
			if (objectLinkPtr != nullptr){
				oldProductUuids << objectLinkPtr->GetObjectUuid();
			}
		}
	}

	for (const imtbase::ICollectionInfo::Id& productId : newProductUuids){
		if (!oldProductUuids.contains(productId)){
			addProducts << productId;
		}
	}

	for (const imtbase::ICollectionInfo::Id& productId : oldProductUuids){
		if (!newProductUuids.contains(productId)){
			removedProducts << productId;
		}
	}
}


} // namespace prolifegql


