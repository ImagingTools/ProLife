// ImtCore includes
#include <imtbase/Init.h>

// ProLife includes
#include <GeneratedFiles/ProLifeClient/CProLifeClient.h>


int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(imtlicguiqml);
	Q_INIT_RESOURCE(imtlicguiTheme);

	Q_INIT_RESOURCE(prolifeqml);
	Q_INIT_RESOURCE(prolifestyle);
	Q_INIT_RESOURCE(prolifeAccountsSdl);
	Q_INIT_RESOURCE(prolifeSensorsSdl);
	Q_INIT_RESOURCE(prolifeLicensesSdl);
	Q_INIT_RESOURCE(prolifeOrdersSdl);
	Q_INIT_RESOURCE(prolifeWorkspaceSdl);

	Q_INIT_RESOURCE(imtlicFeaturesSdl);
	Q_INIT_RESOURCE(imtlicProductsSdl);
	Q_INIT_RESOURCE(imtlicLicensesSdl);

	return Run<CProLifeClient, DefaultImtCoreQmlInitializer>(argc, argv);
}


