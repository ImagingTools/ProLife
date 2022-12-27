#include <imtlicgql/CInstallationCollectionControllerComp.h>


// ImtCore includes
#include <prolifedata/IOrderedProductInfo.h>
#include <imtlic/ILicenseInstance.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CInstallationCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);



	return QVariant();
}


imtbase::CHierarchicalItemModelPtr CInstallationCollectionControllerComp::GetMetaInfo(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	imtbase::CHierarchicalItemModelPtr rootModel(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* metaInfoModel = nullptr;
	imtbase::CTreeItemModel* children = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		metaInfoModel = new imtbase::CTreeItemModel();

		idoc::MetaInfoPtr metaInfo;

		QByteArray objectId = GetObjectIdFromInputParams(inputParams);
		QByteArray productInstanceId = objectId;


		dataModel->SetExternTreeModel("metaInfo", metaInfoModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtlicgql


