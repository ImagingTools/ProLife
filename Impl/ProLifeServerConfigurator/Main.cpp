// ImtCore includes
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreAuthInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>

// ProLife includes
#include <GeneratedFiles/ProLifeServerConfigurator/CProLifeServerConfigurator.h>


class ProLifeServerConfiguratorInitializer
{
public:
	static void Init()
	{
		Q_INIT_RESOURCE(prolifeqml);

		ImtCoreInitLocalizationResources();
		ImtCoreInitBaseResources();
		ImtCoreInitStyleResources();
		ImtCoreInitAuthStyleResources();

		ImtCoreInitQmlApplicationCoreResources();
		ImtCoreInitQmlDocumentManagementResources();
		ImtCoreInitAuthQmlResources();

		InitializeImtCoreStyle();
	}
};


int main(int argc, char *argv[])
{
	ProLifeServerConfiguratorInitializer::Init();

	CProLifeServerConfigurator instance;
	return imtcore::CApplicationRunner::Run(argc, argv, instance);
}


