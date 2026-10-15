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
	sensorManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Sensor-related operations."));
	sensorManagementFeatureInfo->SetOptional(false);
	sensorManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewSensorHistoryFeatureInfo;
	viewSensorHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewSensorHistoryFeatureInfo->SetFeatureId("ViewSensorHistory");
	viewSensorHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Sensor History"));
	viewSensorHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Sensor History."));
	viewSensorHistoryFeatureInfo->SetOptional(false);
	viewSensorHistoryFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(viewSensorHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> unbindSensorFeatureInfo;
	unbindSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	unbindSensorFeatureInfo->SetFeatureId("UnbindSensor");
	unbindSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Unbind Sensor"));
	unbindSensorFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows unbinding Sensor."));
	unbindSensorFeatureInfo->SetOptional(false);
	unbindSensorFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(unbindSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> bindSensorFeatureInfo;
	bindSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	bindSensorFeatureInfo->SetFeatureId("BindSensor");
	bindSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Bind Sensor"));
	bindSensorFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows binding Sensor."));
	bindSensorFeatureInfo->SetOptional(false);
	bindSensorFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(bindSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> createLicenseFileFeatureInfo;
	createLicenseFileFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	createLicenseFileFeatureInfo->SetFeatureId("CreateLicenseFile");
	createLicenseFileFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Create License File"));
	createLicenseFileFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows creating License File."));
	createLicenseFileFeatureInfo->SetOptional(false);
	createLicenseFileFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(createLicenseFileFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAllSensorsFeatureInfo;
	viewAllSensorsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAllSensorsFeatureInfo->SetFeatureId("ViewAllSensors");
	viewAllSensorsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Sensors"));
	viewAllSensorsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing all Sensors."));
	viewAllSensorsFeatureInfo->SetOptional(false);
	viewAllSensorsFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(viewAllSensorsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editSensorFeatureInfo;
	editSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editSensorFeatureInfo->SetFeatureId("EditSensor");
	editSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Sensor"));
	editSensorFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Sensor."));
	editSensorFeatureInfo->SetOptional(false);
	editSensorFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeSensorFeatureInfo;
	changeSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeSensorFeatureInfo->SetFeatureId("ChangeSensor");
	changeSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Sensor"));
	changeSensorFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Sensor."));
	changeSensorFeatureInfo->SetOptional(false);
	changeSensorFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrderForSensorFeatureInfo;
	changeOrderForSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrderForSensorFeatureInfo->SetFeatureId("ChangeOrderForSensor");
	changeOrderForSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order"));
	changeOrderForSensorFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Order."));
	changeOrderForSensorFeatureInfo->SetOptional(false);
	changeOrderForSensorFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeOrderForSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeProductionStatusFeatureInfo;
	changeProductionStatusFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeProductionStatusFeatureInfo->SetFeatureId("ChangeProductionStatus");
	changeProductionStatusFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Production Status"));
	changeProductionStatusFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Production Status."));
	changeProductionStatusFeatureInfo->SetOptional(false);
	changeProductionStatusFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeProductionStatusFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeHardwareConfigurationFeatureInfo;
	changeHardwareConfigurationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeHardwareConfigurationFeatureInfo->SetFeatureId("ChangeHardwareConfiguration");
	changeHardwareConfigurationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Hardware Configuration"));
	changeHardwareConfigurationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Hardware Configuration."));
	changeHardwareConfigurationFeatureInfo->SetOptional(false);
	changeHardwareConfigurationFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeHardwareConfigurationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeDeviceTypeFeatureInfo;
	changeDeviceTypeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeDeviceTypeFeatureInfo->SetFeatureId("ChangeDeviceType");
	changeDeviceTypeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Device Type"));
	changeDeviceTypeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Device Type."));
	changeDeviceTypeFeatureInfo->SetOptional(false);
	changeDeviceTypeFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeDeviceTypeFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeDescriptionForSensorFeatureInfo;
	changeDescriptionForSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeDescriptionForSensorFeatureInfo->SetFeatureId("ChangeDescriptionForSensor");
	changeDescriptionForSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Description"));
	changeDescriptionForSensorFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Description."));
	changeDescriptionForSensorFeatureInfo->SetOptional(false);
	changeDescriptionForSensorFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeDescriptionForSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeSerialNumberForSensorFeatureInfo;
	changeSerialNumberForSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeSerialNumberForSensorFeatureInfo->SetFeatureId("ChangeSerialNumberForSensor");
	changeSerialNumberForSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Serial Number"));
	changeSerialNumberForSensorFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Serial Number."));
	changeSerialNumberForSensorFeatureInfo->SetOptional(false);
	changeSerialNumberForSensorFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeSerialNumberForSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeMacAddressFeatureInfo;
	changeMacAddressFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeMacAddressFeatureInfo->SetFeatureId("ChangeMacAddress");
	changeMacAddressFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Mac Address"));
	changeMacAddressFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Mac Address."));
	changeMacAddressFeatureInfo->SetOptional(false);
	changeMacAddressFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeMacAddressFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeProjectForSensorFeatureInfo;
	changeProjectForSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeProjectForSensorFeatureInfo->SetFeatureId("ChangeProjectForSensor");
	changeProjectForSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Project"));
	changeProjectForSensorFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Project."));
	changeProjectForSensorFeatureInfo->SetOptional(false);
	changeProjectForSensorFeatureInfo->SetIsPermission(true);

	changeSensorFeatureInfo->InsertSubFeature(changeProjectForSensorFeatureInfo.PopPtr());

	editSensorFeatureInfo->InsertSubFeature(changeSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeSensorFeatureInfo;
	removeSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeSensorFeatureInfo->SetFeatureId("RemoveSensor");
	removeSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Sensor"));
	removeSensorFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Sensor."));
	removeSensorFeatureInfo->SetOptional(false);
	removeSensorFeatureInfo->SetIsPermission(true);

	editSensorFeatureInfo->InsertSubFeature(removeSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addSensorFeatureInfo;
	addSensorFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addSensorFeatureInfo->SetFeatureId("AddSensor");
	addSensorFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Sensor"));
	addSensorFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Sensor."));
	addSensorFeatureInfo->SetOptional(false);
	addSensorFeatureInfo->SetIsPermission(true);

	editSensorFeatureInfo->InsertSubFeature(addSensorFeatureInfo.PopPtr());

	sensorManagementFeatureInfo->InsertSubFeature(editSensorFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewSensorsFeatureInfo;
	viewSensorsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewSensorsFeatureInfo->SetFeatureId("ViewSensors");
	viewSensorsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Sensors"));
	viewSensorsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Sensors."));
	viewSensorsFeatureInfo->SetOptional(false);
	viewSensorsFeatureInfo->SetIsPermission(true);
	viewSensorsFeatureInfo->SetRequirements(QByteArray("/SensorManagement/EditSensor;/SensorManagement/EditSensor/ChangeSensor;/SensorManagement/EditSensor/RemoveSensor;/SensorManagement/EditSensor/AddSensor").split(';'));

	sensorManagementFeatureInfo->InsertSubFeature(viewSensorsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> transferLicensesFeatureInfo;
	transferLicensesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	transferLicensesFeatureInfo->SetFeatureId("TransferLicenses");
	transferLicensesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Transfer Licenses"));
	transferLicensesFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows transferring Licenses."));
	transferLicensesFeatureInfo->SetOptional(false);
	transferLicensesFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(transferLicensesFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> resetTransferCounterFeatureInfo;
	resetTransferCounterFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	resetTransferCounterFeatureInfo->SetFeatureId("ResetTransferCounter");
	resetTransferCounterFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Reset Transfer Counter"));
	resetTransferCounterFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows resetting Transfer Counter."));
	resetTransferCounterFeatureInfo->SetOptional(false);
	resetTransferCounterFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(resetTransferCounterFeatureInfo.PopPtr());

	productInfo.AddFeature("06d570ef-170c-48fc-bf81-fab890b8d9da", *sensorManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> orderManagementFeatureInfo;
	orderManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	orderManagementFeatureInfo->SetObjectUuid("0b920f33-dad1-43c8-b885-a01860dac5dd");
	orderManagementFeatureInfo->SetFeatureId("OrderManagement");
	orderManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Order Management"));
	orderManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Order-related operations."));
	orderManagementFeatureInfo->SetOptional(false);
	orderManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrderHistoryFeatureInfo;
	viewOrderHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrderHistoryFeatureInfo->SetFeatureId("ViewOrderHistory");
	viewOrderHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Order History"));
	viewOrderHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Order History."));
	viewOrderHistoryFeatureInfo->SetOptional(false);
	viewOrderHistoryFeatureInfo->SetIsPermission(true);

	orderManagementFeatureInfo->InsertSubFeature(viewOrderHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAllOrdersFeatureInfo;
	viewAllOrdersFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAllOrdersFeatureInfo->SetFeatureId("ViewAllOrders");
	viewAllOrdersFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Orders"));
	viewAllOrdersFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing all Orders."));
	viewAllOrdersFeatureInfo->SetOptional(false);
	viewAllOrdersFeatureInfo->SetIsPermission(true);

	orderManagementFeatureInfo->InsertSubFeature(viewAllOrdersFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrderFeatureInfo;
	editOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrderFeatureInfo->SetFeatureId("EditOrder");
	editOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Order"));
	editOrderFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Order."));
	editOrderFeatureInfo->SetOptional(false);
	editOrderFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrderFeatureInfo;
	changeOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrderFeatureInfo->SetFeatureId("ChangeOrder");
	changeOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order"));
	changeOrderFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Order."));
	changeOrderFeatureInfo->SetOptional(false);
	changeOrderFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrderProductsFeatureInfo;
	changeOrderProductsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrderProductsFeatureInfo->SetFeatureId("ChangeOrderProducts");
	changeOrderProductsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order Products"));
	changeOrderProductsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Order Products."));
	changeOrderProductsFeatureInfo->SetOptional(false);
	changeOrderProductsFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changeOrderProductsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrderStatusFeatureInfo;
	changeOrderStatusFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrderStatusFeatureInfo->SetFeatureId("ChangeOrderStatus");
	changeOrderStatusFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order Status"));
	changeOrderStatusFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Order Status."));
	changeOrderStatusFeatureInfo->SetOptional(false);
	changeOrderStatusFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changeOrderStatusFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeCustomerFeatureInfo;
	changeCustomerFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeCustomerFeatureInfo->SetFeatureId("ChangeCustomer");
	changeCustomerFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Customer"));
	changeCustomerFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Customer."));
	changeCustomerFeatureInfo->SetOptional(false);
	changeCustomerFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changeCustomerFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeDescriptionForOrderFeatureInfo;
	changeDescriptionForOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeDescriptionForOrderFeatureInfo->SetFeatureId("ChangeDescriptionForOrder");
	changeDescriptionForOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Description"));
	changeDescriptionForOrderFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Description."));
	changeDescriptionForOrderFeatureInfo->SetOptional(false);
	changeDescriptionForOrderFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changeDescriptionForOrderFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changePurchaseOrderIdFeatureInfo;
	changePurchaseOrderIdFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changePurchaseOrderIdFeatureInfo->SetFeatureId("ChangePurchaseOrderId");
	changePurchaseOrderIdFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Purchase Order-ID"));
	changePurchaseOrderIdFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Purchase Order-ID."));
	changePurchaseOrderIdFeatureInfo->SetOptional(false);
	changePurchaseOrderIdFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changePurchaseOrderIdFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeDeliveryIdFeatureInfo;
	changeDeliveryIdFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeDeliveryIdFeatureInfo->SetFeatureId("ChangeDeliveryId");
	changeDeliveryIdFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Delivery-ID"));
	changeDeliveryIdFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Delivery-ID."));
	changeDeliveryIdFeatureInfo->SetOptional(false);
	changeDeliveryIdFeatureInfo->SetIsPermission(true);

	changeOrderFeatureInfo->InsertSubFeature(changeDeliveryIdFeatureInfo.PopPtr());

	editOrderFeatureInfo->InsertSubFeature(changeOrderFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeOrderFeatureInfo;
	removeOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeOrderFeatureInfo->SetFeatureId("RemoveOrder");
	removeOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Order"));
	removeOrderFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Order."));
	removeOrderFeatureInfo->SetOptional(false);
	removeOrderFeatureInfo->SetIsPermission(true);

	editOrderFeatureInfo->InsertSubFeature(removeOrderFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addOrderFeatureInfo;
	addOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addOrderFeatureInfo->SetFeatureId("AddOrder");
	addOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Order"));
	addOrderFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Order."));
	addOrderFeatureInfo->SetOptional(false);
	addOrderFeatureInfo->SetIsPermission(true);

	editOrderFeatureInfo->InsertSubFeature(addOrderFeatureInfo.PopPtr());

	orderManagementFeatureInfo->InsertSubFeature(editOrderFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrdersFeatureInfo;
	viewOrdersFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrdersFeatureInfo->SetFeatureId("ViewOrders");
	viewOrdersFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Orders"));
	viewOrdersFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Orders."));
	viewOrdersFeatureInfo->SetOptional(false);
	viewOrdersFeatureInfo->SetIsPermission(true);
	viewOrdersFeatureInfo->SetRequirements(QByteArray("/OrderManagement/EditOrder;/OrderManagement/EditOrder/ChangeOrder;/OrderManagement/EditOrder/RemoveOrder;/OrderManagement/EditOrder/AddOrder").split(';'));

	orderManagementFeatureInfo->InsertSubFeature(viewOrdersFeatureInfo.PopPtr());

	productInfo.AddFeature("0b920f33-dad1-43c8-b885-a01860dac5dd", *orderManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> licenseManagementFeatureInfo;
	licenseManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	licenseManagementFeatureInfo->SetObjectUuid("9225718e-3c9c-4e00-83d8-b39152e32a66");
	licenseManagementFeatureInfo->SetFeatureId("LicenseManagement");
	licenseManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "License Management"));
	licenseManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to License-related operations."));
	licenseManagementFeatureInfo->SetOptional(false);
	licenseManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewLicenseHistoryFeatureInfo;
	viewLicenseHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewLicenseHistoryFeatureInfo->SetFeatureId("ViewLicenseHistory");
	viewLicenseHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View License History"));
	viewLicenseHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing License History."));
	viewLicenseHistoryFeatureInfo->SetOptional(false);
	viewLicenseHistoryFeatureInfo->SetIsPermission(true);

	licenseManagementFeatureInfo->InsertSubFeature(viewLicenseHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAllLicensesFeatureInfo;
	viewAllLicensesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAllLicensesFeatureInfo->SetFeatureId("ViewAllLicenses");
	viewAllLicensesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Licenses"));
	viewAllLicensesFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing all Licenses."));
	viewAllLicensesFeatureInfo->SetOptional(false);
	viewAllLicensesFeatureInfo->SetIsPermission(true);

	licenseManagementFeatureInfo->InsertSubFeature(viewAllLicensesFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editLicenseFeatureInfo;
	editLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editLicenseFeatureInfo->SetFeatureId("EditLicense");
	editLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit License"));
	editLicenseFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing License."));
	editLicenseFeatureInfo->SetOptional(false);
	editLicenseFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeLicenseFeatureInfo;
	changeLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeLicenseFeatureInfo->SetFeatureId("ChangeLicense");
	changeLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change License"));
	changeLicenseFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing License."));
	changeLicenseFeatureInfo->SetOptional(false);
	changeLicenseFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeLicenseNumberFeatureInfo;
	changeLicenseNumberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeLicenseNumberFeatureInfo->SetFeatureId("ChangeLicenseNumber");
	changeLicenseNumberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change License Number"));
	changeLicenseNumberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing License Number."));
	changeLicenseNumberFeatureInfo->SetOptional(false);
	changeLicenseNumberFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeLicenseNumberFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeExpirationFeatureInfo;
	changeExpirationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeExpirationFeatureInfo->SetFeatureId("ChangeExpiration");
	changeExpirationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Expiration"));
	changeExpirationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Expiration."));
	changeExpirationFeatureInfo->SetOptional(false);
	changeExpirationFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeExpirationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeProductLicensesFeatureInfo;
	changeProductLicensesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeProductLicensesFeatureInfo->SetFeatureId("ChangeProductLicenses");
	changeProductLicensesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Product Licenses"));
	changeProductLicensesFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Product Licenses."));
	changeProductLicensesFeatureInfo->SetOptional(false);
	changeProductLicensesFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeProductLicensesFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeProductForLicenseFeatureInfo;
	changeProductForLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeProductForLicenseFeatureInfo->SetFeatureId("ChangeProductForLicense");
	changeProductForLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Product"));
	changeProductForLicenseFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Product."));
	changeProductForLicenseFeatureInfo->SetOptional(false);
	changeProductForLicenseFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeProductForLicenseFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrderForLicenseFeatureInfo;
	changeOrderForLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrderForLicenseFeatureInfo->SetFeatureId("ChangeOrderForLicense");
	changeOrderForLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order"));
	changeOrderForLicenseFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Order."));
	changeOrderForLicenseFeatureInfo->SetOptional(false);
	changeOrderForLicenseFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeOrderForLicenseFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeProjectForLicenseFeatureInfo;
	changeProjectForLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeProjectForLicenseFeatureInfo->SetFeatureId("ChangeProjectForLicense");
	changeProjectForLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Project"));
	changeProjectForLicenseFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Project."));
	changeProjectForLicenseFeatureInfo->SetOptional(false);
	changeProjectForLicenseFeatureInfo->SetIsPermission(true);

	changeLicenseFeatureInfo->InsertSubFeature(changeProjectForLicenseFeatureInfo.PopPtr());

	editLicenseFeatureInfo->InsertSubFeature(changeLicenseFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeLicenseFeatureInfo;
	removeLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeLicenseFeatureInfo->SetFeatureId("RemoveLicense");
	removeLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove License"));
	removeLicenseFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing License."));
	removeLicenseFeatureInfo->SetOptional(false);
	removeLicenseFeatureInfo->SetIsPermission(true);

	editLicenseFeatureInfo->InsertSubFeature(removeLicenseFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addLicenseFeatureInfo;
	addLicenseFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addLicenseFeatureInfo->SetFeatureId("AddLicense");
	addLicenseFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add License"));
	addLicenseFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding License."));
	addLicenseFeatureInfo->SetOptional(false);
	addLicenseFeatureInfo->SetIsPermission(true);

	editLicenseFeatureInfo->InsertSubFeature(addLicenseFeatureInfo.PopPtr());

	licenseManagementFeatureInfo->InsertSubFeature(editLicenseFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewLicensesFeatureInfo;
	viewLicensesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewLicensesFeatureInfo->SetFeatureId("ViewLicenses");
	viewLicensesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Licenses"));
	viewLicensesFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Licenses."));
	viewLicensesFeatureInfo->SetOptional(false);
	viewLicensesFeatureInfo->SetIsPermission(true);
	viewLicensesFeatureInfo->SetRequirements(QByteArray("/LicenseManagement/EditLicense;/LicenseManagement/EditLicense/ChangeLicense;/LicenseManagement/EditLicense/RemoveLicense;/LicenseManagement/EditLicense/AddLicense").split(';'));

	licenseManagementFeatureInfo->InsertSubFeature(viewLicensesFeatureInfo.PopPtr());

	productInfo.AddFeature("9225718e-3c9c-4e00-83d8-b39152e32a66", *licenseManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> accountManagementFeatureInfo;
	accountManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	accountManagementFeatureInfo->SetObjectUuid("d9372bb1-6d29-4531-ba18-4cf7329d99c8");
	accountManagementFeatureInfo->SetFeatureId("AccountManagement");
	accountManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Account Management"));
	accountManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Account-related operations."));
	accountManagementFeatureInfo->SetOptional(false);
	accountManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewAccountHistoryFeatureInfo;
	viewAccountHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAccountHistoryFeatureInfo->SetFeatureId("ViewAccountHistory");
	viewAccountHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Account History"));
	viewAccountHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Account History."));
	viewAccountHistoryFeatureInfo->SetOptional(false);
	viewAccountHistoryFeatureInfo->SetIsPermission(true);

	accountManagementFeatureInfo->InsertSubFeature(viewAccountHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editAccountFeatureInfo;
	editAccountFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editAccountFeatureInfo->SetFeatureId("EditAccount");
	editAccountFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Account"));
	editAccountFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Account."));
	editAccountFeatureInfo->SetOptional(false);
	editAccountFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeAccountFeatureInfo;
	changeAccountFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeAccountFeatureInfo->SetFeatureId("ChangeAccount");
	changeAccountFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account"));
	changeAccountFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Account."));
	changeAccountFeatureInfo->SetOptional(false);
	changeAccountFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeAccountGroupsFeatureInfo;
	changeAccountGroupsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeAccountGroupsFeatureInfo->SetFeatureId("ChangeAccountGroups");
	changeAccountGroupsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Groups"));
	changeAccountGroupsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Account Groups."));
	changeAccountGroupsFeatureInfo->SetOptional(false);
	changeAccountGroupsFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeAccountGroupsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeCompanyAddressFeatureInfo;
	changeCompanyAddressFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeCompanyAddressFeatureInfo->SetFeatureId("ChangeCompanyAddress");
	changeCompanyAddressFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Company Address"));
	changeCompanyAddressFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Company Address."));
	changeCompanyAddressFeatureInfo->SetOptional(false);
	changeCompanyAddressFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeCompanyAddressFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeAccountEmailFeatureInfo;
	changeAccountEmailFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeAccountEmailFeatureInfo->SetFeatureId("ChangeAccountEmail");
	changeAccountEmailFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Email"));
	changeAccountEmailFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Account Email."));
	changeAccountEmailFeatureInfo->SetOptional(false);
	changeAccountEmailFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeAccountEmailFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeAccountDescriptionFeatureInfo;
	changeAccountDescriptionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeAccountDescriptionFeatureInfo->SetFeatureId("ChangeAccountDescription");
	changeAccountDescriptionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Description"));
	changeAccountDescriptionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Account Description."));
	changeAccountDescriptionFeatureInfo->SetOptional(false);
	changeAccountDescriptionFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeAccountDescriptionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeAccountNameFeatureInfo;
	changeAccountNameFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeAccountNameFeatureInfo->SetFeatureId("ChangeAccountName");
	changeAccountNameFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Name"));
	changeAccountNameFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Account Name."));
	changeAccountNameFeatureInfo->SetOptional(false);
	changeAccountNameFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeAccountNameFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeCustomerIdFeatureInfo;
	changeCustomerIdFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeCustomerIdFeatureInfo->SetFeatureId("ChangeCustomerId");
	changeCustomerIdFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Customer-ID"));
	changeCustomerIdFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Customer-ID."));
	changeCustomerIdFeatureInfo->SetOptional(false);
	changeCustomerIdFeatureInfo->SetIsPermission(true);

	changeAccountFeatureInfo->InsertSubFeature(changeCustomerIdFeatureInfo.PopPtr());

	editAccountFeatureInfo->InsertSubFeature(changeAccountFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeAccountFeatureInfo;
	removeAccountFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeAccountFeatureInfo->SetFeatureId("RemoveAccount");
	removeAccountFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Account"));
	removeAccountFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Account."));
	removeAccountFeatureInfo->SetOptional(false);
	removeAccountFeatureInfo->SetIsPermission(true);

	editAccountFeatureInfo->InsertSubFeature(removeAccountFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addAccountFeatureInfo;
	addAccountFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addAccountFeatureInfo->SetFeatureId("AddAccount");
	addAccountFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Account"));
	addAccountFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Account."));
	addAccountFeatureInfo->SetOptional(false);
	addAccountFeatureInfo->SetIsPermission(true);

	editAccountFeatureInfo->InsertSubFeature(addAccountFeatureInfo.PopPtr());

	accountManagementFeatureInfo->InsertSubFeature(editAccountFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAccountsFeatureInfo;
	viewAccountsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAccountsFeatureInfo->SetFeatureId("ViewAccounts");
	viewAccountsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Accounts"));
	viewAccountsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Accounts."));
	viewAccountsFeatureInfo->SetOptional(false);
	viewAccountsFeatureInfo->SetIsPermission(true);
	viewAccountsFeatureInfo->SetRequirements(QByteArray("/AccountManagement/EditAccount;/AccountManagement/EditAccount/ChangeAccount;/AccountManagement/EditAccount/RemoveAccount;/AccountManagement/EditAccount/AddAccount").split(';'));

	accountManagementFeatureInfo->InsertSubFeature(viewAccountsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAllAccountsFeatureInfo;
	viewAllAccountsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAllAccountsFeatureInfo->SetFeatureId("ViewAllAccounts");
	viewAllAccountsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Accounts"));
	viewAllAccountsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing all Accounts."));
	viewAllAccountsFeatureInfo->SetOptional(false);
	viewAllAccountsFeatureInfo->SetIsPermission(true);
	viewAllAccountsFeatureInfo->SetRequirements(QByteArray("/AccountManagement/ViewAccounts").split(';'));

	accountManagementFeatureInfo->InsertSubFeature(viewAllAccountsFeatureInfo.PopPtr());

	productInfo.AddFeature("d9372bb1-6d29-4531-ba18-4cf7329d99c8", *accountManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> organizationManagementFeatureInfo;
	organizationManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	organizationManagementFeatureInfo->SetObjectUuid("f1b9f9b0-7d54-4fd1-9d17-8d2f8c5f2cf0");
	organizationManagementFeatureInfo->SetFeatureId("OrganizationManagement");
	organizationManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Organization Management"));
	organizationManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Organization-related operations."));
	organizationManagementFeatureInfo->SetOptional(false);
	organizationManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationsFeatureInfo;
	viewOrganizationsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationsFeatureInfo->SetFeatureId("ViewOrganizations");
	viewOrganizationsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organizations"));
	viewOrganizationsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organizations."));
	viewOrganizationsFeatureInfo->SetOptional(false);
	viewOrganizationsFeatureInfo->SetIsPermission(true);
	organizationManagementFeatureInfo->InsertSubFeature(viewOrganizationsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrganizationFeatureInfo;
	editOrganizationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrganizationFeatureInfo->SetFeatureId("EditOrganization");
	editOrganizationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization"));
	editOrganizationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Organization."));
	editOrganizationFeatureInfo->SetOptional(false);
	editOrganizationFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationFeatureInfo;
	changeOrganizationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationFeatureInfo->SetFeatureId("ChangeOrganization");
	changeOrganizationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization"));
	changeOrganizationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Organization."));
	changeOrganizationFeatureInfo->SetOptional(false);
	changeOrganizationFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationNameFeatureInfo;
	changeOrganizationNameFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationNameFeatureInfo->SetFeatureId("ChangeOrganizationName");
	changeOrganizationNameFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Name"));
	changeOrganizationNameFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Organization Name."));
	changeOrganizationNameFeatureInfo->SetOptional(false);
	changeOrganizationNameFeatureInfo->SetIsPermission(true);
	changeOrganizationFeatureInfo->InsertSubFeature(changeOrganizationNameFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationDescriptionFeatureInfo;
	changeOrganizationDescriptionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationDescriptionFeatureInfo->SetFeatureId("ChangeOrganizationDescription");
	changeOrganizationDescriptionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Description"));
	changeOrganizationDescriptionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Organization Description."));
	changeOrganizationDescriptionFeatureInfo->SetOptional(false);
	changeOrganizationDescriptionFeatureInfo->SetIsPermission(true);
	changeOrganizationFeatureInfo->InsertSubFeature(changeOrganizationDescriptionFeatureInfo.PopPtr());

	editOrganizationFeatureInfo->InsertSubFeature(changeOrganizationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeOrganizationFeatureInfo;
	removeOrganizationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeOrganizationFeatureInfo->SetFeatureId("RemoveOrganization");
	removeOrganizationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Organization"));
	removeOrganizationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Organization."));
	removeOrganizationFeatureInfo->SetOptional(false);
	removeOrganizationFeatureInfo->SetIsPermission(true);
	editOrganizationFeatureInfo->InsertSubFeature(removeOrganizationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addOrganizationFeatureInfo;
	addOrganizationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addOrganizationFeatureInfo->SetFeatureId("AddOrganization");
	addOrganizationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Organization"));
	addOrganizationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Organization."));
	addOrganizationFeatureInfo->SetOptional(false);
	addOrganizationFeatureInfo->SetIsPermission(true);
	editOrganizationFeatureInfo->InsertSubFeature(addOrganizationFeatureInfo.PopPtr());

	organizationManagementFeatureInfo->InsertSubFeature(editOrganizationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> memberManagementFeatureInfo;
	memberManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	memberManagementFeatureInfo->SetFeatureId("MemberManagement");
	memberManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Member Management"));
	memberManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Member-related operations."));
	memberManagementFeatureInfo->SetOptional(false);
	memberManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationMembersFeatureInfo;
	viewOrganizationMembersFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationMembersFeatureInfo->SetFeatureId("ViewOrganizationMembers");
	viewOrganizationMembersFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Members"));
	viewOrganizationMembersFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Members."));
	viewOrganizationMembersFeatureInfo->SetOptional(false);
	viewOrganizationMembersFeatureInfo->SetIsPermission(true);
	viewOrganizationMembersFeatureInfo->SetRequirements(QByteArray("/OrganizationManagement/MemberManagement/EditOrganizationMember;/OrganizationManagement/MemberManagement/EditOrganizationMember/InviteOrganizationMember;/OrganizationManagement/MemberManagement/EditOrganizationMember/RemoveOrganizationMember;/OrganizationManagement/MemberManagement/EditOrganizationMember/ExcludeOrganizationMember;/OrganizationManagement/MemberManagement/EditOrganizationMember/ChangeOrganizationMember;/OrganizationManagement/MemberManagement/EditOrganizationMember/ChangeOrganizationMember/ChangeOrganizationMemberRole").split(';'));
	memberManagementFeatureInfo->InsertSubFeature(viewOrganizationMembersFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrganizationMemberFeatureInfo;
	editOrganizationMemberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrganizationMemberFeatureInfo->SetFeatureId("EditOrganizationMember");
	editOrganizationMemberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization Member"));
	editOrganizationMemberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Organization Member."));
	editOrganizationMemberFeatureInfo->SetOptional(false);
	editOrganizationMemberFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> inviteOrganizationMemberFeatureInfo;
	inviteOrganizationMemberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	inviteOrganizationMemberFeatureInfo->SetFeatureId("InviteOrganizationMember");
	inviteOrganizationMemberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Invite Organization Member"));
	inviteOrganizationMemberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows inviting Organization Member."));
	inviteOrganizationMemberFeatureInfo->SetOptional(false);
	inviteOrganizationMemberFeatureInfo->SetIsPermission(true);
	editOrganizationMemberFeatureInfo->InsertSubFeature(inviteOrganizationMemberFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeOrganizationMemberFeatureInfo;
	removeOrganizationMemberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeOrganizationMemberFeatureInfo->SetFeatureId("RemoveOrganizationMember");
	removeOrganizationMemberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Organization Member"));
	removeOrganizationMemberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Organization Member."));
	removeOrganizationMemberFeatureInfo->SetOptional(false);
	removeOrganizationMemberFeatureInfo->SetIsPermission(true);
	editOrganizationMemberFeatureInfo->InsertSubFeature(removeOrganizationMemberFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> excludeOrganizationMemberFeatureInfo;
	excludeOrganizationMemberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	excludeOrganizationMemberFeatureInfo->SetFeatureId("ExcludeOrganizationMember");
	excludeOrganizationMemberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Exclude Organization Member"));
	excludeOrganizationMemberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows excluding a member from the organization."));
	excludeOrganizationMemberFeatureInfo->SetOptional(false);
	excludeOrganizationMemberFeatureInfo->SetIsPermission(true);
	editOrganizationMemberFeatureInfo->InsertSubFeature(excludeOrganizationMemberFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationMemberFeatureInfo;
	changeOrganizationMemberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationMemberFeatureInfo->SetFeatureId("ChangeOrganizationMember");
	changeOrganizationMemberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Member"));
	changeOrganizationMemberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing a member within the organization."));
	changeOrganizationMemberFeatureInfo->SetOptional(false);
	changeOrganizationMemberFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationMemberRoleFeatureInfo;
	changeOrganizationMemberRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationMemberRoleFeatureInfo->SetFeatureId("ChangeOrganizationMemberRole");
	changeOrganizationMemberRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Member Role"));
	changeOrganizationMemberRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Organization Member Role."));
	changeOrganizationMemberRoleFeatureInfo->SetOptional(false);
	changeOrganizationMemberRoleFeatureInfo->SetIsPermission(true);
	changeOrganizationMemberFeatureInfo->InsertSubFeature(changeOrganizationMemberRoleFeatureInfo.PopPtr());
	editOrganizationMemberFeatureInfo->InsertSubFeature(changeOrganizationMemberFeatureInfo.PopPtr());

	memberManagementFeatureInfo->InsertSubFeature(editOrganizationMemberFeatureInfo.PopPtr());
	organizationManagementFeatureInfo->InsertSubFeature(memberManagementFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> roleManagementFeatureInfo;
	roleManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	roleManagementFeatureInfo->SetFeatureId("RoleManagement");
	roleManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Role Management"));
	roleManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Role-related operations."));
	roleManagementFeatureInfo->SetOptional(false);
	roleManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationRolesFeatureInfo;
	viewOrganizationRolesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationRolesFeatureInfo->SetFeatureId("ViewOrganizationRoles");
	viewOrganizationRolesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Roles"));
	viewOrganizationRolesFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Roles."));
	viewOrganizationRolesFeatureInfo->SetOptional(false);
	viewOrganizationRolesFeatureInfo->SetIsPermission(true);
	viewOrganizationRolesFeatureInfo->SetRequirements(QByteArray("/OrganizationManagement/RoleManagement/EditOrganizationRole;/OrganizationManagement/RoleManagement/EditOrganizationRole/ChangeOrganizationRole;/OrganizationManagement/RoleManagement/EditOrganizationRole/RemoveOrganizationRole;/OrganizationManagement/RoleManagement/EditOrganizationRole/AddOrganizationRole").split(';'));
	roleManagementFeatureInfo->InsertSubFeature(viewOrganizationRolesFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrganizationRoleFeatureInfo;
	editOrganizationRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrganizationRoleFeatureInfo->SetFeatureId("EditOrganizationRole");
	editOrganizationRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization Role"));
	editOrganizationRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Organization Role."));
	editOrganizationRoleFeatureInfo->SetOptional(false);
	editOrganizationRoleFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationRoleFeatureInfo;
	changeOrganizationRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationRoleFeatureInfo->SetFeatureId("ChangeOrganizationRole");
	changeOrganizationRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Role"));
	changeOrganizationRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Organization Role."));
	changeOrganizationRoleFeatureInfo->SetOptional(false);
	changeOrganizationRoleFeatureInfo->SetIsPermission(true);
	editOrganizationRoleFeatureInfo->InsertSubFeature(changeOrganizationRoleFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeOrganizationRoleFeatureInfo;
	removeOrganizationRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeOrganizationRoleFeatureInfo->SetFeatureId("RemoveOrganizationRole");
	removeOrganizationRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Organization Role"));
	removeOrganizationRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Organization Role."));
	removeOrganizationRoleFeatureInfo->SetOptional(false);
	removeOrganizationRoleFeatureInfo->SetIsPermission(true);
	editOrganizationRoleFeatureInfo->InsertSubFeature(removeOrganizationRoleFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addOrganizationRoleFeatureInfo;
	addOrganizationRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addOrganizationRoleFeatureInfo->SetFeatureId("AddOrganizationRole");
	addOrganizationRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Organization Role"));
	addOrganizationRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Organization Role."));
	addOrganizationRoleFeatureInfo->SetOptional(false);
	addOrganizationRoleFeatureInfo->SetIsPermission(true);
	editOrganizationRoleFeatureInfo->InsertSubFeature(addOrganizationRoleFeatureInfo.PopPtr());

	roleManagementFeatureInfo->InsertSubFeature(editOrganizationRoleFeatureInfo.PopPtr());
	organizationManagementFeatureInfo->InsertSubFeature(roleManagementFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> groupManagementFeatureInfo;
	groupManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	groupManagementFeatureInfo->SetFeatureId("GroupManagement");
	groupManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Group Management"));
	groupManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Group-related operations."));
	groupManagementFeatureInfo->SetOptional(false);
	groupManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationGroupsFeatureInfo;
	viewOrganizationGroupsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationGroupsFeatureInfo->SetFeatureId("ViewOrganizationGroups");
	viewOrganizationGroupsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Groups"));
	viewOrganizationGroupsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Groups."));
	viewOrganizationGroupsFeatureInfo->SetOptional(false);
	viewOrganizationGroupsFeatureInfo->SetIsPermission(true);
	viewOrganizationGroupsFeatureInfo->SetRequirements(QByteArray("/OrganizationManagement/GroupManagement/EditOrganizationGroup;/OrganizationManagement/GroupManagement/EditOrganizationGroup/ChangeOrganizationGroup;/OrganizationManagement/GroupManagement/EditOrganizationGroup/RemoveOrganizationGroup;/OrganizationManagement/GroupManagement/EditOrganizationGroup/AddOrganizationGroup").split(';'));
	groupManagementFeatureInfo->InsertSubFeature(viewOrganizationGroupsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrganizationGroupFeatureInfo;
	editOrganizationGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrganizationGroupFeatureInfo->SetFeatureId("EditOrganizationGroup");
	editOrganizationGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization Group"));
	editOrganizationGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Organization Group."));
	editOrganizationGroupFeatureInfo->SetOptional(false);
	editOrganizationGroupFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationGroupFeatureInfo;
	changeOrganizationGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationGroupFeatureInfo->SetFeatureId("ChangeOrganizationGroup");
	changeOrganizationGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Group"));
	changeOrganizationGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Organization Group."));
	changeOrganizationGroupFeatureInfo->SetOptional(false);
	changeOrganizationGroupFeatureInfo->SetIsPermission(true);
	editOrganizationGroupFeatureInfo->InsertSubFeature(changeOrganizationGroupFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeOrganizationGroupFeatureInfo;
	removeOrganizationGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeOrganizationGroupFeatureInfo->SetFeatureId("RemoveOrganizationGroup");
	removeOrganizationGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Organization Group"));
	removeOrganizationGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Organization Group."));
	removeOrganizationGroupFeatureInfo->SetOptional(false);
	removeOrganizationGroupFeatureInfo->SetIsPermission(true);
	editOrganizationGroupFeatureInfo->InsertSubFeature(removeOrganizationGroupFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addOrganizationGroupFeatureInfo;
	addOrganizationGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addOrganizationGroupFeatureInfo->SetFeatureId("AddOrganizationGroup");
	addOrganizationGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Organization Group"));
	addOrganizationGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Organization Group."));
	addOrganizationGroupFeatureInfo->SetOptional(false);
	addOrganizationGroupFeatureInfo->SetIsPermission(true);
	editOrganizationGroupFeatureInfo->InsertSubFeature(addOrganizationGroupFeatureInfo.PopPtr());

	groupManagementFeatureInfo->InsertSubFeature(editOrganizationGroupFeatureInfo.PopPtr());
	organizationManagementFeatureInfo->InsertSubFeature(groupManagementFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> permissionManagementFeatureInfo;
	permissionManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	permissionManagementFeatureInfo->SetFeatureId("PermissionManagement");
	permissionManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Permission Management"));
	permissionManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Permission-related operations."));
	permissionManagementFeatureInfo->SetOptional(false);
	permissionManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationPermissionsFeatureInfo;
	viewOrganizationPermissionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationPermissionsFeatureInfo->SetFeatureId("ViewOrganizationPermissions");
	viewOrganizationPermissionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Permissions"));
	viewOrganizationPermissionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Permissions."));
	viewOrganizationPermissionsFeatureInfo->SetOptional(false);
	viewOrganizationPermissionsFeatureInfo->SetIsPermission(true);
	permissionManagementFeatureInfo->InsertSubFeature(viewOrganizationPermissionsFeatureInfo.PopPtr());
	organizationManagementFeatureInfo->InsertSubFeature(permissionManagementFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> connectionManagementFeatureInfo;
	connectionManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	connectionManagementFeatureInfo->SetFeatureId("ConnectionManagement");
	connectionManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Connection Management"));
	connectionManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Connection-related operations."));
	connectionManagementFeatureInfo->SetOptional(false);
	connectionManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationConnectionsFeatureInfo;
	viewOrganizationConnectionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationConnectionsFeatureInfo->SetFeatureId("ViewOrganizationConnections");
	viewOrganizationConnectionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Connections"));
	viewOrganizationConnectionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Connections."));
	viewOrganizationConnectionsFeatureInfo->SetOptional(false);
	viewOrganizationConnectionsFeatureInfo->SetIsPermission(true);
	viewOrganizationConnectionsFeatureInfo->SetRequirements(QByteArray("/OrganizationManagement/ConnectionManagement/EditOrganizationConnection;/OrganizationManagement/ConnectionManagement/EditOrganizationConnection/ConnectOrganization;/OrganizationManagement/ConnectionManagement/EditOrganizationConnection/RemoveOrganizationConnection").split(';'));
	connectionManagementFeatureInfo->InsertSubFeature(viewOrganizationConnectionsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrganizationConnectionFeatureInfo;
	editOrganizationConnectionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrganizationConnectionFeatureInfo->SetFeatureId("EditOrganizationConnection");
	editOrganizationConnectionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization Connection"));
	editOrganizationConnectionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Organization Connection."));
	editOrganizationConnectionFeatureInfo->SetOptional(false);
	editOrganizationConnectionFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> connectOrganizationFeatureInfo;
	connectOrganizationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	connectOrganizationFeatureInfo->SetFeatureId("ConnectOrganization");
	connectOrganizationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Connect Organization"));
	connectOrganizationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows connecting Organization."));
	connectOrganizationFeatureInfo->SetOptional(false);
	connectOrganizationFeatureInfo->SetIsPermission(true);
	editOrganizationConnectionFeatureInfo->InsertSubFeature(connectOrganizationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeOrganizationConnectionFeatureInfo;
	removeOrganizationConnectionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeOrganizationConnectionFeatureInfo->SetFeatureId("RemoveOrganizationConnection");
	removeOrganizationConnectionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Organization Connection"));
	removeOrganizationConnectionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Organization Connection."));
	removeOrganizationConnectionFeatureInfo->SetOptional(false);
	removeOrganizationConnectionFeatureInfo->SetIsPermission(true);
	editOrganizationConnectionFeatureInfo->InsertSubFeature(removeOrganizationConnectionFeatureInfo.PopPtr());

	connectionManagementFeatureInfo->InsertSubFeature(editOrganizationConnectionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationConnectionCodeFeatureInfo;
	viewOrganizationConnectionCodeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationConnectionCodeFeatureInfo->SetFeatureId("ViewOrganizationConnectionCode");
	viewOrganizationConnectionCodeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Connection Code"));
	viewOrganizationConnectionCodeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Connection Code."));
	viewOrganizationConnectionCodeFeatureInfo->SetOptional(false);
	viewOrganizationConnectionCodeFeatureInfo->SetIsPermission(true);
	connectionManagementFeatureInfo->InsertSubFeature(viewOrganizationConnectionCodeFeatureInfo.PopPtr());
	organizationManagementFeatureInfo->InsertSubFeature(connectionManagementFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> contractManagementFeatureInfo;
	contractManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	contractManagementFeatureInfo->SetFeatureId("ContractManagement");
	contractManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Contract Management"));
	contractManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Contract-related operations."));
	contractManagementFeatureInfo->SetOptional(false);
	contractManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationContractsFeatureInfo;
	viewOrganizationContractsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationContractsFeatureInfo->SetFeatureId("ViewOrganizationContracts");
	viewOrganizationContractsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Contracts"));
	viewOrganizationContractsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Contracts."));
	viewOrganizationContractsFeatureInfo->SetOptional(false);
	viewOrganizationContractsFeatureInfo->SetIsPermission(true);
	contractManagementFeatureInfo->InsertSubFeature(viewOrganizationContractsFeatureInfo.PopPtr());
	organizationManagementFeatureInfo->InsertSubFeature(contractManagementFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> messageManagementFeatureInfo;
	messageManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	messageManagementFeatureInfo->SetFeatureId("MessageManagement");
	messageManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Message Management"));
	messageManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Message-related operations."));
	messageManagementFeatureInfo->SetOptional(false);
	messageManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationMessagesFeatureInfo;
	viewOrganizationMessagesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationMessagesFeatureInfo->SetFeatureId("ViewOrganizationMessages");
	viewOrganizationMessagesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Messages"));
	viewOrganizationMessagesFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Messages."));
	viewOrganizationMessagesFeatureInfo->SetOptional(false);
	viewOrganizationMessagesFeatureInfo->SetIsPermission(true);
	messageManagementFeatureInfo->InsertSubFeature(viewOrganizationMessagesFeatureInfo.PopPtr());
	organizationManagementFeatureInfo->InsertSubFeature(messageManagementFeatureInfo.PopPtr());

	productInfo.AddFeature("f1b9f9b0-7d54-4fd1-9d17-8d2f8c5f2cf0", *organizationManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> searchManagementFeatureInfo;
	searchManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	searchManagementFeatureInfo->SetObjectUuid("3d98c36e-882a-4a5a-9bff-f932c62b3757");
	searchManagementFeatureInfo->SetFeatureId("SearchManagement");
	searchManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Search management"));
	searchManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Search-related operations."));
	searchManagementFeatureInfo->SetOptional(false);
	searchManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewSearchFeatureInfo;
	viewSearchFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewSearchFeatureInfo->SetFeatureId("ViewSearch");
	viewSearchFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View search page"));
	viewSearchFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Opens the search page."));
	viewSearchFeatureInfo->SetOptional(false);
	viewSearchFeatureInfo->SetIsPermission(true);

	searchManagementFeatureInfo->InsertSubFeature(viewSearchFeatureInfo.PopPtr());

	productInfo.AddFeature("3d98c36e-882a-4a5a-9bff-f932c62b3757", *searchManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> revisionManagementFeatureInfo;
	revisionManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	revisionManagementFeatureInfo->SetObjectUuid("c52f36c9-0dbe-49f0-84a7-f59116bd7225");
	revisionManagementFeatureInfo->SetFeatureId("RevisionManagement");
	revisionManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Revision Management"));
	revisionManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Revision-related operations."));
	revisionManagementFeatureInfo->SetOptional(false);
	revisionManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> deleteRevisionFeatureInfo;
	deleteRevisionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	deleteRevisionFeatureInfo->SetFeatureId("DeleteRevision");
	deleteRevisionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Delete Revision"));
	deleteRevisionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows deleting Revision."));
	deleteRevisionFeatureInfo->SetOptional(false);
	deleteRevisionFeatureInfo->SetIsPermission(true);

	revisionManagementFeatureInfo->InsertSubFeature(deleteRevisionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> exportRevisionFeatureInfo;
	exportRevisionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	exportRevisionFeatureInfo->SetFeatureId("ExportRevision");
	exportRevisionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Export Revision"));
	exportRevisionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows exporting Revision."));
	exportRevisionFeatureInfo->SetOptional(false);
	exportRevisionFeatureInfo->SetIsPermission(true);

	revisionManagementFeatureInfo->InsertSubFeature(exportRevisionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewRevisionsFeatureInfo;
	viewRevisionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewRevisionsFeatureInfo->SetFeatureId("ViewRevisions");
	viewRevisionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Revisions"));
	viewRevisionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Revisions."));
	viewRevisionsFeatureInfo->SetOptional(false);
	viewRevisionsFeatureInfo->SetIsPermission(true);

	revisionManagementFeatureInfo->InsertSubFeature(viewRevisionsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> backupRevisionFeatureInfo;
	backupRevisionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	backupRevisionFeatureInfo->SetFeatureId("BackupRevision");
	backupRevisionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Backup Revision"));
	backupRevisionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows backing up Revision."));
	backupRevisionFeatureInfo->SetOptional(false);
	backupRevisionFeatureInfo->SetIsPermission(true);

	revisionManagementFeatureInfo->InsertSubFeature(backupRevisionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> restoreRevisionFeatureInfo;
	restoreRevisionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	restoreRevisionFeatureInfo->SetFeatureId("RestoreRevision");
	restoreRevisionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Restore Revision"));
	restoreRevisionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows restoring Revision."));
	restoreRevisionFeatureInfo->SetOptional(false);
	restoreRevisionFeatureInfo->SetIsPermission(true);

	revisionManagementFeatureInfo->InsertSubFeature(restoreRevisionFeatureInfo.PopPtr());

	productInfo.AddFeature("c52f36c9-0dbe-49f0-84a7-f59116bd7225", *revisionManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> administrationFeatureInfo;
	administrationFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	administrationFeatureInfo->SetObjectUuid("ce4a7f72-5303-4044-b25c-3a02020eebd3");
	administrationFeatureInfo->SetFeatureId("Administration");
	administrationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Administration"));
	administrationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to administration pages."));
	administrationFeatureInfo->SetOptional(false);
	administrationFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> editAdministrationFeatureInfo;
	editAdministrationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editAdministrationFeatureInfo->SetFeatureId("EditAdministration");
	editAdministrationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Administration"));
	editAdministrationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Administration."));
	editAdministrationFeatureInfo->SetOptional(false);
	editAdministrationFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> editGroupFeatureInfo;
	editGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editGroupFeatureInfo->SetFeatureId("EditGroup");
	editGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Group"));
	editGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Group."));
	editGroupFeatureInfo->SetOptional(false);
	editGroupFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeGroupFeatureInfo;
	changeGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeGroupFeatureInfo->SetFeatureId("ChangeGroup");
	changeGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Group"));
	changeGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Group."));
	changeGroupFeatureInfo->SetOptional(false);
	changeGroupFeatureInfo->SetIsPermission(true);

	editGroupFeatureInfo->InsertSubFeature(changeGroupFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeGroupFeatureInfo;
	removeGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeGroupFeatureInfo->SetFeatureId("RemoveGroup");
	removeGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Group"));
	removeGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Group."));
	removeGroupFeatureInfo->SetOptional(false);
	removeGroupFeatureInfo->SetIsPermission(true);

	editGroupFeatureInfo->InsertSubFeature(removeGroupFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addGroupFeatureInfo;
	addGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addGroupFeatureInfo->SetFeatureId("AddGroup");
	addGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Group"));
	addGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Group."));
	addGroupFeatureInfo->SetOptional(false);
	addGroupFeatureInfo->SetIsPermission(true);

	editGroupFeatureInfo->InsertSubFeature(addGroupFeatureInfo.PopPtr());

	editAdministrationFeatureInfo->InsertSubFeature(editGroupFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editRoleFeatureInfo;
	editRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editRoleFeatureInfo->SetFeatureId("EditRole");
	editRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Role"));
	editRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Role."));
	editRoleFeatureInfo->SetOptional(false);
	editRoleFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeRoleFeatureInfo;
	changeRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeRoleFeatureInfo->SetFeatureId("ChangeRole");
	changeRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Role"));
	changeRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Role."));
	changeRoleFeatureInfo->SetOptional(false);
	changeRoleFeatureInfo->SetIsPermission(true);

	editRoleFeatureInfo->InsertSubFeature(changeRoleFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeRoleFeatureInfo;
	removeRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeRoleFeatureInfo->SetFeatureId("RemoveRole");
	removeRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Role"));
	removeRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Role."));
	removeRoleFeatureInfo->SetOptional(false);
	removeRoleFeatureInfo->SetIsPermission(true);

	editRoleFeatureInfo->InsertSubFeature(removeRoleFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addRoleFeatureInfo;
	addRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addRoleFeatureInfo->SetFeatureId("AddRole");
	addRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Role"));
	addRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Role."));
	addRoleFeatureInfo->SetOptional(false);
	addRoleFeatureInfo->SetIsPermission(true);

	editRoleFeatureInfo->InsertSubFeature(addRoleFeatureInfo.PopPtr());

	editAdministrationFeatureInfo->InsertSubFeature(editRoleFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editUserFeatureInfo;
	editUserFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editUserFeatureInfo->SetFeatureId("EditUser");
	editUserFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit User"));
	editUserFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing User."));
	editUserFeatureInfo->SetOptional(false);
	editUserFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeUserFeatureInfo;
	changeUserFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeUserFeatureInfo->SetFeatureId("ChangeUser");
	changeUserFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change User"));
	changeUserFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing User."));
	changeUserFeatureInfo->SetOptional(false);
	changeUserFeatureInfo->SetIsPermission(true);

	editUserFeatureInfo->InsertSubFeature(changeUserFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeUserFeatureInfo;
	removeUserFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeUserFeatureInfo->SetFeatureId("RemoveUser");
	removeUserFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove User"));
	removeUserFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing User."));
	removeUserFeatureInfo->SetOptional(false);
	removeUserFeatureInfo->SetIsPermission(true);

	editUserFeatureInfo->InsertSubFeature(removeUserFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addUserFeatureInfo;
	addUserFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addUserFeatureInfo->SetFeatureId("AddUser");
	addUserFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add User"));
	addUserFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding User."));
	addUserFeatureInfo->SetOptional(false);
	addUserFeatureInfo->SetIsPermission(true);

	editUserFeatureInfo->InsertSubFeature(addUserFeatureInfo.PopPtr());

	editAdministrationFeatureInfo->InsertSubFeature(editUserFeatureInfo.PopPtr());

	administrationFeatureInfo->InsertSubFeature(editAdministrationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAdministrationFeatureInfo;
	viewAdministrationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAdministrationFeatureInfo->SetFeatureId("ViewAdministration");
	viewAdministrationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Administration"));
	viewAdministrationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Administration."));
	viewAdministrationFeatureInfo->SetOptional(false);
	viewAdministrationFeatureInfo->SetIsPermission(true);
	viewAdministrationFeatureInfo->SetRequirements(QByteArray("/Administration/EditAdministration;/Administration/EditAdministration/EditGroup;/Administration/EditAdministration/EditGroup/ChangeGroup;/Administration/EditAdministration/EditGroup/RemoveGroup;/Administration/EditAdministration/EditGroup/AddGroup;/Administration/EditAdministration/EditRole;/Administration/EditAdministration/EditRole/ChangeRole;/Administration/EditAdministration/EditRole/RemoveRole;/Administration/EditAdministration/EditRole/AddRole;/Administration/EditAdministration/EditUser;/Administration/EditAdministration/EditUser/ChangeUser;/Administration/EditAdministration/EditUser/RemoveUser;/Administration/EditAdministration/EditUser/AddUser").split(';'));

	istd::TDelPtr<imtlic::CFeatureInfo> viewGroupHistoryFeatureInfo;
	viewGroupHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewGroupHistoryFeatureInfo->SetFeatureId("ViewGroupHistory");
	viewGroupHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Group History"));
	viewGroupHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Group History."));
	viewGroupHistoryFeatureInfo->SetOptional(false);
	viewGroupHistoryFeatureInfo->SetIsPermission(true);

	viewAdministrationFeatureInfo->InsertSubFeature(viewGroupHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewUserHistoryFeatureInfo;
	viewUserHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewUserHistoryFeatureInfo->SetFeatureId("ViewUserHistory");
	viewUserHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View User History"));
	viewUserHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing User History."));
	viewUserHistoryFeatureInfo->SetOptional(false);
	viewUserHistoryFeatureInfo->SetIsPermission(true);

	viewAdministrationFeatureInfo->InsertSubFeature(viewUserHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewRoleHistoryFeatureInfo;
	viewRoleHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewRoleHistoryFeatureInfo->SetFeatureId("ViewRoleHistory");
	viewRoleHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Role History"));
	viewRoleHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Role History."));
	viewRoleHistoryFeatureInfo->SetOptional(false);
	viewRoleHistoryFeatureInfo->SetIsPermission(true);

	viewAdministrationFeatureInfo->InsertSubFeature(viewRoleHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewGroupsFeatureInfo;
	viewGroupsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewGroupsFeatureInfo->SetFeatureId("ViewGroups");
	viewGroupsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Groups"));
	viewGroupsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Groups."));
	viewGroupsFeatureInfo->SetOptional(false);
	viewGroupsFeatureInfo->SetIsPermission(true);
	viewGroupsFeatureInfo->SetRequirements(QByteArray("/Administration/EditAdministration/EditGroup;/Administration/EditAdministration/EditGroup/ChangeGroup;/Administration/EditAdministration/EditGroup/RemoveGroup;/Administration/EditAdministration/EditGroup/AddGroup").split(';'));

	viewAdministrationFeatureInfo->InsertSubFeature(viewGroupsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewUsersFeatureInfo;
	viewUsersFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewUsersFeatureInfo->SetFeatureId("ViewUsers");
	viewUsersFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Users"));
	viewUsersFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Users."));
	viewUsersFeatureInfo->SetOptional(false);
	viewUsersFeatureInfo->SetIsPermission(true);
	viewUsersFeatureInfo->SetRequirements(QByteArray("/Administration/EditAdministration/EditUser;/Administration/EditAdministration/EditUser/ChangeUser;/Administration/EditAdministration/EditUser/RemoveUser;/Administration/EditAdministration/EditUser/AddUser").split(';'));

	viewAdministrationFeatureInfo->InsertSubFeature(viewUsersFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewRolesFeatureInfo;
	viewRolesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewRolesFeatureInfo->SetFeatureId("ViewRoles");
	viewRolesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Roles"));
	viewRolesFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Roles."));
	viewRolesFeatureInfo->SetOptional(false);
	viewRolesFeatureInfo->SetIsPermission(true);
	viewRolesFeatureInfo->SetRequirements(QByteArray("/Administration/EditAdministration/EditRole;/Administration/EditAdministration/EditRole/ChangeRole;/Administration/EditAdministration/EditRole/RemoveRole;/Administration/EditAdministration/EditRole/AddRole").split(';'));

	viewAdministrationFeatureInfo->InsertSubFeature(viewRolesFeatureInfo.PopPtr());

	administrationFeatureInfo->InsertSubFeature(viewAdministrationFeatureInfo.PopPtr());

	productInfo.AddFeature("ce4a7f72-5303-4044-b25c-3a02020eebd3", *administrationFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> workspaceManagementFeatureInfo;
	workspaceManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	workspaceManagementFeatureInfo->SetObjectUuid("df22ac46-7253-4b13-a1b8-d4391943adde");
	workspaceManagementFeatureInfo->SetFeatureId("WorkspaceManagement");
	workspaceManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Workspace Management"));
	workspaceManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Workspace-related operations."));
	workspaceManagementFeatureInfo->SetOptional(false);
	workspaceManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewWorkspaceFeatureInfo;
	viewWorkspaceFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewWorkspaceFeatureInfo->SetFeatureId("ViewWorkspace");
	viewWorkspaceFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Workspace"));
	viewWorkspaceFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Workspace."));
	viewWorkspaceFeatureInfo->SetOptional(false);
	viewWorkspaceFeatureInfo->SetIsPermission(true);

	workspaceManagementFeatureInfo->InsertSubFeature(viewWorkspaceFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewUserActionsFeatureInfo;
	viewUserActionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewUserActionsFeatureInfo->SetFeatureId("ViewUserActions");
	viewUserActionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View User Actions"));
	viewUserActionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing User Actions."));
	viewUserActionsFeatureInfo->SetOptional(false);
	viewUserActionsFeatureInfo->SetIsPermission(true);

	workspaceManagementFeatureInfo->InsertSubFeature(viewUserActionsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAllUserActionsFeatureInfo;
	viewAllUserActionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAllUserActionsFeatureInfo->SetFeatureId("ViewAllUserActions");
	viewAllUserActionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All User Actions"));
	viewAllUserActionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing all User Actions."));
	viewAllUserActionsFeatureInfo->SetOptional(false);
	viewAllUserActionsFeatureInfo->SetIsPermission(true);

	workspaceManagementFeatureInfo->InsertSubFeature(viewAllUserActionsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewAnalyticsFeatureInfo;
	viewAnalyticsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewAnalyticsFeatureInfo->SetFeatureId("ViewAnalytics");
	viewAnalyticsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Analytics"));
	viewAnalyticsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Analytics."));
	viewAnalyticsFeatureInfo->SetOptional(false);
	viewAnalyticsFeatureInfo->SetIsPermission(true);

	workspaceManagementFeatureInfo->InsertSubFeature(viewAnalyticsFeatureInfo.PopPtr());

	productInfo.AddFeature("df22ac46-7253-4b13-a1b8-d4391943adde", *workspaceManagementFeatureInfo.GetPtr());

}


};

