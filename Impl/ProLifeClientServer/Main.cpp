// Qt includes
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include <QtQml/QQmlEngine>
#include <QQmlEngine>
#include <QtQml>

// ACF includes
#include <ibase/IApplication.h>

// ImtCore includes
#include <imtstyle/CImtStyle.h>
#include <imtbase/CTreeItemModel.h>
#include <imtqml/CGqlModel.h>
#include <imtqml/CRemoteFileController.h>
#include <imtqml/CQuickApplicationComp.h>

#include <GeneratedFiles/ProLifeClientServer/CProLifeClientServer.h>


int main(int argc, char *argv[])
{
#ifdef WEB_COMPILE
	Q_INIT_RESOURCE(prolifeqmlWeb);
#endif

	Q_INIT_RESOURCE(imtauthgui);
	Q_INIT_RESOURCE(imtqml);
	Q_INIT_RESOURCE(imtstyle);
	Q_INIT_RESOURCE(imtgui);
	Q_INIT_RESOURCE(prolifeqml);
	Q_INIT_RESOURCE(ImtCoreLoc);
	Q_INIT_RESOURCE(ProLifeLoc);

	Q_INIT_RESOURCE(imtresthtml);
	Q_INIT_RESOURCE(imtlicgui);

	Q_INIT_RESOURCE(Webimt3dgui);
	Q_INIT_RESOURCE(Webimtauthgui);
	Q_INIT_RESOURCE(Webimtgui);
	Q_INIT_RESOURCE(Webimtlicgui);

	imtstyle::CImtStyle* imtStylePtr = imtstyle::CImtStyle::GetInstance();
	Q_ASSERT(imtStylePtr != nullptr);
	
	CProLifeClientServer instance;

	qmlRegisterType<imtbase::CTreeItemModel>("Acf", 1, 0, "TreeItemModel");
	qmlRegisterType<imtqml::CGqlModel>("Acf", 1, 0, "GqlModel");
	qmlRegisterType<imtqml::CRemoteFileController>("Acf", 1, 0, "RemoteFileController");

	imtbase::CTreeItemModel *mainModel = new imtbase::CTreeItemModel();
	mainModel->SetIsArray(true);
	for (int i = 0; i < 3; i++){
		mainModel->InsertNewItem();
		imtbase::CTreeItemModel *secondModel = new imtbase::CTreeItemModel();
		secondModel->SetIsArray(true);
		for (int j = 0; j < 4; j++){
			secondModel->InsertNewItem();
			secondModel->SetData("Id", j);
			secondModel->SetData("Value", j * 5.2,j);
		}
		mainModel->SetExternTreeModel("", secondModel,i);

	}

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != nullptr){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


