// ACF includes
#include <icomp/export.h>

// ImtCore includes
#include <imtbase/PluginInterface.h>
#include <imtservice/TConnectionCollectionPluginComponentImpl.h>

// Local includes
#include <GeneratedFiles/ProLifeSettingsPlugin/CProLifeSettingsPlugin.h>


IMT_REGISTER_PLUGIN(
			imtservice::IConnectionCollectionPlugin,
			imtservice::TConnectionCollectionPluginComponentImpl<CProLifeSettingsPlugin>,
			ServiceSettings,
			ProLifeSettings);


