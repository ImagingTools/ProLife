// Qt includes
#include <QtQml/QQmlEngine>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtQml>

// ACF includes
#include <ibase/IApplication.h>

// ImtCore includes
#include <GeneratedFiles/ProLifeServerConfigurator/CProLifeServerConfigurator.h>
#include <imtbase/CTreeItemModel.h>
#include <imtqml/CGqlModel.h>


int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(ProLifeServerConfigurator);
	Q_INIT_RESOURCE(imtgui);
	Q_INIT_RESOURCE(imtqml);
	Q_INIT_RESOURCE(prolifeqml);

	Q_INIT_RESOURCE(Webimtgui);

	qmlRegisterType<imtbase::CTreeItemModel>("Acf", 1, 0, "TreeItemModel");
	qmlRegisterType<imtqml::CGqlModel>("Acf", 1, 0, "GqlModel");

	CProLifeServerConfigurator instance;

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != nullptr){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


