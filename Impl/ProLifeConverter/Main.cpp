// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

// ACF includes
#include <ibase/IApplication.h>

#include <GeneratedFiles/ProLifeConverter/CProLifeConverter.h>


int main(int argc, char *argv[])
{
    CProLifeConverter instance;

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != nullptr){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


