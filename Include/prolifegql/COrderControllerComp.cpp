#include <prolifegql/COrderControllerComp.h>

// ImtCore includes
#include <imtlic/CFeaturePackageCollectionUtility.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <imtlic/CLicenseInstance.h>
#include <prolifedata/IOrderedProductInfo.h>
#include <imtbase/ICollectionInfo.h>


namespace prolifegql
{


imtbase::CHierarchicalItemModelPtr COrderControllerComp::GetObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			const imtgql::IGqlContext* gqlContext,
			QString& errorMessage) const
{
	imtbase::CHierarchicalItemModelPtr rootModel(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return imtbase::CHierarchicalItemModelPtr();
	}

	dataModel->SetData("Id", "");
	dataModel->SetData("AccountId", "");
//	dataModel->SetData("ProductId", "");

	imtbase::CTreeItemModel* productsModel = dataModel->AddTreeModel("Products");
	imtbase::CTreeItemModel* activeLicenses = productsModel->AddTreeModel("ActiveLicenses");


	QByteArray objectId = GetObjectIdFromInputParams(inputParams);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::IOrderInfo* orderPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());

		if (orderPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an product instance");
			return imtbase::CHierarchicalItemModelPtr();
		}

//		QByteArray instanceId = objectId;
//		if (m_separatorObjectIdAttrPtr.IsValid()){
//			QString objectIdStr = objectId;
//			QStringList splitData = objectIdStr.split(*m_separatorObjectIdAttrPtr);
//			instanceId = splitData[0].toUtf8();
//		}

		QByteArray orderId = orderPtr->GetOrderId();
		QByteArray customerId = orderPtr->GetCustomerId();

//		dataModel->SetData("Id", instanceId);

		QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
		dataModel->SetData("Name", name);

		dataModel->SetData("OrderId", orderId);
		dataModel->SetData("CustomerId", customerId);

		prolifedata::IOrderInfo::ProductIds orderedProducts = orderPtr->GetProducts();

		for(const QByteArray& productId : orderedProducts)
		{
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_productCollectionCompPtr->GetObjectData(productId, dataPtr)){
				const prolifedata::IOrderedProductInfo* productPtr = dynamic_cast<const prolifedata::IOrderedProductInfo*>(dataPtr.GetPtr());
				if(productPtr != nullptr)
				{
					int productIndex = productsModel->InsertNewItem();
					QByteArray productId = productPtr->GetProductId();
					QByteArray productInstance = productPtr->GetProductInstanceId();

					productsModel -> SetData("ProductId", productId, productIndex);
					productsModel -> SetData("ProductInstance", productInstance, productIndex);

					const imtbase::ICollectionInfo& licenseInstances = productPtr->GetLicenseInstances();
					imtbase::ICollectionInfo::Ids activeLicenseIds = licenseInstances.GetElementIds();

					for (const QByteArray& activeLicenseId : activeLicenseIds){
						const imtlic::ILicenseInstance* licenseInstancePtr = productPtr->GetLicenseInstance(activeLicenseId);

						int index = activeLicenses->InsertNewItem();

						QString licenseName = licenseInstancePtr->GetLicenseName();

						activeLicenses->SetData("Id", activeLicenseId, index);
						activeLicenses->SetData("Name", name, index);

						QDate date = licenseInstancePtr->GetExpiration().date();
						QString licenseExpiration = date.toString("yyyy-MM-dd");
						activeLicenses->SetData("Expiration", licenseExpiration, index);
					}
				}
			}
		}

	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


istd::IChangeable* COrderControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray& objectId,
		QString& name,
		QString& description,
		QString &errorMessage) const
{
	if (inputParams.isEmpty()) {
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<prolifedata::IOrderInfo> orderPtr = m_orderPtr.CreateInstance();
		if (!orderPtr.IsValid()) {
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		QByteArray orderId;

		if (itemModel.ContainsKey("OrderId")){
			orderId = itemModel.GetData("OrderId").toByteArray();
		}

		if (orderId.isEmpty()){
			errorMessage = QT_TR_NOOP("Order-ID can not be empty!");
			return nullptr;
		}

//		if (itemModel.ContainsKey("Name")){
//			name = itemModel.GetData("Name").toByteArray();
//		}

		QByteArray customerId;
		if (itemModel.ContainsKey("CustomerId")){
			customerId = itemModel.GetData("CustomerId").toByteArray();
		}

		if (customerId.isEmpty()){
			errorMessage = QT_TR_NOOP("Customer can not be empty!");
			return nullptr;
		}

//		QByteArray productId;
//		if (itemModel.ContainsKey("ProductId")){
//			productId = itemModel.GetData("ProductId").toByteArray();
//		}

//		if (productId.isEmpty()){
//			errorMessage = QT_TR_NOOP("Product can not be empty!");
//			return nullptr;
//		}

		objectId = orderId;

		if (m_separatorObjectIdAttrPtr.IsValid()){
			objectId += *m_separatorObjectIdAttrPtr + customerId;
		}
		orderPtr->SetOrderId(orderId);
		orderPtr->SetCustomerId(customerId);

		imtbase::CTreeItemModel* orderedProducts = itemModel.GetTreeItemModel("Products");
		if(orderedProducts != nullptr)
		{
			for(int productIdx = 0; productIdx < orderedProducts->GetItemsCount(); productIdx++)
			{
				QByteArray productId;
				if(orderedProducts->ContainsKey("ProductId"))
				{
					productId = orderedProducts->GetData("ProductId", productIdx).toByteArray();
				}
				orderPtr->AddProduct(productId);
			}
		}
//		imtbase::CTreeItemModel* activeLicenses = itemModel.GetTreeItemModel("ActiveLicenses");
//		if (activeLicenses != nullptr){
//			for (int i = 0; i < activeLicenses->GetItemsCount(); i++){
//				QByteArray licenseId;
//				if (activeLicenses->ContainsKey("Id")){
//					licenseId = activeLicenses->GetData("Id", i).toByteArray();
//				}

//				QDateTime expirationDate;
//				if (activeLicenses->ContainsKey("Expiration")){
//					QString dateExpirationStr = activeLicenses->GetData("Expiration", i).toString();
//					expirationDate = QDateTime::fromString(dateExpirationStr, "yyyy-MM-dd");
//				}

////				orderPtr->AddLicense(licenseId, expirationDate);
//			}
//		}

		return orderPtr.PopPtr();
	}
	errorMessage = QObject::tr("Can not create order: %1").arg(QString(objectId));


	return nullptr;
}


} // namespace prolifegql


