// ImtCore includes
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreLicInitializer.h>
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>
#include <imtcore/CImtCoreAuthInitializer.h>

// ProLife includes
#include <GeneratedFiles/ProLifeClient/CProLifeClient.h>


static void InitializeProLifeClientResources()
{
	Q_INIT_RESOURCE(prolifeqml);
	Q_INIT_RESOURCE(prolifestyle);
	Q_INIT_RESOURCE(prolifeAccountsSdl);
	Q_INIT_RESOURCE(prolifeSensorsSdl);
	Q_INIT_RESOURCE(prolifeLicensesSdl);
	Q_INIT_RESOURCE(prolifeOrdersSdl);
	Q_INIT_RESOURCE(prolifeWorkspaceSdl);
	Q_INIT_RESOURCE(prolifeDeviceCollectionDocumentServiceSdl);

	Q_INIT_RESOURCE(imtdeskguiqml);
	Q_INIT_RESOURCE(imtdeskImtDeskSdl);
	Q_INIT_RESOURCE(imtdeskTicketCollectionDocumentServiceSdl);

	Q_INIT_RESOURCE(imtauthguiqml);

	ImtCoreInitLocalizationResources();
	ImtCoreInitBaseResources();

	ImtCoreInitStyleResources();
	ImtCoreInitAuthStyleResources();
	ImtCoreInitLicStyleResources();

	ImtCoreInitQmlApplicationCoreResources();
	ImtCoreInitQmlDocumentManagementResources();
	ImtCoreInitLicQmlResources();

	InitializeImtCoreStyle();
}


int main(int argc, char *argv[])
{
	InitializeProLifeClientResources();

	CProLifeClient instance;
	return imtcore::CApplicationRunner::Run(argc, argv, instance);
}


