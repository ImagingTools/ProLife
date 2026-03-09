// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

// ImtCore includes
#include <imtbase/Init.h>

// ProLife includes
#include <GeneratedFiles/ProLifeServerConfigurator/CProLifeServerConfigurator.h>


int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(prolifeqml);
	return Run<CProLifeServerConfigurator, DefaultImtCoreQmlInitializer>(argc, argv);
}


