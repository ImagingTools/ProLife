#include <prolifelicgql/CProductCollectionControllerComp.h>


// Qt includes
#include <QtCore/QTranslator>

// ImtCore includes
#include <prolifeauth/CProductInfo.h>


namespace prolifelicgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CProductCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);

	if (metaInfoPtr.IsValid()){
		if (informationId == QByteArray("Name")){
			return metaInfoPtr->GetMetaInfo(prolifeauth::IProductInfo::MIT_PRODUCT_NAME);
		}
		else if (informationId == QByteArray("Description")){
			return metaInfoPtr->GetMetaInfo(prolifeauth::IProductInfo::MIT_PRODUCT_DESCRIPTION);
		}else if(informationId == QByteArray("Manufacturer"))
		{
			return metaInfoPtr->GetMetaInfo(prolifeauth::IProductInfo::MIT_PRODUCT_MANUFACTURER);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CProductCollectionControllerComp::GetMetaInfo(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* metaInfoModel = nullptr;
	imtbase::CTreeItemModel* children = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error");
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		metaInfoModel = new imtbase::CTreeItemModel();

		idoc::MetaInfoPtr metaInfoPtr;

		QByteArray ProductId = GetObjectIdFromInputParams(inputParams);

		imtbase::IObjectCollection::DataPtr dataPtr;

		const QTranslator* translatorPtr = nullptr;
		if (m_translationManagerCompPtr.IsValid()){
			QByteArray languageId;
			languageId = "en_US";
			if (languageId.isEmpty()){
				int currentIndex = iprm::FindOptionIndexById(languageId, m_translationManagerCompPtr->GetLanguagesInfo());
				if (currentIndex >= 0){
					translatorPtr = m_translationManagerCompPtr->GetLanguageTranslator(currentIndex);
				}
			}
		}

		if (!m_objectCollectionCompPtr->GetObjectData(ProductId, dataPtr)){
			errorMessage = QT_TR_NOOP("Unable to load an object data");
			return nullptr;
		}

		const prolifeauth::IProductInfo* ProductInfoPtr = dynamic_cast<const prolifeauth::IProductInfo*>(dataPtr.GetPtr());
		if (ProductInfoPtr == nullptr){
			if (translatorPtr != nullptr){
				errorMessage = translatorPtr->translate("imtlicgql::CProductCollectionControllerComp", "Unable to get an account info");
			}
			else{
				errorMessage = QT_TR_NOOP("Unable to get an Product info");
			}

			return nullptr;
		}
		int index = metaInfoModel->InsertNewItem();
		children = metaInfoModel->AddTreeModel("Children", index);
		QString description = ProductInfoPtr->GetProductDescription();

		children->SetData("Value", description);
		if (translatorPtr != nullptr){
			errorMessage = translatorPtr->translate("imtlicgql::CProductCollectionControllerComp", "Product Name");
		}
		else{
			metaInfoModel->SetData("Name", QT_TR_NOOP("Product description"), index);
		}
		children = metaInfoModel->AddTreeModel("Children", index);

		children->SetData("Value", description);

		QString manufacturer = ProductInfoPtr->GetProductManufacturer();

		index = metaInfoModel->InsertNewItem();
		if (translatorPtr != nullptr){
			errorMessage = translatorPtr->translate("imtlicgql::CProductCollectionControllerComp", "Product Name");
		}
		else{
			metaInfoModel->SetData("Name", QT_TR_NOOP("Product manufacturer"), index);
		}
		children = metaInfoModel->AddTreeModel("Children", index);
		children->SetData("Value", manufacturer);

		QString name = ProductInfoPtr->GetProductName();

//		children->SetData("Value", name);

		dataModel->SetExternTreeModel("metaInfo", metaInfoModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtlicgql


