// ImtCore includes
#include <imtbase/Init.h>
#include <imtqml/CQmlProcess.h>

// ProLife includes
#include <GeneratedFiles/ProLifeServerConfigurator/CProLifeServerConfigurator.h>


int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(ProLifeServerConfigurator);
	Q_INIT_RESOURCE(prolifeqml);
	qmlRegisterType<imtqml::CQmlProcess>("imtqml", 1, 0, "Process");
	return Run<CProLifeServerConfigurator, DefaultImtCoreQmlInitializer>(argc, argv);
}


