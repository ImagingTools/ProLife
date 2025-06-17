// ImtCore includes
#include <imtlic/Init.h>

// ProLife includes
#include <GeneratedFiles/ProLifeServer/CProLifeServer.h>
#include "ProLifeFeatures.h"


int main(int argc, char *argv[])
{
#ifdef WEB_COMPILE
	Q_INIT_RESOURCE(prolifeqmlWeb);
#endif

	Q_INIT_RESOURCE(prolifestyle);
	Q_INIT_RESOURCE(prolifeqml);
	Q_INIT_RESOURCE(imtlicguiTheme);
	Q_INIT_RESOURCE(ProLifeLoc);

	CProLifeServer instance;

	imtlic::IProductInfo* productInfoPtr = instance.GetInterface<imtlic::IProductInfo>();
	if (productInfoPtr != nullptr){
		prolife::FillProduct(*productInfoPtr);
	}

	return ProductFeatureRun<CProLifeServer, DefaultImtCoreQmlInitializer, prolife::FillProduct>(argc, argv);
}


