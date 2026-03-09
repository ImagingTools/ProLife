// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

// ACF includes
#include <icomp/export.h>

// ImtCore includes
#include <imtbase/PluginInterface.h>
#include <imtservice/TConnectionCollectionPluginComponentImpl.h>
#include <imtservice/TObjectCollectionPluginComponentImpl.h>

// Local includes
#include <GeneratedFiles/ProLifeSettingsPlugin/CProLifeSettingsPlugin.h>


IMT_REGISTER_PLUGIN(
			imtservice::IConnectionCollectionPlugin,
			imtservice::TConnectionCollectionPluginComponentImpl<CProLifeSettingsPlugin>,
			ServiceSettings,
			ProLifeServerSettings);


		IMT_REGISTER_PLUGIN(
			imtservice::IObjectCollectionPlugin,
			imtservice::TObjectCollectionPluginComponentImpl<CProLifeSettingsPlugin>,
			ServiceLog,
			ProLifeServerLog);

