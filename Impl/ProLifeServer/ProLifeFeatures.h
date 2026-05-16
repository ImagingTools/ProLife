// Qt includes
#include <QtCore/qglobal.h>

// ImtCore includes
#include <imtlic/CProductInfo.h>
#include <imtlic/CFeatureInfo.h>


namespace prolife
{


static void FillProduct(imtlic::IProductInfo& productInfo){
	productInfo.SetProductId("ProLife");
	productInfo.SetName(QT_TRANSLATE_NOOP("Product", "ProLife"));
	productInfo.SetCategoryId("Software");

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> sensorManagementFeatureInfo;
	sensorManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	sensorManagementFeatureInfo->SetObjectUuid("06d570ef-170c-48fc-bf81-fab890b8d9da");
	sensorManagementFeatureInfo->SetFeatureId("SensorManagement");
	sensorManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Sensor Management"));
	sensorManagementFeatureInfo->SetOptional(false);
	sensorManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewSensorHistoryFeatureInfo;
	viewSensorHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewSensorHistoryFeatureInfo->SetFeatureId("ViewSensorHistory");
	viewSensorHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Sensor History"));
	viewSensorHistoryFeatureInfo->SetOptional(false);
	viewSensorHistoryFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(viewSensorHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> unbindSensorFeatureInfo;
	unbindSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	unbindSensorFeatureInfo->SetFeatureId("UnbindSensor");
	unbindSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Unbind Sensor"));
	unbindSensorFeatureInfo->SetOptional(false);
	unbindSensorFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(unbindSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> bindSensorFeatureInfo;
	bindSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	bindSensorFeatureInfo->SetFeatureId("BindSensor");
	bindSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Bind Sensor"));
	bindSensorFeatureInfo->SetOptional(false);
	bindSensorFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(bindSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> createLicenseFileFeatureInfo;
	createLicenseFileFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	createLicenseFileFeatureInfo->SetFeatureId("CreateLicenseFile");
	createLicenseFileFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Create License File"));
	createLicenseFileFeatureInfo->SetOptional(false);
	createLicenseFileFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(createLicenseFileFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAllSensorsFeatureInfo;
	viewAllSensorsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAllSensorsFeatureInfo->SetFeatureId("ViewAllSensors");
	viewAllSensorsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Sensors"));
	viewAllSensorsFeatureInfo->SetOptional(false);
	viewAllSensorsFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(viewAllSensorsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editSensorFeatureInfo;
	editSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editSensorFeatureInfo->SetFeatureId("EditSensor");
	editSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Sensor"));
	editSensorFeatureInfo->SetOptional(false);
	editSensorFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeSensorFeatureInfo;
	changeSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeSensorFeatureInfo->SetFeatureId("ChangeSensor");
	changeSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Sensor"));
	changeSensorFeatureInfo->SetOptional(false);
	changeSensorFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrderForSensorFeatureInfo;
	changeOrderForSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrderForSensorFeatureInfo->SetFeatureId("ChangeOrderForSensor");
	changeOrderForSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order"));
	changeOrderForSensorFeatureInfo->SetOptional(false);
	changeOrderForSensorFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeOrderForSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeProductionStatusFeatureInfo;
	changeProductionStatusFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeProductionStatusFeatureInfo->SetFeatureId("ChangeProductionStatus");
	changeProductionStatusFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Production Status"));
	changeProductionStatusFeatureInfo->SetOptional(false);
	changeProductionStatusFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeProductionStatusFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeHardwareConfigurationFeatureInfo;
	changeHardwareConfigurationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeHardwareConfigurationFeatureInfo->SetFeatureId("ChangeHardwareConfiguration");
	changeHardwareConfigurationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Hardware Configuration"));
	changeHardwareConfigurationFeatureInfo->SetOptional(false);
	changeHardwareConfigurationFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeHardwareConfigurationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeDeviceTypeFeatureInfo;
	changeDeviceTypeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeDeviceTypeFeatureInfo->SetFeatureId("ChangeDeviceType");
	changeDeviceTypeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Device Type"));
	changeDeviceTypeFeatureInfo->SetOptional(false);
	changeDeviceTypeFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeDeviceTypeFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeDescriptionForSensorFeatureInfo;
	changeDescriptionForSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeDescriptionForSensorFeatureInfo->SetFeatureId("ChangeDescriptionForSensor");
	changeDescriptionForSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Description"));
	changeDescriptionForSensorFeatureInfo->SetOptional(false);
	changeDescriptionForSensorFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeDescriptionForSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeSerialNumberForSensorFeatureInfo;
	changeSerialNumberForSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeSerialNumberForSensorFeatureInfo->SetFeatureId("ChangeSerialNumberForSensor");
	changeSerialNumberForSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Serial Number"));
	changeSerialNumberForSensorFeatureInfo->SetOptional(false);
	changeSerialNumberForSensorFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeSerialNumberForSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeMacAddressFeatureInfo;
	changeMacAddressFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeMacAddressFeatureInfo->SetFeatureId("ChangeMacAddress");
	changeMacAddressFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Mac Address"));
	changeMacAddressFeatureInfo->SetOptional(false);
	changeMacAddressFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeMacAddressFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeProjectForSensorFeatureInfo;
	changeProjectForSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeProjectForSensorFeatureInfo->SetFeatureId("ChangeProjectForSensor");
	changeProjectForSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Project"));
	changeProjectForSensorFeatureInfo->SetOptional(false);
	changeProjectForSensorFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeProjectForSensorFeatureInfo.PopPtr());

	editSensorFeatureInfo->InsertSubFeature(changeSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeSensorFeatureInfo;
	removeSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeSensorFeatureInfo->SetFeatureId("RemoveSensor");
	removeSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Sensor"));
	removeSensorFeatureInfo->SetOptional(false);
	removeSensorFeatureInfo->SetIsPermission(true);

	editSensorFeatureInfo->InsertSubFeature(removeSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addSensorFeatureInfo;
	addSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addSensorFeatureInfo->SetFeatureId("AddSensor");
	addSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Sensor"));
	addSensorFeatureInfo->SetOptional(false);
	addSensorFeatureInfo->SetIsPermission(true);

	editSensorFeatureInfo->InsertSubFeature(addSensorFeatureInfo.PopPtr());

	sensorManagementFeatureInfo->InsertSubFeature(editSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewSensorsFeatureInfo;
	viewSensorsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewSensorsFeatureInfo->SetFeatureId("ViewSensors");
	viewSensorsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Sensors"));
	viewSensorsFeatureInfo->SetOptional(false);
	viewSensorsFeatureInfo->SetIsPermission(true);
	viewSensorsFeatureInfo->SetDependencies(QByteArray("EditSensor;ChangeSensorDescription;ChangeSerialNumber;ChangeMacAddress;ChangeProductionStatus;RemoveSensor;AddSensor").split(';'));

	sensorManagementFeatureInfo->InsertSubFeature(viewSensorsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> transferLicensesFeatureInfo;
	transferLicensesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	transferLicensesFeatureInfo->SetFeatureId("TransferLicenses");
	transferLicensesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Transfer Licenses"));
	transferLicensesFeatureInfo->SetOptional(false);
	transferLicensesFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(transferLicensesFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> resetTransferCounterFeatureInfo;
	resetTransferCounterFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	resetTransferCounterFeatureInfo->SetFeatureId("ResetTransferCounter");
	resetTransferCounterFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Reset Transfer Counter"));
	resetTransferCounterFeatureInfo->SetOptional(false);
	resetTransferCounterFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(resetTransferCounterFeatureInfo.PopPtr());

	productInfo.AddFeature("06d570ef-170c-48fc-bf81-fab890b8d9da", *sensorManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> orderManagementFeatureInfo;
	orderManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	orderManagementFeatureInfo->SetObjectUuid("0b920f33-dad1-43c8-b885-a01860dac5dd");
	orderManagementFeatureInfo->SetFeatureId("OrderManagement");
	orderManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Order Management"));
	orderManagementFeatureInfo->SetOptional(false);
	orderManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrderHistoryFeatureInfo;
	viewOrderHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrderHistoryFeatureInfo->SetFeatureId("ViewOrderHistory");
	viewOrderHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Order History"));
	viewOrderHistoryFeatureInfo->SetOptional(false);
	viewOrderHistoryFeatureInfo->SetIsPermission(true);

	orderManagementFeatureInfo->InsertSubFeature(viewOrderHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAllOrdersFeatureInfo;
	viewAllOrdersFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAllOrdersFeatureInfo->SetFeatureId("ViewAllOrders");
	viewAllOrdersFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Orders"));
	viewAllOrdersFeatureInfo->SetOptional(false);
	viewAllOrdersFeatureInfo->SetIsPermission(true);

	orderManagementFeatureInfo->InsertSubFeature(viewAllOrdersFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrderFeatureInfo;
	editOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrderFeatureInfo->SetFeatureId("EditOrder");
	editOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Order"));
	editOrderFeatureInfo->SetOptional(false);
	editOrderFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrderFeatureInfo;
	changeOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrderFeatureInfo->SetFeatureId("ChangeOrder");
	changeOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order"));
	changeOrderFeatureInfo->SetOptional(false);
	changeOrderFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrderProductsFeatureInfo;
	changeOrderProductsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrderProductsFeatureInfo->SetFeatureId("ChangeOrderProducts");
	changeOrderProductsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order Products"));
	changeOrderProductsFeatureInfo->SetOptional(false);
	changeOrderProductsFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changeOrderProductsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrderStatusFeatureInfo;
	changeOrderStatusFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrderStatusFeatureInfo->SetFeatureId("ChangeOrderStatus");
	changeOrderStatusFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order Status"));
	changeOrderStatusFeatureInfo->SetOptional(false);
	changeOrderStatusFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changeOrderStatusFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeCustomerFeatureInfo;
	changeCustomerFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeCustomerFeatureInfo->SetFeatureId("ChangeCustomer");
	changeCustomerFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Customer"));
	changeCustomerFeatureInfo->SetOptional(false);
	changeCustomerFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changeCustomerFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeDescriptionForOrderFeatureInfo;
	changeDescriptionForOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeDescriptionForOrderFeatureInfo->SetFeatureId("ChangeDescriptionForOrder");
	changeDescriptionForOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Description"));
	changeDescriptionForOrderFeatureInfo->SetOptional(false);
	changeDescriptionForOrderFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changeDescriptionForOrderFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changePurchaseOrderIdFeatureInfo;
	changePurchaseOrderIdFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changePurchaseOrderIdFeatureInfo->SetFeatureId("ChangePurchaseOrderId");
	changePurchaseOrderIdFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Purchase Order-ID"));
	changePurchaseOrderIdFeatureInfo->SetOptional(false);
	changePurchaseOrderIdFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changePurchaseOrderIdFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeDeliveryIdFeatureInfo;
	changeDeliveryIdFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeDeliveryIdFeatureInfo->SetFeatureId("ChangeDeliveryId");
	changeDeliveryIdFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Delivery-ID"));
	changeDeliveryIdFeatureInfo->SetOptional(false);
	changeDeliveryIdFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changeDeliveryIdFeatureInfo.PopPtr());

	editOrderFeatureInfo->InsertSubFeature(changeOrderFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeOrderFeatureInfo;
	removeOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeOrderFeatureInfo->SetFeatureId("RemoveOrder");
	removeOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Order"));
	removeOrderFeatureInfo->SetOptional(false);
	removeOrderFeatureInfo->SetIsPermission(true);

	editOrderFeatureInfo->InsertSubFeature(removeOrderFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addOrderFeatureInfo;
	addOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addOrderFeatureInfo->SetFeatureId("AddOrder");
	addOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Order"));
	addOrderFeatureInfo->SetOptional(false);
	addOrderFeatureInfo->SetIsPermission(true);

	editOrderFeatureInfo->InsertSubFeature(addOrderFeatureInfo.PopPtr());

	orderManagementFeatureInfo->InsertSubFeature(editOrderFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrdersFeatureInfo;
	viewOrdersFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrdersFeatureInfo->SetFeatureId("ViewOrders");
	viewOrdersFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Orders"));
	viewOrdersFeatureInfo->SetOptional(false);
	viewOrdersFeatureInfo->SetIsPermission(true);
	viewOrdersFeatureInfo->SetDependencies(QByteArray("EditOrder;ChangeOrder;RemoveOrder;AddOrder").split(';'));

	orderManagementFeatureInfo->InsertSubFeature(viewOrdersFeatureInfo.PopPtr());

	productInfo.AddFeature("0b920f33-dad1-43c8-b885-a01860dac5dd", *orderManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> licenseManagementFeatureInfo;
	licenseManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	licenseManagementFeatureInfo->SetObjectUuid("9225718e-3c9c-4e00-83d8-b39152e32a66");
	licenseManagementFeatureInfo->SetFeatureId("LicenseManagement");
	licenseManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "License Management"));
	licenseManagementFeatureInfo->SetOptional(false);
	licenseManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewLicenseHistoryFeatureInfo;
	viewLicenseHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewLicenseHistoryFeatureInfo->SetFeatureId("ViewLicenseHistory");
	viewLicenseHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View License History"));
	viewLicenseHistoryFeatureInfo->SetOptional(false);
	viewLicenseHistoryFeatureInfo->SetIsPermission(true);

	licenseManagementFeatureInfo->InsertSubFeature(viewLicenseHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAllLicensesFeatureInfo;
	viewAllLicensesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAllLicensesFeatureInfo->SetFeatureId("ViewAllLicenses");
	viewAllLicensesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Licenses"));
	viewAllLicensesFeatureInfo->SetOptional(false);
	viewAllLicensesFeatureInfo->SetIsPermission(true);

	licenseManagementFeatureInfo->InsertSubFeature(viewAllLicensesFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editLicenseFeatureInfo;
	editLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editLicenseFeatureInfo->SetFeatureId("EditLicense");
	editLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit License"));
	editLicenseFeatureInfo->SetOptional(false);
	editLicenseFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeLicenseFeatureInfo;
	changeLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeLicenseFeatureInfo->SetFeatureId("ChangeLicense");
	changeLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change License"));
	changeLicenseFeatureInfo->SetOptional(false);
	changeLicenseFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeLicenseNumberFeatureInfo;
	changeLicenseNumberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeLicenseNumberFeatureInfo->SetFeatureId("ChangeLicenseNumber");
	changeLicenseNumberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change License Number"));
	changeLicenseNumberFeatureInfo->SetOptional(false);
	changeLicenseNumberFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeLicenseNumberFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeExpirationFeatureInfo;
	changeExpirationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeExpirationFeatureInfo->SetFeatureId("ChangeExpiration");
	changeExpirationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Expiration"));
	changeExpirationFeatureInfo->SetOptional(false);
	changeExpirationFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeExpirationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeProductLicensesFeatureInfo;
	changeProductLicensesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeProductLicensesFeatureInfo->SetFeatureId("ChangeProductLicenses");
	changeProductLicensesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Product Licenses"));
	changeProductLicensesFeatureInfo->SetOptional(false);
	changeProductLicensesFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeProductLicensesFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeProductForLicenseFeatureInfo;
	changeProductForLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeProductForLicenseFeatureInfo->SetFeatureId("ChangeProductForLicense");
	changeProductForLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Product"));
	changeProductForLicenseFeatureInfo->SetOptional(false);
	changeProductForLicenseFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeProductForLicenseFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrderForLicenseFeatureInfo;
	changeOrderForLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrderForLicenseFeatureInfo->SetFeatureId("ChangeOrderForLicense");
	changeOrderForLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order"));
	changeOrderForLicenseFeatureInfo->SetOptional(false);
	changeOrderForLicenseFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeOrderForLicenseFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeProjectForLicenseFeatureInfo;
	changeProjectForLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeProjectForLicenseFeatureInfo->SetFeatureId("ChangeProjectForLicense");
	changeProjectForLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Project"));
	changeProjectForLicenseFeatureInfo->SetOptional(false);
	changeProjectForLicenseFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeProjectForLicenseFeatureInfo.PopPtr());

	editLicenseFeatureInfo->InsertSubFeature(changeLicenseFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeLicenseFeatureInfo;
	removeLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeLicenseFeatureInfo->SetFeatureId("RemoveLicense");
	removeLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove License"));
	removeLicenseFeatureInfo->SetOptional(false);
	removeLicenseFeatureInfo->SetIsPermission(true);

	editLicenseFeatureInfo->InsertSubFeature(removeLicenseFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addLicenseFeatureInfo;
	addLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addLicenseFeatureInfo->SetFeatureId("AddLicense");
	addLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add License"));
	addLicenseFeatureInfo->SetOptional(false);
	addLicenseFeatureInfo->SetIsPermission(true);

	editLicenseFeatureInfo->InsertSubFeature(addLicenseFeatureInfo.PopPtr());

	licenseManagementFeatureInfo->InsertSubFeature(editLicenseFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewLicensesFeatureInfo;
	viewLicensesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewLicensesFeatureInfo->SetFeatureId("ViewLicenses");
	viewLicensesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Licenses"));
	viewLicensesFeatureInfo->SetOptional(false);
	viewLicensesFeatureInfo->SetIsPermission(true);
	viewLicensesFeatureInfo->SetDependencies(QByteArray("EditLicense;ChangeLicenseNumber;RemoveLicense;AddLicense").split(';'));

	licenseManagementFeatureInfo->InsertSubFeature(viewLicensesFeatureInfo.PopPtr());

	productInfo.AddFeature("9225718e-3c9c-4e00-83d8-b39152e32a66", *licenseManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> accountManagementFeatureInfo;
	accountManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	accountManagementFeatureInfo->SetObjectUuid("d9372bb1-6d29-4531-ba18-4cf7329d99c8");
	accountManagementFeatureInfo->SetFeatureId("AccountManagement");
	accountManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Account Management"));
	accountManagementFeatureInfo->SetOptional(false);
	accountManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewAccountHistoryFeatureInfo;
	viewAccountHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAccountHistoryFeatureInfo->SetFeatureId("ViewAccountHistory");
	viewAccountHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Account History"));
	viewAccountHistoryFeatureInfo->SetOptional(false);
	viewAccountHistoryFeatureInfo->SetIsPermission(true);

	accountManagementFeatureInfo->InsertSubFeature(viewAccountHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editAccountFeatureInfo;
	editAccountFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editAccountFeatureInfo->SetFeatureId("EditAccount");
	editAccountFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Account"));
	editAccountFeatureInfo->SetOptional(false);
	editAccountFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeAccountFeatureInfo;
	changeAccountFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeAccountFeatureInfo->SetFeatureId("ChangeAccount");
	changeAccountFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account"));
	changeAccountFeatureInfo->SetOptional(false);
	changeAccountFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeAccountGroupsFeatureInfo;
	changeAccountGroupsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeAccountGroupsFeatureInfo->SetFeatureId("ChangeAccountGroups");
	changeAccountGroupsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Groups"));
	changeAccountGroupsFeatureInfo->SetOptional(false);
	changeAccountGroupsFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeAccountGroupsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeCompanyAddressFeatureInfo;
	changeCompanyAddressFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeCompanyAddressFeatureInfo->SetFeatureId("ChangeCompanyAddress");
	changeCompanyAddressFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Company Address"));
	changeCompanyAddressFeatureInfo->SetOptional(false);
	changeCompanyAddressFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeCompanyAddressFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeAccountEmailFeatureInfo;
	changeAccountEmailFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeAccountEmailFeatureInfo->SetFeatureId("ChangeAccountEmail");
	changeAccountEmailFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Email"));
	changeAccountEmailFeatureInfo->SetOptional(false);
	changeAccountEmailFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeAccountEmailFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeAccountDescriptionFeatureInfo;
	changeAccountDescriptionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeAccountDescriptionFeatureInfo->SetFeatureId("ChangeAccountDescription");
	changeAccountDescriptionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Description"));
	changeAccountDescriptionFeatureInfo->SetOptional(false);
	changeAccountDescriptionFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeAccountDescriptionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeAccountNameFeatureInfo;
	changeAccountNameFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeAccountNameFeatureInfo->SetFeatureId("ChangeAccountName");
	changeAccountNameFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Name"));
	changeAccountNameFeatureInfo->SetOptional(false);
	changeAccountNameFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeAccountNameFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeCustomerIdFeatureInfo;
	changeCustomerIdFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeCustomerIdFeatureInfo->SetFeatureId("ChangeCustomerId");
	changeCustomerIdFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Customer-ID"));
	changeCustomerIdFeatureInfo->SetOptional(false);
	changeCustomerIdFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeCustomerIdFeatureInfo.PopPtr());

	editAccountFeatureInfo->InsertSubFeature(changeAccountFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeAccountFeatureInfo;
	removeAccountFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeAccountFeatureInfo->SetFeatureId("RemoveAccount");
	removeAccountFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Account"));
	removeAccountFeatureInfo->SetOptional(false);
	removeAccountFeatureInfo->SetIsPermission(true);

	editAccountFeatureInfo->InsertSubFeature(removeAccountFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addAccountFeatureInfo;
	addAccountFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addAccountFeatureInfo->SetFeatureId("AddAccount");
	addAccountFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Account"));
	addAccountFeatureInfo->SetOptional(false);
	addAccountFeatureInfo->SetIsPermission(true);

	editAccountFeatureInfo->InsertSubFeature(addAccountFeatureInfo.PopPtr());

	accountManagementFeatureInfo->InsertSubFeature(editAccountFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAccountsFeatureInfo;
	viewAccountsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAccountsFeatureInfo->SetFeatureId("ViewAccounts");
	viewAccountsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Accounts"));
	viewAccountsFeatureInfo->SetOptional(false);
	viewAccountsFeatureInfo->SetIsPermission(true);
	viewAccountsFeatureInfo->SetDependencies(QByteArray("EditAccount;ChangeAccount;RemoveAccount;AddAccount").split(';'));

	accountManagementFeatureInfo->InsertSubFeature(viewAccountsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAllAccountsFeatureInfo;
	viewAllAccountsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAllAccountsFeatureInfo->SetFeatureId("ViewAllAccounts");
	viewAllAccountsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Accounts"));
	viewAllAccountsFeatureInfo->SetOptional(false);
	viewAllAccountsFeatureInfo->SetIsPermission(true);
	viewAllAccountsFeatureInfo->SetDependencies(QByteArray("ViewAccounts").split(';'));

	accountManagementFeatureInfo->InsertSubFeature(viewAllAccountsFeatureInfo.PopPtr());

	productInfo.AddFeature("d9372bb1-6d29-4531-ba18-4cf7329d99c8", *accountManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> searchManagementFeatureInfo;
	searchManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	searchManagementFeatureInfo->SetObjectUuid("3d98c36e-882a-4a5a-9bff-f932c62b3757");
	searchManagementFeatureInfo->SetFeatureId("SearchManagement");
	searchManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Search management"));
	searchManagementFeatureInfo->SetOptional(false);
	searchManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewSearchFeatureInfo;
	viewSearchFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewSearchFeatureInfo->SetFeatureId("ViewSearch");
	viewSearchFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View search page"));
	viewSearchFeatureInfo->SetOptional(false);
	viewSearchFeatureInfo->SetIsPermission(true);

	searchManagementFeatureInfo->InsertSubFeature(viewSearchFeatureInfo.PopPtr());

	productInfo.AddFeature("3d98c36e-882a-4a5a-9bff-f932c62b3757", *searchManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> revisionManagementFeatureInfo;
	revisionManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	revisionManagementFeatureInfo->SetObjectUuid("c52f36c9-0dbe-49f0-84a7-f59116bd7225");
	revisionManagementFeatureInfo->SetFeatureId("RevisionManagement");
	revisionManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Revision Management"));
	revisionManagementFeatureInfo->SetOptional(false);
	revisionManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> deleteRevisionFeatureInfo;
	deleteRevisionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	deleteRevisionFeatureInfo->SetFeatureId("DeleteRevision");
	deleteRevisionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Delete Revision"));
	deleteRevisionFeatureInfo->SetOptional(false);
	deleteRevisionFeatureInfo->SetIsPermission(true);

	revisionManagementFeatureInfo->InsertSubFeature(deleteRevisionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> exportRevisionFeatureInfo;
	exportRevisionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	exportRevisionFeatureInfo->SetFeatureId("ExportRevision");
	exportRevisionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Export Revision"));
	exportRevisionFeatureInfo->SetOptional(false);
	exportRevisionFeatureInfo->SetIsPermission(true);

	revisionManagementFeatureInfo->InsertSubFeature(exportRevisionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewRevisionsFeatureInfo;
	viewRevisionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewRevisionsFeatureInfo->SetFeatureId("ViewRevisions");
	viewRevisionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Revisions"));
	viewRevisionsFeatureInfo->SetOptional(false);
	viewRevisionsFeatureInfo->SetIsPermission(true);

	revisionManagementFeatureInfo->InsertSubFeature(viewRevisionsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> backupRevisionFeatureInfo;
	backupRevisionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	backupRevisionFeatureInfo->SetFeatureId("BackupRevision");
	backupRevisionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Backup Revision"));
	backupRevisionFeatureInfo->SetOptional(false);
	backupRevisionFeatureInfo->SetIsPermission(true);

	revisionManagementFeatureInfo->InsertSubFeature(backupRevisionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> restoreRevisionFeatureInfo;
	restoreRevisionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	restoreRevisionFeatureInfo->SetFeatureId("RestoreRevision");
	restoreRevisionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Restore Revision"));
	restoreRevisionFeatureInfo->SetOptional(false);
	restoreRevisionFeatureInfo->SetIsPermission(true);

	revisionManagementFeatureInfo->InsertSubFeature(restoreRevisionFeatureInfo.PopPtr());

	productInfo.AddFeature("c52f36c9-0dbe-49f0-84a7-f59116bd7225", *revisionManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> administrationFeatureInfo;
	administrationFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	administrationFeatureInfo->SetObjectUuid("ce4a7f72-5303-4044-b25c-3a02020eebd3");
	administrationFeatureInfo->SetFeatureId("Administration");
	administrationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Administration"));
	administrationFeatureInfo->SetOptional(false);
	administrationFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> editAdministrationFeatureInfo;
	editAdministrationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editAdministrationFeatureInfo->SetFeatureId("EditAdministration");
	editAdministrationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Administration"));
	editAdministrationFeatureInfo->SetOptional(false);
	editAdministrationFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> editGroupFeatureInfo;
	editGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editGroupFeatureInfo->SetFeatureId("EditGroup");
	editGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Group"));
	editGroupFeatureInfo->SetOptional(false);
	editGroupFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeGroupFeatureInfo;
	changeGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeGroupFeatureInfo->SetFeatureId("ChangeGroup");
	changeGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Group"));
	changeGroupFeatureInfo->SetOptional(false);
	changeGroupFeatureInfo->SetIsPermission(true);

	editGroupFeatureInfo->InsertSubFeature(changeGroupFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeGroupFeatureInfo;
	removeGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeGroupFeatureInfo->SetFeatureId("RemoveGroup");
	removeGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Group"));
	removeGroupFeatureInfo->SetOptional(false);
	removeGroupFeatureInfo->SetIsPermission(true);

	editGroupFeatureInfo->InsertSubFeature(removeGroupFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addGroupFeatureInfo;
	addGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addGroupFeatureInfo->SetFeatureId("AddGroup");
	addGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Group"));
	addGroupFeatureInfo->SetOptional(false);
	addGroupFeatureInfo->SetIsPermission(true);

	editGroupFeatureInfo->InsertSubFeature(addGroupFeatureInfo.PopPtr());

	editAdministrationFeatureInfo->InsertSubFeature(editGroupFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editRoleFeatureInfo;
	editRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editRoleFeatureInfo->SetFeatureId("EditRole");
	editRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Role"));
	editRoleFeatureInfo->SetOptional(false);
	editRoleFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeRoleFeatureInfo;
	changeRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeRoleFeatureInfo->SetFeatureId("ChangeRole");
	changeRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Role"));
	changeRoleFeatureInfo->SetOptional(false);
	changeRoleFeatureInfo->SetIsPermission(true);

	editRoleFeatureInfo->InsertSubFeature(changeRoleFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeRoleFeatureInfo;
	removeRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeRoleFeatureInfo->SetFeatureId("RemoveRole");
	removeRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Role"));
	removeRoleFeatureInfo->SetOptional(false);
	removeRoleFeatureInfo->SetIsPermission(true);

	editRoleFeatureInfo->InsertSubFeature(removeRoleFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addRoleFeatureInfo;
	addRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addRoleFeatureInfo->SetFeatureId("AddRole");
	addRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Role"));
	addRoleFeatureInfo->SetOptional(false);
	addRoleFeatureInfo->SetIsPermission(true);

	editRoleFeatureInfo->InsertSubFeature(addRoleFeatureInfo.PopPtr());

	editAdministrationFeatureInfo->InsertSubFeature(editRoleFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editUserFeatureInfo;
	editUserFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editUserFeatureInfo->SetFeatureId("EditUser");
	editUserFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit User"));
	editUserFeatureInfo->SetOptional(false);
	editUserFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeUserFeatureInfo;
	changeUserFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeUserFeatureInfo->SetFeatureId("ChangeUser");
	changeUserFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change User"));
	changeUserFeatureInfo->SetOptional(false);
	changeUserFeatureInfo->SetIsPermission(true);

	editUserFeatureInfo->InsertSubFeature(changeUserFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeUserFeatureInfo;
	removeUserFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeUserFeatureInfo->SetFeatureId("RemoveUser");
	removeUserFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove User"));
	removeUserFeatureInfo->SetOptional(false);
	removeUserFeatureInfo->SetIsPermission(true);

	editUserFeatureInfo->InsertSubFeature(removeUserFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addUserFeatureInfo;
	addUserFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addUserFeatureInfo->SetFeatureId("AddUser");
	addUserFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add User"));
	addUserFeatureInfo->SetOptional(false);
	addUserFeatureInfo->SetIsPermission(true);

	editUserFeatureInfo->InsertSubFeature(addUserFeatureInfo.PopPtr());

	editAdministrationFeatureInfo->InsertSubFeature(editUserFeatureInfo.PopPtr());

	administrationFeatureInfo->InsertSubFeature(editAdministrationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAdministrationFeatureInfo;
	viewAdministrationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAdministrationFeatureInfo->SetFeatureId("ViewAdministration");
	viewAdministrationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Administration"));
	viewAdministrationFeatureInfo->SetOptional(false);
	viewAdministrationFeatureInfo->SetIsPermission(true);
	viewAdministrationFeatureInfo->SetDependencies(QByteArray("EditAdministration;EditGroup;ChangeGroup;RemoveGroup;AddGroup;EditRole;ChangeRole;RemoveRole;AddRole;EditUser;ChangeUser;RemoveUser;AddUser").split(';'));

	istd::TDelPtr<imtlic::CFeatureInfo> viewGroupHistoryFeatureInfo;
	viewGroupHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewGroupHistoryFeatureInfo->SetFeatureId("ViewGroupHistory");
	viewGroupHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Group History"));
	viewGroupHistoryFeatureInfo->SetOptional(false);
	viewGroupHistoryFeatureInfo->SetIsPermission(true);

	viewAdministrationFeatureInfo->InsertSubFeature(viewGroupHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewUserHistoryFeatureInfo;
	viewUserHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewUserHistoryFeatureInfo->SetFeatureId("ViewUserHistory");
	viewUserHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View User History"));
	viewUserHistoryFeatureInfo->SetOptional(false);
	viewUserHistoryFeatureInfo->SetIsPermission(true);

	viewAdministrationFeatureInfo->InsertSubFeature(viewUserHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewRoleHistoryFeatureInfo;
	viewRoleHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewRoleHistoryFeatureInfo->SetFeatureId("ViewRoleHistory");
	viewRoleHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Role History"));
	viewRoleHistoryFeatureInfo->SetOptional(false);
	viewRoleHistoryFeatureInfo->SetIsPermission(true);

	viewAdministrationFeatureInfo->InsertSubFeature(viewRoleHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewGroupsFeatureInfo;
	viewGroupsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewGroupsFeatureInfo->SetFeatureId("ViewGroups");
	viewGroupsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Groups"));
	viewGroupsFeatureInfo->SetOptional(false);
	viewGroupsFeatureInfo->SetIsPermission(true);
	viewGroupsFeatureInfo->SetDependencies(QByteArray("EditGroup;ChangeGroup;RemoveGroup;AddGroup").split(';'));

	viewAdministrationFeatureInfo->InsertSubFeature(viewGroupsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewUsersFeatureInfo;
	viewUsersFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewUsersFeatureInfo->SetFeatureId("ViewUsers");
	viewUsersFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Users"));
	viewUsersFeatureInfo->SetOptional(false);
	viewUsersFeatureInfo->SetIsPermission(true);
	viewUsersFeatureInfo->SetDependencies(QByteArray("EditUser;ChangeUser;RemoveUser;AddUser").split(';'));

	viewAdministrationFeatureInfo->InsertSubFeature(viewUsersFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewRolesFeatureInfo;
	viewRolesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewRolesFeatureInfo->SetFeatureId("ViewRoles");
	viewRolesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Roles"));
	viewRolesFeatureInfo->SetOptional(false);
	viewRolesFeatureInfo->SetIsPermission(true);
	viewRolesFeatureInfo->SetDependencies(QByteArray("EditRole;ChangeRole;RemoveRole;AddRole").split(';'));

	viewAdministrationFeatureInfo->InsertSubFeature(viewRolesFeatureInfo.PopPtr());

	administrationFeatureInfo->InsertSubFeature(viewAdministrationFeatureInfo.PopPtr());

	productInfo.AddFeature("ce4a7f72-5303-4044-b25c-3a02020eebd3", *administrationFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> workspaceManagementFeatureInfo;
	workspaceManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	workspaceManagementFeatureInfo->SetObjectUuid("df22ac46-7253-4b13-a1b8-d4391943adde");
	workspaceManagementFeatureInfo->SetFeatureId("WorkspaceManagement");
	workspaceManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Workspace Management"));
	workspaceManagementFeatureInfo->SetOptional(false);
	workspaceManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewWorkspaceFeatureInfo;
	viewWorkspaceFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewWorkspaceFeatureInfo->SetFeatureId("ViewWorkspace");
	viewWorkspaceFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Workspace"));
	viewWorkspaceFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "View workspace page"));
	viewWorkspaceFeatureInfo->SetOptional(false);
	viewWorkspaceFeatureInfo->SetIsPermission(true);

	workspaceManagementFeatureInfo->InsertSubFeature(viewWorkspaceFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewUserActionsFeatureInfo;
	viewUserActionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewUserActionsFeatureInfo->SetFeatureId("ViewUserActions");
	viewUserActionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View User Actions"));
	viewUserActionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "View only your activity"));
	viewUserActionsFeatureInfo->SetOptional(false);
	viewUserActionsFeatureInfo->SetIsPermission(true);

	workspaceManagementFeatureInfo->InsertSubFeature(viewUserActionsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAllUserActionsFeatureInfo;
	viewAllUserActionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAllUserActionsFeatureInfo->SetFeatureId("ViewAllUserActions");
	viewAllUserActionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All User Actions"));
	viewAllUserActionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "View the activity of all users"));
	viewAllUserActionsFeatureInfo->SetOptional(false);
	viewAllUserActionsFeatureInfo->SetIsPermission(true);

	workspaceManagementFeatureInfo->InsertSubFeature(viewAllUserActionsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAnalyticsFeatureInfo;
	viewAnalyticsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAnalyticsFeatureInfo->SetFeatureId("ViewAnalytics");
	viewAnalyticsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Analytics"));
	viewAnalyticsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "View analytics page"));
	viewAnalyticsFeatureInfo->SetOptional(false);
	viewAnalyticsFeatureInfo->SetIsPermission(true);

	workspaceManagementFeatureInfo->InsertSubFeature(viewAnalyticsFeatureInfo.PopPtr());

	productInfo.AddFeature("df22ac46-7253-4b13-a1b8-d4391943adde", *workspaceManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> organizationManagementFeatureInfo;
	organizationManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	organizationManagementFeatureInfo->SetObjectUuid("a1b2c3d4-e5f6-7890-abcd-ef1234567890");
	organizationManagementFeatureInfo->SetFeatureId("OrganizationManagement");
	organizationManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Organization Management"));
	organizationManagementFeatureInfo->SetOptional(false);
	organizationManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewTenantHistoryFeatureInfo;
	viewTenantHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewTenantHistoryFeatureInfo->SetFeatureId("ViewTenantHistory");
	viewTenantHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization History"));
	viewTenantHistoryFeatureInfo->SetOptional(false);
	viewTenantHistoryFeatureInfo->SetIsPermission(true);

	organizationManagementFeatureInfo->InsertSubFeature(viewTenantHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editTenantFeatureInfo;
	editTenantFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editTenantFeatureInfo->SetFeatureId("EditTenant");
	editTenantFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization"));
	editTenantFeatureInfo->SetOptional(false);
	editTenantFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeTenantFeatureInfo;
	changeTenantFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeTenantFeatureInfo->SetFeatureId("ChangeTenant");
	changeTenantFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization"));
	changeTenantFeatureInfo->SetOptional(false);
	changeTenantFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeTenantNameFeatureInfo;
	changeTenantNameFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeTenantNameFeatureInfo->SetFeatureId("ChangeTenantName");
	changeTenantNameFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Name"));
	changeTenantNameFeatureInfo->SetOptional(false);
	changeTenantNameFeatureInfo->SetIsPermission(true);

	changeTenantFeatureInfo->InsertSubFeature(changeTenantNameFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeTenantDescriptionFeatureInfo;
	changeTenantDescriptionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeTenantDescriptionFeatureInfo->SetFeatureId("ChangeTenantDescription");
	changeTenantDescriptionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Description"));
	changeTenantDescriptionFeatureInfo->SetOptional(false);
	changeTenantDescriptionFeatureInfo->SetIsPermission(true);

	changeTenantFeatureInfo->InsertSubFeature(changeTenantDescriptionFeatureInfo.PopPtr());

	editTenantFeatureInfo->InsertSubFeature(changeTenantFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeTenantFeatureInfo;
	removeTenantFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeTenantFeatureInfo->SetFeatureId("RemoveTenant");
	removeTenantFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Organization"));
	removeTenantFeatureInfo->SetOptional(false);
	removeTenantFeatureInfo->SetIsPermission(true);

	editTenantFeatureInfo->InsertSubFeature(removeTenantFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addTenantFeatureInfo;
	addTenantFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addTenantFeatureInfo->SetFeatureId("AddTenant");
	addTenantFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Organization"));
	addTenantFeatureInfo->SetOptional(false);
	addTenantFeatureInfo->SetIsPermission(true);

	editTenantFeatureInfo->InsertSubFeature(addTenantFeatureInfo.PopPtr());

	organizationManagementFeatureInfo->InsertSubFeature(editTenantFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewTenantsFeatureInfo;
	viewTenantsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewTenantsFeatureInfo->SetFeatureId("ViewTenants");
	viewTenantsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organizations"));
	viewTenantsFeatureInfo->SetOptional(false);
	viewTenantsFeatureInfo->SetIsPermission(true);
	viewTenantsFeatureInfo->SetDependencies(QByteArray("EditTenant;ChangeTenant;RemoveTenant;AddTenant").split(';'));

	organizationManagementFeatureInfo->InsertSubFeature(viewTenantsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAllTenantsFeatureInfo;
	viewAllTenantsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAllTenantsFeatureInfo->SetFeatureId("ViewAllTenants");
	viewAllTenantsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Organizations"));
	viewAllTenantsFeatureInfo->SetOptional(false);
	viewAllTenantsFeatureInfo->SetIsPermission(true);
	viewAllTenantsFeatureInfo->SetDependencies(QByteArray("ViewTenants").split(';'));

	organizationManagementFeatureInfo->InsertSubFeature(viewAllTenantsFeatureInfo.PopPtr());

	productInfo.AddFeature("a1b2c3d4-e5f6-7890-abcd-ef1234567890", *organizationManagementFeatureInfo.GetPtr());

}


};

