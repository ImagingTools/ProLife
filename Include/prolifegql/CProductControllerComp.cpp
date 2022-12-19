#include <prolifegql/CProductControllerComp.h>


// ProLife includes
#include <prolifedata/CProductInfo.h>


namespace prolifegql
{


imtbase::CHierarchicalItemModelPtr CProductControllerComp::GetObject(
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
	dataModel->SetData("Name", "");
	dataModel->SetData("Description", "");
	dataModel->SetData("Manufacturer", "");


	QByteArray ProductId = GetObjectIdFromInputParams(inputParams);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(ProductId, dataPtr)){
        const prolifedata::IProductInfo* productInfoPtr = dynamic_cast<const prolifedata::IProductInfo*>(dataPtr.GetPtr());

		if (productInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an product info");
			return imtbase::CHierarchicalItemModelPtr();
		}

		QString ProductName = productInfoPtr->GetProductName();
		QString ProductDescription = productInfoPtr->GetProductDescription();
		QString ProductManufacturer = productInfoPtr->GetProductManufacturer();

		QByteArray ProductId = ProductName.toUtf8();

		dataModel->SetData("Id", ProductId);
		dataModel->SetData("Name", ProductName);
		dataModel->SetData("Description", ProductDescription);
		dataModel->SetData("Manufacturer", ProductManufacturer);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


istd::IChangeable* CProductControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray &objectId,
			QString &name,
			QString &description,
			QString& errorMessage) const
{
	if (!m_productInfoFactCompPtr.IsValid()){
		errorMessage = QObject::tr("Can not create Product: %1").arg(QString(objectId));
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
        istd::TDelPtr<prolifedata::CProductInfo> productInfoPtr = new prolifedata::CProductInfo();

		if (productInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an Product info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			productInfoPtr->SetProductName(name);
			objectId = name.toUtf8();

			if (objectId.isEmpty()){
				errorMessage = QT_TR_NOOP("Product name can't be empty!");
				return nullptr;
			}
		}

		if (itemModel.ContainsKey("Description")){
			description = itemModel.GetData("Description").toString();
			productInfoPtr->SetProductDescription(description);
		}

		if (itemModel.ContainsKey("Manufacturer")){
			QString manufacturer = itemModel.GetData("Manufacturer").toString();
			productInfoPtr->SetProductManufacturer(manufacturer);
		}

		return productInfoPtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create Product: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace prolifegql


