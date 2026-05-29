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

	Q_INIT_RESOURCE(prolifeDeviceCollectionDocumentServiceSdl);

	Q_INIT_RESOURCE(imtdeskguiqml);
	Q_INIT_RESOURCE(imtdeskImtDeskSdl);
	Q_INIT_RESOURCE(imtdeskTicketCollectionDocumentServiceSdl);

	Q_INIT_RESOURCE(imtchatguiqml);
	Q_INIT_RESOURCE(imtchatImtChatSdl);
	Q_INIT_RESOURCE(imtchatConversationCollectionDocumentServiceSdl);

	return Run<CProLifeClient, DefaultImtCoreQmlInitializer>(argc, argv);
}


