// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#pragma once


// ACF includes
#include <iser/CJsonMemWriteArchive.h>
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlHandlerTest.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtauth/CUserInfo.h>


class CGetOrdersForUserWithoutGroupsTest: public imtgql::CGqlHandlerTest
{
public:
	// reimplemented (imtgql::CGqlHandlerTest)
	virtual imtgql::CGqlRequest* CreateGqlRequest() const override;
	virtual imtbase::CTreeItemModel* CreateExpectedModel() const override;

	virtual void initTestCase() override
	{
		QString proLifeDir = qEnvironmentVariable("PROLIFEDIR");
		m_registryFile = proLifeDir + QString("/Partitura/ProLifeVoce.arp/ProLifeHandlersTest.acc");
		m_configFile = proLifeDir + "/Config/ProLife.awc";

		ipackage::CComponentAccessor* accessorPtr = GetComponentAccessor();
		if (accessorPtr != nullptr){
			imtdb::IDatabaseEngine* databaseEnginePtr = accessorPtr->GetComponentInterface<imtdb::IDatabaseEngine>();
			if (databaseEnginePtr != nullptr){
				databaseEnginePtr->ExecSqlQueryFromFile(proLifeDir + "/Include/prolifetest/TestData/GetOrdersForUserWithoutGroupsTestData.sql");
			}
		}
	}

	virtual void cleanupTestCase() override
	{
		QString proLifeDir = qEnvironmentVariable("PROLIFEDIR");

		ipackage::CComponentAccessor* accessorPtr = GetComponentAccessor();
		if (accessorPtr != nullptr){
			imtdb::IDatabaseEngine* databaseEnginePtr = accessorPtr->GetComponentInterface<imtdb::IDatabaseEngine>();
			if (databaseEnginePtr != nullptr){
				databaseEnginePtr->ExecSqlQueryFromFile(proLifeDir + "/Include/prolifetest/TestData/ClearAllData.sql");
			}
		}
	}
};


