// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#include "CGetOrdersForUserWithManyGroupTest.h"


// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtgql/CGqlContext.h>


// public methods

// reimplemented (imtgql::CGqlHandlerTest)

imtbase::CTreeItemModel* CGetOrdersForUserWithManyGroupTest::CreateExpectedModel() const
{
	istd::TDelPtr<imtbase::CTreeItemModel> expectedModelPtr;
	expectedModelPtr.SetPtr(new imtbase::CTreeItemModel);

	imtbase::CTreeItemModel* dataModelPtr = expectedModelPtr->AddTreeModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	imtbase::CTreeItemModel* itemsModelPtr = dataModelPtr->AddTreeModel("items");
	Q_ASSERT(itemsModelPtr != nullptr);

	int index = itemsModelPtr->InsertNewItem();

	itemsModelPtr->SetData("Id", QString("Order1"), index);
	itemsModelPtr->SetData("Name", QString("Order1"), index);

	index = itemsModelPtr->InsertNewItem();

	itemsModelPtr->SetData("Id", QString("Order2"), index);
	itemsModelPtr->SetData("Name", QString("Order2"), index);

	index = itemsModelPtr->InsertNewItem();

	itemsModelPtr->SetData("Id", QString("Order3"), index);
	itemsModelPtr->SetData("Name", QString("Order3"), index);

	index = itemsModelPtr->InsertNewItem();

	itemsModelPtr->SetData("Id", QString("Order4"), index);
	itemsModelPtr->SetData("Name", QString("Order4"), index);

	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("notification");
	Q_ASSERT(notificationModelPtr != nullptr);

	notificationModelPtr->SetData("PagesCount", 1);
	notificationModelPtr->SetData("TotalCount", 4);

	return expectedModelPtr.PopPtr();
}


imtgql::CGqlRequest* CGetOrdersForUserWithManyGroupTest::CreateGqlRequest() const
{
	istd::TDelPtr<imtgql::CGqlRequest> gqlRequestPtr;
	gqlRequestPtr.SetPtr(new imtgql::CGqlRequest);

	gqlRequestPtr->SetRequestType(imtgql::CGqlRequest::RT_QUERY);
	gqlRequestPtr->SetCommandId("OrdersList");

	istd::TDelPtr<imtgql::CGqlObject> inputObjectPtr;
	inputObjectPtr.SetPtr(new imtgql::CGqlObject("input"));

	imtgql::CGqlObject* viewParamObjectPtr = inputObjectPtr->CreateFieldObject("viewParams");
	viewParamObjectPtr->InsertField("Count", -1);
	viewParamObjectPtr->InsertField("Offset", 0);

	gqlRequestPtr->AddParam(*inputObjectPtr.PopPtr());

	istd::TDelPtr<imtgql::CGqlObject> fieldObjectPtr;
	fieldObjectPtr.SetPtr(new imtgql::CGqlObject("items"));
	fieldObjectPtr->InsertField("Id");
	fieldObjectPtr->InsertField("Name");
	gqlRequestPtr->AddField(*fieldObjectPtr.PopPtr());

	istd::TDelPtr<imtgql::CGqlContext> gqlContextPtr;
	gqlContextPtr.SetPtr(new imtgql::CGqlContext);


	istd::TDelPtr<imtauth::CUserInfo> userInfoPtr;
	userInfoPtr.SetPtr(new imtauth::CUserInfo);

	userInfoPtr->SetId("Roman");
	userInfoPtr->AddToGroup("Group1");
	userInfoPtr->AddToGroup("Group2");

	gqlContextPtr->SetUserInfo(userInfoPtr.PopPtr());

	gqlRequestPtr->SetGqlContext(gqlContextPtr.PopPtr());

	return gqlRequestPtr.PopPtr();
}


I_ADD_TEST(CGetOrdersForUserWithManyGroupTest);


