// ImtCore includes
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreAuthorizableServerInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreDeskInitializer.h>
#include <imtlic/IProductInfo.h>

// ProLife includes
#include <GeneratedFiles/ProLifeServerTest/CProLifeServerTest.h>
#include "ProLifeFeatures.h"


static void InitializeProLifeServerTestResources()
{
#ifdef WEB_COMPILE
	Q_INIT_RESOURCE(prolifeqmlWeb);
#endif

	Q_INIT_RESOURCE(prolifestyle);
	Q_INIT_RESOURCE(prolifeqml);
	Q_INIT_RESOURCE(imtlicguiTheme);
	Q_INIT_RESOURCE(ProLifeLoc);

	InitializeImtCoreAuthorizableServer();
	ImtCoreInitDeskSqlResources();

	// GetStyleData serves the themes from ':/Style'.
	ImtCoreInitStyleResources();
}


int main(int argc, char *argv[])
{
	InitializeProLifeServerTestResources();

	CProLifeServerTest instance;
	auto* productInfoPtr = instance.GetInterface<imtlic::IProductInfo>();
	if (productInfoPtr != nullptr) {
		prolife::FillProduct(*productInfoPtr);
	}

	return imtcore::CApplicationRunner::Run(argc, argv, instance);
}

