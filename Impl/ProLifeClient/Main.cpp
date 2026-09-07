// ImtCore includes
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreAuthInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreDeskInitializer.h>
#include <imtcore/CImtCoreLicInitializer.h>
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>

// ProLife includes
#include <GeneratedFiles/ProLifeClient/CProLifeClient.h>


static void InitializeProLifeClientResources()
{
	Q_INIT_RESOURCE(ProLifeLoc);

	Q_INIT_RESOURCE(prolifeqml);
	Q_INIT_RESOURCE(prolifestyle);
	Q_INIT_RESOURCE(prolifeAccountsSdl);
	Q_INIT_RESOURCE(prolifeSensorsSdl);
	Q_INIT_RESOURCE(prolifeLicensesSdl);
	Q_INIT_RESOURCE(prolifeOrdersSdl);
	Q_INIT_RESOURCE(prolifeWorkspaceSdl);
	Q_INIT_RESOURCE(prolifeDeviceCollectionDocumentServiceSdl);

	ImtCoreInitLocalizationResources();
	ImtCoreInitBaseResources();

	ImtCoreInitStyleResources();
	ImtCoreInitAuthStyleResources();
	ImtCoreInitLicStyleResources();

	ImtCoreInitQmlApplicationCoreResources();
	ImtCoreInitQmlDocumentManagementResources();
	ImtCoreInitAuthQmlResources();
	ImtCoreInitLicQmlResources();
	ImtCoreInitDeskQmlResources();

	InitializeImtCoreStyle();
}


int main(int argc, char *argv[])
{
	InitializeProLifeClientResources();

	CProLifeClient instance;
	return imtcore::CApplicationRunner::Run(argc, argv, instance);
}
