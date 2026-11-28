#include "CFieldPermissionsTest.h"


// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtaccount/CAddress.h>
#include <imtlic/CProductInstanceInfo.h>

// ProLife includes
#include <prolifegql/prolifegql.h>
#include <prolifedata/CCustomerInfo.h>
#include <prolifedata/COrderInfo.h>
#include <prolifedata/COrderCustomerRole.h>
#include <prolifedata/TOrderedWrap.h>


namespace
{


const QByteArray s_productId("ProLife");


typedef prolifedata::TOrderedWrap<imtlic::CProductInstanceInfo> COrderedLicense;


class CAdminUser: public imtauth::CUserInfo
{
public:
	virtual bool IsAdmin() const override
	{
		return true;
	}

	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override
	{
		istd::IChangeableUniquePtr clonePtr(new CAdminUser);
		if (clonePtr->CopyFrom(*this, mode)){
			return clonePtr;
		}

		return istd::IChangeableUniquePtr();
	}
};


imtgql::CGqlRequest CreateRequest(const QByteArrayList& permissions, bool isAdmin = false, bool hasUser = true)
{
	imtgql::CGqlRequest request(imtgql::IGqlRequest::RT_MUTATION, "Update");
	istd::TSharedInterfacePtr<imtgql::CGqlContext> contextPtr(new imtgql::CGqlContext);
	contextPtr->SetProductId(s_productId);
	if (hasUser){
		CAdminUser admin;
		imtauth::CUserInfo user;
		imtauth::CUserInfo& usedUser = isAdmin ? admin : user;
		usedUser.SetId("u1");
		usedUser.SetLocalPermissions(s_productId, permissions);
		contextPtr->SetUserId(usedUser.GetId());
		contextPtr->SetUserInfo(&usedUser);
	}
	request.SetGqlContext(contextPtr);

	return request;
}


void FillOrder(prolifedata::COrderInfo& order)
{
	order.SetOrderId("PO-1");
	order.SetPurchaseOrderId("P-1");
	order.SetDescription("d");
	order.SetCustomerId("c1");
	order.SetOrderStatus(prolifedata::IOrderInfo::OS_CREATED);
}


void FillLicense(COrderedLicense& license)
{
	license.SetSerialNumber("S-1");
	license.SetProject("P");
	license.SetupProductInstance("prod", "", "c1");
	license.SetOrderId("o1");
	license.AddLicense("lic", QDateTime(QDate(2030, 1, 2), QTime(0, 0)));
}


void FillAccount(prolifedata::CCustomerInfo& account)
{
	account.SetName("N");
	account.SetDescription("D");
	account.SetEmail("e@x.test");
	account.SetCustomerId("C-1");
	imtaccount::CAddress address;
	address.SetCity("City");
	address.SetPostalCode(123);
	account.AddAddress(address);
}



} // namespace


// private slots

void CFieldPermissionsTest::OrderUnchangedTest()
{
	prolifedata::COrderInfo oldOrder, newOrder;
	FillOrder(oldOrder);
	FillOrder(newOrder);
	QString error;
	QVERIFY2(prolifegql::CheckOrderChangePermissions(oldOrder, newOrder, false, CreateRequest({}), error), qPrintable(error));
}

void CFieldPermissionsTest::OrderFieldsTest_data()
{
	QTest::addColumn<int>("field");
	QTest::addColumn<QByteArray>("permission");
	QTest::addColumn<QString>("name");
	QTest::newRow("deliveryId") << 0 << QByteArray("/OrderManagement/Edit/Change/DeliveryId") << "Delivery-ID";
	QTest::newRow("purchase") << 1 << QByteArray("/OrderManagement/Edit/Change/PurchaseOrderId") << "Purchase Order-ID";
	QTest::newRow("description") << 2 << QByteArray("/OrderManagement/Edit/Change/Description") << "Description";
	QTest::newRow("customer") << 3 << QByteArray("/OrderManagement/Edit/Change/Customer") << "Customer";
	QTest::newRow("status") << 4 << QByteArray("/OrderManagement/Edit/Change/Status") << "Status";
	QTest::newRow("products") << 5 << QByteArray("/OrderManagement/Edit/Change/Products") << "Products";
}

void CFieldPermissionsTest::OrderFieldsTest()
{
	QFETCH(int, field);
	QFETCH(QByteArray, permission);
	QFETCH(QString, name);

	prolifedata::COrderInfo oldOrder, newOrder;
	FillOrder(oldOrder);
	FillOrder(newOrder);
	switch (field){
	case 0: newOrder.SetOrderId("PO-2"); break;
	case 1: newOrder.SetPurchaseOrderId("P-2"); break;
	case 2: newOrder.SetDescription("x"); break;
	case 3: newOrder.ResetData(); FillOrder(newOrder); newOrder.GetCustomerRoles()->ResetData(); newOrder.SetCustomerId("c2"); break;
	case 4: newOrder.SetOrderStatus(prolifedata::IOrderInfo::OS_CLOSED); break;
	}
	const bool products = field == 5;

	QString error;
	QVERIFY(!prolifegql::CheckOrderChangePermissions(oldOrder, newOrder, products, CreateRequest({"/OrderManagement/View"}), error));
	QCOMPARE(error, QStringLiteral("No permission to change: %1").arg(name));

	error.clear();
	QVERIFY2(prolifegql::CheckOrderChangePermissions(oldOrder, newOrder, products, CreateRequest({permission}), error), qPrintable(error));
}

void CFieldPermissionsTest::OrderLegacyStatusTest()
{
	prolifedata::COrderInfo oldOrder, newOrder;
	FillOrder(oldOrder);
	FillOrder(newOrder);
	oldOrder.SetOrderStatus(prolifedata::IOrderInfo::OS_ON_HOLD);
	QString error;
	QVERIFY2(prolifegql::CheckOrderChangePermissions(oldOrder, newOrder, false, CreateRequest({}), error), qPrintable(error));
}

void CFieldPermissionsTest::OrderLegacyCustomerTest()
{
	prolifedata::COrderInfo oldOrder, newOrder;
	FillOrder(oldOrder);
	oldOrder.GetCustomerRoles()->ResetData();
	FillOrder(newOrder);
	QCOMPARE(oldOrder.GetCustomerId(), QByteArray("c1"));
	QString error;
	QVERIFY2(prolifegql::CheckOrderChangePermissions(oldOrder, newOrder, false, CreateRequest({}), error), qPrintable(error));
}

void CFieldPermissionsTest::OrderSecondCustomerRoleTest()
{
	prolifedata::COrderInfo oldOrder, newOrder;
	FillOrder(oldOrder);
	FillOrder(newOrder);
	prolifedata::COrderCustomerRole role;
	role.SetCustomerId("c3");
	role.SetRoleType(prolifedata::IOrderCustomerRole::RT_END_CUSTOMER);
	newOrder.GetCustomerRoles()->InsertNewObject("OrderCustomerRole", "", "", &role);
	QString error;
	QVERIFY(!prolifegql::CheckOrderChangePermissions(oldOrder, newOrder, false, CreateRequest({}), error));
	QCOMPARE(error, QStringLiteral("No permission to change: Customer"));
}

void CFieldPermissionsTest::LicenseFieldsTest_data()
{
	QTest::addColumn<int>("field");
	QTest::addColumn<QByteArray>("permission");
	QTest::addColumn<QString>("name");
	QTest::newRow("number") << 0 << QByteArray("/LicenseManagement/Edit/Change/Number") << "Number";
	QTest::newRow("expiration") << 1 << QByteArray("/LicenseManagement/Edit/Change/Expiration") << "Expiration";
	QTest::newRow("licenses") << 2 << QByteArray("/LicenseManagement/Edit/Change/ProductLicenses") << "Product Licenses";
	QTest::newRow("count") << 3 << QByteArray("/LicenseManagement/Edit/Change/ProductLicenses") << "Product Licenses";
	QTest::newRow("product") << 4 << QByteArray("/LicenseManagement/Edit/Change/Product") << "Product";
	QTest::newRow("order") << 5 << QByteArray("/LicenseManagement/Edit/Change/Order") << "Order";
	QTest::newRow("project") << 6 << QByteArray("/LicenseManagement/Edit/Change/Project") << "Project";
	QTest::newRow("customer") << 7 << QByteArray("/LicenseManagement/Edit/Change/Order") << "Order";
}

void CFieldPermissionsTest::LicenseFieldsTest()
{
	QFETCH(int, field);
	QFETCH(QByteArray, permission);
	QFETCH(QString, name);

	COrderedLicense oldLicense, newLicense;
	FillLicense(oldLicense);
	FillLicense(newLicense);
	QString error;
	QVERIFY2(prolifegql::CheckSoftwareChangePermissions(oldLicense, newLicense, CreateRequest({}), error), qPrintable(error));

	switch (field){
	case 0: newLicense.SetSerialNumber("S-2"); break;
	case 1: newLicense.AddLicense("lic", QDateTime(QDate(2031, 1, 2), QTime(0, 0))); break;
	case 2: newLicense.AddLicense("lic2"); break;
	case 3: newLicense.SetProductCount(5); break;
	case 4: newLicense.SetupProductInstance("prod2", "", "c1"); break;
	case 5: newLicense.SetOrderId("o2"); break;
	case 6: newLicense.SetProject("Q"); break;
	case 7: newLicense.SetupProductInstance("prod", "", "c2"); break;
	}

	QVERIFY(!prolifegql::CheckSoftwareChangePermissions(oldLicense, newLicense, CreateRequest({"/LicenseManagement/View"}), error));
	QCOMPARE(error, QStringLiteral("No permission to change: %1").arg(name));
	QVERIFY2(prolifegql::CheckSoftwareChangePermissions(oldLicense, newLicense, CreateRequest({permission}), error), qPrintable(error));
}

void CFieldPermissionsTest::LicenseExpirationDayTest()
{
	COrderedLicense oldLicense, newLicense;
	FillLicense(oldLicense);
	FillLicense(newLicense);
	newLicense.AddLicense("lic", QDateTime(QDate(2030, 1, 2), QTime(13, 5)));
	QString error;
	QVERIFY2(prolifegql::CheckSoftwareChangePermissions(oldLicense, newLicense, CreateRequest({}), error), qPrintable(error));
}

void CFieldPermissionsTest::AccountFieldsTest_data()
{
	QTest::addColumn<int>("field");
	QTest::addColumn<QByteArray>("permission");
	QTest::addColumn<QString>("name");
	QTest::newRow("name") << 0 << QByteArray("/AccountManagement/Edit/Change/Name") << "Name";
	QTest::newRow("website") << 1 << QByteArray("/AccountManagement/Edit/Change/Name") << "Name";
	QTest::newRow("supplier") << 2 << QByteArray("/AccountManagement/Edit/Change/Name") << "Name";
	QTest::newRow("description") << 3 << QByteArray("/AccountManagement/Edit/Change/Description") << "Description";
	QTest::newRow("email") << 4 << QByteArray("/AccountManagement/Edit/Change/Email") << "Email";
	QTest::newRow("address") << 5 << QByteArray("/AccountManagement/Edit/Change/CompanyAddress") << "Company Address";
	QTest::newRow("customerId") << 6 << QByteArray("/AccountManagement/Edit/Change/CustomerId") << "Customer-ID";
	QTest::newRow("groups") << 7 << QByteArray("/AccountManagement/Edit/Change/Groups") << "Groups";
}

void CFieldPermissionsTest::AccountFieldsTest()
{
	QFETCH(int, field);
	QFETCH(QByteArray, permission);
	QFETCH(QString, name);

	prolifedata::CCustomerInfo oldAccount, newAccount;
	FillAccount(oldAccount);
	FillAccount(newAccount);
	QString error;
	QVERIFY2(prolifegql::CheckAccountChangePermissions(oldAccount, newAccount, CreateRequest({}), error), qPrintable(error));

	switch (field){
	case 0: newAccount.SetName("M"); break;
	case 1: newAccount.SetWebsite("https://x.test"); break;
	case 2: newAccount.SetSupplier(true); break;
	case 3: newAccount.SetDescription("E"); break;
	case 4: newAccount.SetEmail("f@x.test"); break;
	case 5: { newAccount.ResetData(); FillAccount(oldAccount); newAccount.SetName("N"); newAccount.SetDescription("D"); newAccount.SetEmail("e@x.test"); newAccount.SetCustomerId("C-1");
		imtaccount::CAddress address; address.SetCity("Other"); address.SetPostalCode(123); newAccount.AddAddress(address); } break;
	case 6: newAccount.SetCustomerId("C-2"); break;
	case 7: newAccount.AddGroup("g1"); break;
	}

	QVERIFY(!prolifegql::CheckAccountChangePermissions(oldAccount, newAccount, CreateRequest({"/AccountManagement/View"}), error));
	QCOMPARE(error, QStringLiteral("No permission to change: %1").arg(name));
	QVERIFY2(prolifegql::CheckAccountChangePermissions(oldAccount, newAccount, CreateRequest({permission}), error), qPrintable(error));
}

void CFieldPermissionsTest::AccountEmptyAddressTest()
{
	prolifedata::CCustomerInfo oldAccount, newAccount;
	oldAccount.SetName("N");
	newAccount.SetName("N");
	imtaccount::CAddress emptyAddress;
	newAccount.AddAddress(emptyAddress);
	QString error;
	QVERIFY2(prolifegql::CheckAccountChangePermissions(oldAccount, newAccount, CreateRequest({}), error), qPrintable(error));
}

void CFieldPermissionsTest::AdminTest()
{
	prolifedata::CCustomerInfo oldAccount, newAccount;
	FillAccount(oldAccount);
	newAccount.SetName("Other");
	QString error;
	QVERIFY(prolifegql::CheckAccountChangePermissions(oldAccount, newAccount, CreateRequest({}, true), error));
	QVERIFY(prolifegql::HoldsPermission(CreateRequest({}, true), "/SensorManagement/Bind"));
}

void CFieldPermissionsTest::UnknownUserTest()
{
	prolifedata::COrderInfo oldOrder, newOrder;
	QString error;
	QVERIFY(!prolifegql::CheckOrderChangePermissions(oldOrder, newOrder, false, CreateRequest({}, false, false), error));
	QVERIFY(error.contains("User is unknown"));
}

void CFieldPermissionsTest::HoldsPermissionTest()
{
	QVERIFY(prolifegql::HoldsPermission(CreateRequest({"/SensorManagement/Bind"}), "/SensorManagement/Bind"));
	QVERIFY(!prolifegql::HoldsPermission(CreateRequest({"/SensorManagement/Bind"}), "/SensorManagement/Unbind"));
	QVERIFY(!prolifegql::HoldsPermission(CreateRequest({}, false, false), "/SensorManagement/Bind"));
}

I_ADD_TEST(CFieldPermissionsTest);


