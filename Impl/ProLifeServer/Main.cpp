// Qt includes
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

// ACF includes
#include <ibase/IApplication.h>

// ImtCore includes
#include <GeneratedFiles/ProLifeServer/CProLifeServer.h>


int main(int argc, char *argv[])
{
#ifdef WEB_COMPILE
	Q_INIT_RESOURCE(prolifeqmlWeb);
#endif

	Q_INIT_RESOURCE(prolifestyle);
	Q_INIT_RESOURCE(prolifeqml);
	Q_INIT_RESOURCE(ImtCoreLoc);
	Q_INIT_RESOURCE(ProLifeLoc);

	Q_INIT_RESOURCE(imtstyle);

	Q_INIT_RESOURCE(imtgui);
	Q_INIT_RESOURCE(imtresthtml);

	Q_INIT_RESOURCE(imtauthguiTheme);
	Q_INIT_RESOURCE(imtguiTheme);
	Q_INIT_RESOURCE(imtlicguiTheme);

	CProLifeServer instance;

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != nullptr){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


