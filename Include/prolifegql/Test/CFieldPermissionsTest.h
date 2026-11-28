#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of the checks of field permissions (prolifegql.h): an update of an order, a license or an account
	may change a field only with the permission of that field.
*/
class CFieldPermissionsTest: public QObject
{
	Q_OBJECT
private slots:
	void OrderUnchangedTest();
	void OrderFieldsTest_data();
	void OrderFieldsTest();
	void OrderLegacyStatusTest();
	void OrderLegacyCustomerTest();
	void OrderSecondCustomerRoleTest();
	void LicenseFieldsTest_data();
	void LicenseFieldsTest();
	void LicenseExpirationDayTest();
	void AccountFieldsTest_data();
	void AccountFieldsTest();
	void AccountEmptyAddressTest();
	void AdminTest();
	void UnknownUserTest();
	void HoldsPermissionTest();
};


