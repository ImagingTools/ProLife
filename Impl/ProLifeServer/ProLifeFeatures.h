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

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementViewPageFeatureInfo;
	sensorManagementViewPageFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementViewPageFeatureInfo->SetFeatureId("ViewPage");
	sensorManagementViewPageFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Sensor Page"));
	sensorManagementViewPageFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows opening the sensor page."));
	sensorManagementViewPageFeatureInfo->SetOptional(false);
	sensorManagementViewPageFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementViewPageFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementViewFeatureInfo;
	sensorManagementViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementViewFeatureInfo->SetFeatureId("View");
	sensorManagementViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Sensors"));
	sensorManagementViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Sensors."));
	sensorManagementViewFeatureInfo->SetOptional(false);
	sensorManagementViewFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementViewAllFeatureInfo;
	sensorManagementViewAllFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementViewAllFeatureInfo->SetFeatureId("ViewAll");
	sensorManagementViewAllFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Sensors"));
	sensorManagementViewAllFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing all Sensors."));
	sensorManagementViewAllFeatureInfo->SetOptional(false);
	sensorManagementViewAllFeatureInfo->SetIsPermission(true);
	sensorManagementViewAllFeatureInfo->SetRequirements(QByteArray("/SensorManagement/View").split(';'));

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementViewAllFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementViewHistoryFeatureInfo;
	sensorManagementViewHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementViewHistoryFeatureInfo->SetFeatureId("ViewHistory");
	sensorManagementViewHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Sensor History"));
	sensorManagementViewHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Sensor History."));
	sensorManagementViewHistoryFeatureInfo->SetOptional(false);
	sensorManagementViewHistoryFeatureInfo->SetIsPermission(true);
	sensorManagementViewHistoryFeatureInfo->SetRequirements(QByteArray("/SensorManagement/View").split(';'));

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementViewHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementBindFeatureInfo;
	sensorManagementBindFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementBindFeatureInfo->SetFeatureId("Bind");
	sensorManagementBindFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Bind Sensor"));
	sensorManagementBindFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows binding Sensor."));
	sensorManagementBindFeatureInfo->SetOptional(false);
	sensorManagementBindFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementBindFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementUnbindFeatureInfo;
	sensorManagementUnbindFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementUnbindFeatureInfo->SetFeatureId("Unbind");
	sensorManagementUnbindFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Unbind Sensor"));
	sensorManagementUnbindFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows unbinding Sensor."));
	sensorManagementUnbindFeatureInfo->SetOptional(false);
	sensorManagementUnbindFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementUnbindFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementCreateLicenseFileFeatureInfo;
	sensorManagementCreateLicenseFileFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementCreateLicenseFileFeatureInfo->SetFeatureId("CreateLicenseFile");
	sensorManagementCreateLicenseFileFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Create License File"));
	sensorManagementCreateLicenseFileFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows creating License File."));
	sensorManagementCreateLicenseFileFeatureInfo->SetOptional(false);
	sensorManagementCreateLicenseFileFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementCreateLicenseFileFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementDecryptLicenseFileFeatureInfo;
	sensorManagementDecryptLicenseFileFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementDecryptLicenseFileFeatureInfo->SetFeatureId("DecryptLicenseFile");
	sensorManagementDecryptLicenseFileFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Decrypt License File"));
	sensorManagementDecryptLicenseFileFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows decrypting a license file."));
	sensorManagementDecryptLicenseFileFeatureInfo->SetOptional(false);
	sensorManagementDecryptLicenseFileFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementDecryptLicenseFileFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementTransferLicensesFeatureInfo;
	sensorManagementTransferLicensesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementTransferLicensesFeatureInfo->SetFeatureId("TransferLicenses");
	sensorManagementTransferLicensesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Transfer Licenses"));
	sensorManagementTransferLicensesFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows transferring Licenses."));
	sensorManagementTransferLicensesFeatureInfo->SetOptional(false);
	sensorManagementTransferLicensesFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementTransferLicensesFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementResetTransferCounterFeatureInfo;
	sensorManagementResetTransferCounterFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementResetTransferCounterFeatureInfo->SetFeatureId("ResetTransferCounter");
	sensorManagementResetTransferCounterFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Reset Transfer Counter"));
	sensorManagementResetTransferCounterFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows resetting Transfer Counter."));
	sensorManagementResetTransferCounterFeatureInfo->SetOptional(false);
	sensorManagementResetTransferCounterFeatureInfo->SetIsPermission(true);

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementResetTransferCounterFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditFeatureInfo;
	sensorManagementEditFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditFeatureInfo->SetFeatureId("Edit");
	sensorManagementEditFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Sensor"));
	sensorManagementEditFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Sensor."));
	sensorManagementEditFeatureInfo->SetOptional(false);
	sensorManagementEditFeatureInfo->SetIsPermission(true);
	sensorManagementEditFeatureInfo->SetRequirements(QByteArray("/SensorManagement/View").split(';'));

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditAddFeatureInfo;
	sensorManagementEditAddFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditAddFeatureInfo->SetFeatureId("Add");
	sensorManagementEditAddFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Sensor"));
	sensorManagementEditAddFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Sensor."));
	sensorManagementEditAddFeatureInfo->SetOptional(false);
	sensorManagementEditAddFeatureInfo->SetIsPermission(true);

	sensorManagementEditFeatureInfo->InsertSubFeature(sensorManagementEditAddFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditRemoveFeatureInfo;
	sensorManagementEditRemoveFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditRemoveFeatureInfo->SetFeatureId("Remove");
	sensorManagementEditRemoveFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Sensor"));
	sensorManagementEditRemoveFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Sensor."));
	sensorManagementEditRemoveFeatureInfo->SetOptional(false);
	sensorManagementEditRemoveFeatureInfo->SetIsPermission(true);

	sensorManagementEditFeatureInfo->InsertSubFeature(sensorManagementEditRemoveFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditChangeFeatureInfo;
	sensorManagementEditChangeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditChangeFeatureInfo->SetFeatureId("Change");
	sensorManagementEditChangeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Sensor"));
	sensorManagementEditChangeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Sensor."));
	sensorManagementEditChangeFeatureInfo->SetOptional(false);
	sensorManagementEditChangeFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditChangeOrderFeatureInfo;
	sensorManagementEditChangeOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditChangeOrderFeatureInfo->SetFeatureId("Order");
	sensorManagementEditChangeOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order"));
	sensorManagementEditChangeOrderFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Order."));
	sensorManagementEditChangeOrderFeatureInfo->SetOptional(false);
	sensorManagementEditChangeOrderFeatureInfo->SetIsPermission(true);

	sensorManagementEditChangeFeatureInfo->InsertSubFeature(sensorManagementEditChangeOrderFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditChangeProductionStatusFeatureInfo;
	sensorManagementEditChangeProductionStatusFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditChangeProductionStatusFeatureInfo->SetFeatureId("ProductionStatus");
	sensorManagementEditChangeProductionStatusFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Production Status"));
	sensorManagementEditChangeProductionStatusFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Production Status."));
	sensorManagementEditChangeProductionStatusFeatureInfo->SetOptional(false);
	sensorManagementEditChangeProductionStatusFeatureInfo->SetIsPermission(true);

	sensorManagementEditChangeFeatureInfo->InsertSubFeature(sensorManagementEditChangeProductionStatusFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditChangeHardwareConfigurationFeatureInfo;
	sensorManagementEditChangeHardwareConfigurationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditChangeHardwareConfigurationFeatureInfo->SetFeatureId("HardwareConfiguration");
	sensorManagementEditChangeHardwareConfigurationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Hardware Configuration"));
	sensorManagementEditChangeHardwareConfigurationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Hardware Configuration."));
	sensorManagementEditChangeHardwareConfigurationFeatureInfo->SetOptional(false);
	sensorManagementEditChangeHardwareConfigurationFeatureInfo->SetIsPermission(true);

	sensorManagementEditChangeFeatureInfo->InsertSubFeature(sensorManagementEditChangeHardwareConfigurationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditChangeDeviceTypeFeatureInfo;
	sensorManagementEditChangeDeviceTypeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditChangeDeviceTypeFeatureInfo->SetFeatureId("DeviceType");
	sensorManagementEditChangeDeviceTypeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Device Type"));
	sensorManagementEditChangeDeviceTypeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Device Type."));
	sensorManagementEditChangeDeviceTypeFeatureInfo->SetOptional(false);
	sensorManagementEditChangeDeviceTypeFeatureInfo->SetIsPermission(true);

	sensorManagementEditChangeFeatureInfo->InsertSubFeature(sensorManagementEditChangeDeviceTypeFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditChangeDescriptionFeatureInfo;
	sensorManagementEditChangeDescriptionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditChangeDescriptionFeatureInfo->SetFeatureId("Description");
	sensorManagementEditChangeDescriptionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Description"));
	sensorManagementEditChangeDescriptionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Description."));
	sensorManagementEditChangeDescriptionFeatureInfo->SetOptional(false);
	sensorManagementEditChangeDescriptionFeatureInfo->SetIsPermission(true);

	sensorManagementEditChangeFeatureInfo->InsertSubFeature(sensorManagementEditChangeDescriptionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditChangeSerialNumberFeatureInfo;
	sensorManagementEditChangeSerialNumberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditChangeSerialNumberFeatureInfo->SetFeatureId("SerialNumber");
	sensorManagementEditChangeSerialNumberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Serial Number"));
	sensorManagementEditChangeSerialNumberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Serial Number."));
	sensorManagementEditChangeSerialNumberFeatureInfo->SetOptional(false);
	sensorManagementEditChangeSerialNumberFeatureInfo->SetIsPermission(true);

	sensorManagementEditChangeFeatureInfo->InsertSubFeature(sensorManagementEditChangeSerialNumberFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditChangeMacAddressFeatureInfo;
	sensorManagementEditChangeMacAddressFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditChangeMacAddressFeatureInfo->SetFeatureId("MacAddress");
	sensorManagementEditChangeMacAddressFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Mac Address"));
	sensorManagementEditChangeMacAddressFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Mac Address."));
	sensorManagementEditChangeMacAddressFeatureInfo->SetOptional(false);
	sensorManagementEditChangeMacAddressFeatureInfo->SetIsPermission(true);

	sensorManagementEditChangeFeatureInfo->InsertSubFeature(sensorManagementEditChangeMacAddressFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementEditChangeProjectFeatureInfo;
	sensorManagementEditChangeProjectFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementEditChangeProjectFeatureInfo->SetFeatureId("Project");
	sensorManagementEditChangeProjectFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Project"));
	sensorManagementEditChangeProjectFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Project."));
	sensorManagementEditChangeProjectFeatureInfo->SetOptional(false);
	sensorManagementEditChangeProjectFeatureInfo->SetIsPermission(true);

	sensorManagementEditChangeFeatureInfo->InsertSubFeature(sensorManagementEditChangeProjectFeatureInfo.PopPtr());

	sensorManagementEditFeatureInfo->InsertSubFeature(sensorManagementEditChangeFeatureInfo.PopPtr());

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementEditFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementShareFeatureInfo;
	sensorManagementShareFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementShareFeatureInfo->SetFeatureId("Share");
	sensorManagementShareFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Share Sensors"));
	sensorManagementShareFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows sharing Sensors with related organizations."));
	sensorManagementShareFeatureInfo->SetOptional(false);
	sensorManagementShareFeatureInfo->SetIsPermission(true);
	sensorManagementShareFeatureInfo->SetRequirements(QByteArray("/SensorManagement/View").split(';'));

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementShareFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementRevisionsFeatureInfo;
	sensorManagementRevisionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementRevisionsFeatureInfo->SetFeatureId("Revisions");
	sensorManagementRevisionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Sensor Revisions"));
	sensorManagementRevisionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to the revisions of sensor."));
	sensorManagementRevisionsFeatureInfo->SetOptional(false);
	sensorManagementRevisionsFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementRevisionsViewFeatureInfo;
	sensorManagementRevisionsViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementRevisionsViewFeatureInfo->SetFeatureId("View");
	sensorManagementRevisionsViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Sensor Revisions"));
	sensorManagementRevisionsViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing the revisions of sensor."));
	sensorManagementRevisionsViewFeatureInfo->SetOptional(false);
	sensorManagementRevisionsViewFeatureInfo->SetIsPermission(true);
	sensorManagementRevisionsViewFeatureInfo->SetRequirements(QByteArray("/SensorManagement/View").split(';'));

	sensorManagementRevisionsFeatureInfo->InsertSubFeature(sensorManagementRevisionsViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementRevisionsRestoreFeatureInfo;
	sensorManagementRevisionsRestoreFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementRevisionsRestoreFeatureInfo->SetFeatureId("Restore");
	sensorManagementRevisionsRestoreFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Restore Sensor Revision"));
	sensorManagementRevisionsRestoreFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows restoring a revision of sensor."));
	sensorManagementRevisionsRestoreFeatureInfo->SetOptional(false);
	sensorManagementRevisionsRestoreFeatureInfo->SetIsPermission(true);
	sensorManagementRevisionsRestoreFeatureInfo->SetRequirements(QByteArray("/SensorManagement/Revisions/View").split(';'));

	sensorManagementRevisionsFeatureInfo->InsertSubFeature(sensorManagementRevisionsRestoreFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> sensorManagementRevisionsDeleteFeatureInfo;
	sensorManagementRevisionsDeleteFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	sensorManagementRevisionsDeleteFeatureInfo->SetFeatureId("Delete");
	sensorManagementRevisionsDeleteFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Delete Sensor Revision"));
	sensorManagementRevisionsDeleteFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows deleting a revision of sensor."));
	sensorManagementRevisionsDeleteFeatureInfo->SetOptional(false);
	sensorManagementRevisionsDeleteFeatureInfo->SetIsPermission(true);
	sensorManagementRevisionsDeleteFeatureInfo->SetRequirements(QByteArray("/SensorManagement/Revisions/View").split(';'));

	sensorManagementRevisionsFeatureInfo->InsertSubFeature(sensorManagementRevisionsDeleteFeatureInfo.PopPtr());

	sensorManagementFeatureInfo->InsertSubFeature(sensorManagementRevisionsFeatureInfo.PopPtr());

	productInfo.AddFeature("06d570ef-170c-48fc-bf81-fab890b8d9da", *sensorManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> orderManagementFeatureInfo;
	orderManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	orderManagementFeatureInfo->SetObjectUuid("0b920f33-dad1-43c8-b885-a01860dac5dd");
	orderManagementFeatureInfo->SetFeatureId("OrderManagement");
	orderManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Order Management"));
	orderManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Order-related operations."));
	orderManagementFeatureInfo->SetOptional(false);
	orderManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementViewPageFeatureInfo;
	orderManagementViewPageFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementViewPageFeatureInfo->SetFeatureId("ViewPage");
	orderManagementViewPageFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Order Page"));
	orderManagementViewPageFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows opening the order page."));
	orderManagementViewPageFeatureInfo->SetOptional(false);
	orderManagementViewPageFeatureInfo->SetIsPermission(true);

	orderManagementFeatureInfo->InsertSubFeature(orderManagementViewPageFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementViewFeatureInfo;
	orderManagementViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementViewFeatureInfo->SetFeatureId("View");
	orderManagementViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Orders"));
	orderManagementViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Orders."));
	orderManagementViewFeatureInfo->SetOptional(false);
	orderManagementViewFeatureInfo->SetIsPermission(true);

	orderManagementFeatureInfo->InsertSubFeature(orderManagementViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementViewAllFeatureInfo;
	orderManagementViewAllFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementViewAllFeatureInfo->SetFeatureId("ViewAll");
	orderManagementViewAllFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Orders"));
	orderManagementViewAllFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing all Orders."));
	orderManagementViewAllFeatureInfo->SetOptional(false);
	orderManagementViewAllFeatureInfo->SetIsPermission(true);
	orderManagementViewAllFeatureInfo->SetRequirements(QByteArray("/OrderManagement/View").split(';'));

	orderManagementFeatureInfo->InsertSubFeature(orderManagementViewAllFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementViewHistoryFeatureInfo;
	orderManagementViewHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementViewHistoryFeatureInfo->SetFeatureId("ViewHistory");
	orderManagementViewHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Order History"));
	orderManagementViewHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Order History."));
	orderManagementViewHistoryFeatureInfo->SetOptional(false);
	orderManagementViewHistoryFeatureInfo->SetIsPermission(true);
	orderManagementViewHistoryFeatureInfo->SetRequirements(QByteArray("/OrderManagement/View").split(';'));

	orderManagementFeatureInfo->InsertSubFeature(orderManagementViewHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementEditFeatureInfo;
	orderManagementEditFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementEditFeatureInfo->SetFeatureId("Edit");
	orderManagementEditFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Order"));
	orderManagementEditFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Order."));
	orderManagementEditFeatureInfo->SetOptional(false);
	orderManagementEditFeatureInfo->SetIsPermission(true);
	orderManagementEditFeatureInfo->SetRequirements(QByteArray("/OrderManagement/View").split(';'));

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementEditAddFeatureInfo;
	orderManagementEditAddFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementEditAddFeatureInfo->SetFeatureId("Add");
	orderManagementEditAddFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Order"));
	orderManagementEditAddFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Order."));
	orderManagementEditAddFeatureInfo->SetOptional(false);
	orderManagementEditAddFeatureInfo->SetIsPermission(true);

	orderManagementEditFeatureInfo->InsertSubFeature(orderManagementEditAddFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementEditRemoveFeatureInfo;
	orderManagementEditRemoveFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementEditRemoveFeatureInfo->SetFeatureId("Remove");
	orderManagementEditRemoveFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Order"));
	orderManagementEditRemoveFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Order."));
	orderManagementEditRemoveFeatureInfo->SetOptional(false);
	orderManagementEditRemoveFeatureInfo->SetIsPermission(true);

	orderManagementEditFeatureInfo->InsertSubFeature(orderManagementEditRemoveFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementEditChangeFeatureInfo;
	orderManagementEditChangeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementEditChangeFeatureInfo->SetFeatureId("Change");
	orderManagementEditChangeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order"));
	orderManagementEditChangeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Order."));
	orderManagementEditChangeFeatureInfo->SetOptional(false);
	orderManagementEditChangeFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementEditChangeProductsFeatureInfo;
	orderManagementEditChangeProductsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementEditChangeProductsFeatureInfo->SetFeatureId("Products");
	orderManagementEditChangeProductsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order Products"));
	orderManagementEditChangeProductsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Order Products."));
	orderManagementEditChangeProductsFeatureInfo->SetOptional(false);
	orderManagementEditChangeProductsFeatureInfo->SetIsPermission(true);

	orderManagementEditChangeFeatureInfo->InsertSubFeature(orderManagementEditChangeProductsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementEditChangeStatusFeatureInfo;
	orderManagementEditChangeStatusFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementEditChangeStatusFeatureInfo->SetFeatureId("Status");
	orderManagementEditChangeStatusFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order Status"));
	orderManagementEditChangeStatusFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Order Status."));
	orderManagementEditChangeStatusFeatureInfo->SetOptional(false);
	orderManagementEditChangeStatusFeatureInfo->SetIsPermission(true);

	orderManagementEditChangeFeatureInfo->InsertSubFeature(orderManagementEditChangeStatusFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementEditChangeCustomerFeatureInfo;
	orderManagementEditChangeCustomerFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementEditChangeCustomerFeatureInfo->SetFeatureId("Customer");
	orderManagementEditChangeCustomerFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Customer"));
	orderManagementEditChangeCustomerFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Customer."));
	orderManagementEditChangeCustomerFeatureInfo->SetOptional(false);
	orderManagementEditChangeCustomerFeatureInfo->SetIsPermission(true);

	orderManagementEditChangeFeatureInfo->InsertSubFeature(orderManagementEditChangeCustomerFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementEditChangeDescriptionFeatureInfo;
	orderManagementEditChangeDescriptionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementEditChangeDescriptionFeatureInfo->SetFeatureId("Description");
	orderManagementEditChangeDescriptionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Description"));
	orderManagementEditChangeDescriptionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Description."));
	orderManagementEditChangeDescriptionFeatureInfo->SetOptional(false);
	orderManagementEditChangeDescriptionFeatureInfo->SetIsPermission(true);

	orderManagementEditChangeFeatureInfo->InsertSubFeature(orderManagementEditChangeDescriptionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementEditChangePurchaseOrderIdFeatureInfo;
	orderManagementEditChangePurchaseOrderIdFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementEditChangePurchaseOrderIdFeatureInfo->SetFeatureId("PurchaseOrderId");
	orderManagementEditChangePurchaseOrderIdFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Purchase Order-ID"));
	orderManagementEditChangePurchaseOrderIdFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Purchase Order-ID."));
	orderManagementEditChangePurchaseOrderIdFeatureInfo->SetOptional(false);
	orderManagementEditChangePurchaseOrderIdFeatureInfo->SetIsPermission(true);

	orderManagementEditChangeFeatureInfo->InsertSubFeature(orderManagementEditChangePurchaseOrderIdFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementEditChangeDeliveryIdFeatureInfo;
	orderManagementEditChangeDeliveryIdFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementEditChangeDeliveryIdFeatureInfo->SetFeatureId("DeliveryId");
	orderManagementEditChangeDeliveryIdFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Delivery-ID"));
	orderManagementEditChangeDeliveryIdFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Delivery-ID."));
	orderManagementEditChangeDeliveryIdFeatureInfo->SetOptional(false);
	orderManagementEditChangeDeliveryIdFeatureInfo->SetIsPermission(true);

	orderManagementEditChangeFeatureInfo->InsertSubFeature(orderManagementEditChangeDeliveryIdFeatureInfo.PopPtr());

	orderManagementEditFeatureInfo->InsertSubFeature(orderManagementEditChangeFeatureInfo.PopPtr());

	orderManagementFeatureInfo->InsertSubFeature(orderManagementEditFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementRevisionsFeatureInfo;
	orderManagementRevisionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementRevisionsFeatureInfo->SetFeatureId("Revisions");
	orderManagementRevisionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Order Revisions"));
	orderManagementRevisionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to the revisions of order."));
	orderManagementRevisionsFeatureInfo->SetOptional(false);
	orderManagementRevisionsFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementRevisionsViewFeatureInfo;
	orderManagementRevisionsViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementRevisionsViewFeatureInfo->SetFeatureId("View");
	orderManagementRevisionsViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Order Revisions"));
	orderManagementRevisionsViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing the revisions of order."));
	orderManagementRevisionsViewFeatureInfo->SetOptional(false);
	orderManagementRevisionsViewFeatureInfo->SetIsPermission(true);
	orderManagementRevisionsViewFeatureInfo->SetRequirements(QByteArray("/OrderManagement/View").split(';'));

	orderManagementRevisionsFeatureInfo->InsertSubFeature(orderManagementRevisionsViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementRevisionsRestoreFeatureInfo;
	orderManagementRevisionsRestoreFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementRevisionsRestoreFeatureInfo->SetFeatureId("Restore");
	orderManagementRevisionsRestoreFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Restore Order Revision"));
	orderManagementRevisionsRestoreFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows restoring a revision of order."));
	orderManagementRevisionsRestoreFeatureInfo->SetOptional(false);
	orderManagementRevisionsRestoreFeatureInfo->SetIsPermission(true);
	orderManagementRevisionsRestoreFeatureInfo->SetRequirements(QByteArray("/OrderManagement/Revisions/View").split(';'));

	orderManagementRevisionsFeatureInfo->InsertSubFeature(orderManagementRevisionsRestoreFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> orderManagementRevisionsDeleteFeatureInfo;
	orderManagementRevisionsDeleteFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	orderManagementRevisionsDeleteFeatureInfo->SetFeatureId("Delete");
	orderManagementRevisionsDeleteFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Delete Order Revision"));
	orderManagementRevisionsDeleteFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows deleting a revision of order."));
	orderManagementRevisionsDeleteFeatureInfo->SetOptional(false);
	orderManagementRevisionsDeleteFeatureInfo->SetIsPermission(true);
	orderManagementRevisionsDeleteFeatureInfo->SetRequirements(QByteArray("/OrderManagement/Revisions/View").split(';'));

	orderManagementRevisionsFeatureInfo->InsertSubFeature(orderManagementRevisionsDeleteFeatureInfo.PopPtr());

	orderManagementFeatureInfo->InsertSubFeature(orderManagementRevisionsFeatureInfo.PopPtr());

	productInfo.AddFeature("0b920f33-dad1-43c8-b885-a01860dac5dd", *orderManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> licenseManagementFeatureInfo;
	licenseManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	licenseManagementFeatureInfo->SetObjectUuid("9225718e-3c9c-4e00-83d8-b39152e32a66");
	licenseManagementFeatureInfo->SetFeatureId("LicenseManagement");
	licenseManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "License Management"));
	licenseManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to License-related operations."));
	licenseManagementFeatureInfo->SetOptional(false);
	licenseManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementViewPageFeatureInfo;
	licenseManagementViewPageFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementViewPageFeatureInfo->SetFeatureId("ViewPage");
	licenseManagementViewPageFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View License Page"));
	licenseManagementViewPageFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows opening the license page."));
	licenseManagementViewPageFeatureInfo->SetOptional(false);
	licenseManagementViewPageFeatureInfo->SetIsPermission(true);

	licenseManagementFeatureInfo->InsertSubFeature(licenseManagementViewPageFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementViewFeatureInfo;
	licenseManagementViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementViewFeatureInfo->SetFeatureId("View");
	licenseManagementViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Licenses"));
	licenseManagementViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Licenses."));
	licenseManagementViewFeatureInfo->SetOptional(false);
	licenseManagementViewFeatureInfo->SetIsPermission(true);

	licenseManagementFeatureInfo->InsertSubFeature(licenseManagementViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementViewAllFeatureInfo;
	licenseManagementViewAllFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementViewAllFeatureInfo->SetFeatureId("ViewAll");
	licenseManagementViewAllFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Licenses"));
	licenseManagementViewAllFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing all Licenses."));
	licenseManagementViewAllFeatureInfo->SetOptional(false);
	licenseManagementViewAllFeatureInfo->SetIsPermission(true);
	licenseManagementViewAllFeatureInfo->SetRequirements(QByteArray("/LicenseManagement/View").split(';'));

	licenseManagementFeatureInfo->InsertSubFeature(licenseManagementViewAllFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementViewHistoryFeatureInfo;
	licenseManagementViewHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementViewHistoryFeatureInfo->SetFeatureId("ViewHistory");
	licenseManagementViewHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View License History"));
	licenseManagementViewHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing License History."));
	licenseManagementViewHistoryFeatureInfo->SetOptional(false);
	licenseManagementViewHistoryFeatureInfo->SetIsPermission(true);
	licenseManagementViewHistoryFeatureInfo->SetRequirements(QByteArray("/LicenseManagement/View").split(';'));

	licenseManagementFeatureInfo->InsertSubFeature(licenseManagementViewHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditFeatureInfo;
	licenseManagementEditFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditFeatureInfo->SetFeatureId("Edit");
	licenseManagementEditFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit License"));
	licenseManagementEditFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing License."));
	licenseManagementEditFeatureInfo->SetOptional(false);
	licenseManagementEditFeatureInfo->SetIsPermission(true);
	licenseManagementEditFeatureInfo->SetRequirements(QByteArray("/LicenseManagement/View").split(';'));

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditAddFeatureInfo;
	licenseManagementEditAddFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditAddFeatureInfo->SetFeatureId("Add");
	licenseManagementEditAddFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add License"));
	licenseManagementEditAddFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding License."));
	licenseManagementEditAddFeatureInfo->SetOptional(false);
	licenseManagementEditAddFeatureInfo->SetIsPermission(true);

	licenseManagementEditFeatureInfo->InsertSubFeature(licenseManagementEditAddFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditRemoveFeatureInfo;
	licenseManagementEditRemoveFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditRemoveFeatureInfo->SetFeatureId("Remove");
	licenseManagementEditRemoveFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove License"));
	licenseManagementEditRemoveFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing License."));
	licenseManagementEditRemoveFeatureInfo->SetOptional(false);
	licenseManagementEditRemoveFeatureInfo->SetIsPermission(true);

	licenseManagementEditFeatureInfo->InsertSubFeature(licenseManagementEditRemoveFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditSplitFeatureInfo;
	licenseManagementEditSplitFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditSplitFeatureInfo->SetFeatureId("Split");
	licenseManagementEditSplitFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Split License"));
	licenseManagementEditSplitFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows splitting a license."));
	licenseManagementEditSplitFeatureInfo->SetOptional(false);
	licenseManagementEditSplitFeatureInfo->SetIsPermission(true);

	licenseManagementEditFeatureInfo->InsertSubFeature(licenseManagementEditSplitFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditRevokeFeatureInfo;
	licenseManagementEditRevokeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditRevokeFeatureInfo->SetFeatureId("Revoke");
	licenseManagementEditRevokeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Revoke License"));
	licenseManagementEditRevokeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows revoking a license."));
	licenseManagementEditRevokeFeatureInfo->SetOptional(false);
	licenseManagementEditRevokeFeatureInfo->SetIsPermission(true);

	licenseManagementEditFeatureInfo->InsertSubFeature(licenseManagementEditRevokeFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditChangeFeatureInfo;
	licenseManagementEditChangeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditChangeFeatureInfo->SetFeatureId("Change");
	licenseManagementEditChangeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change License"));
	licenseManagementEditChangeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing License."));
	licenseManagementEditChangeFeatureInfo->SetOptional(false);
	licenseManagementEditChangeFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditChangeNumberFeatureInfo;
	licenseManagementEditChangeNumberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditChangeNumberFeatureInfo->SetFeatureId("Number");
	licenseManagementEditChangeNumberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change License Number"));
	licenseManagementEditChangeNumberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing License Number."));
	licenseManagementEditChangeNumberFeatureInfo->SetOptional(false);
	licenseManagementEditChangeNumberFeatureInfo->SetIsPermission(true);

	licenseManagementEditChangeFeatureInfo->InsertSubFeature(licenseManagementEditChangeNumberFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditChangeExpirationFeatureInfo;
	licenseManagementEditChangeExpirationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditChangeExpirationFeatureInfo->SetFeatureId("Expiration");
	licenseManagementEditChangeExpirationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Expiration"));
	licenseManagementEditChangeExpirationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Expiration."));
	licenseManagementEditChangeExpirationFeatureInfo->SetOptional(false);
	licenseManagementEditChangeExpirationFeatureInfo->SetIsPermission(true);

	licenseManagementEditChangeFeatureInfo->InsertSubFeature(licenseManagementEditChangeExpirationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditChangeProductLicensesFeatureInfo;
	licenseManagementEditChangeProductLicensesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditChangeProductLicensesFeatureInfo->SetFeatureId("ProductLicenses");
	licenseManagementEditChangeProductLicensesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Product Licenses"));
	licenseManagementEditChangeProductLicensesFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Product Licenses."));
	licenseManagementEditChangeProductLicensesFeatureInfo->SetOptional(false);
	licenseManagementEditChangeProductLicensesFeatureInfo->SetIsPermission(true);

	licenseManagementEditChangeFeatureInfo->InsertSubFeature(licenseManagementEditChangeProductLicensesFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditChangeProductFeatureInfo;
	licenseManagementEditChangeProductFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditChangeProductFeatureInfo->SetFeatureId("Product");
	licenseManagementEditChangeProductFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Product"));
	licenseManagementEditChangeProductFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Product."));
	licenseManagementEditChangeProductFeatureInfo->SetOptional(false);
	licenseManagementEditChangeProductFeatureInfo->SetIsPermission(true);

	licenseManagementEditChangeFeatureInfo->InsertSubFeature(licenseManagementEditChangeProductFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditChangeOrderFeatureInfo;
	licenseManagementEditChangeOrderFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditChangeOrderFeatureInfo->SetFeatureId("Order");
	licenseManagementEditChangeOrderFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Order"));
	licenseManagementEditChangeOrderFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Order."));
	licenseManagementEditChangeOrderFeatureInfo->SetOptional(false);
	licenseManagementEditChangeOrderFeatureInfo->SetIsPermission(true);

	licenseManagementEditChangeFeatureInfo->InsertSubFeature(licenseManagementEditChangeOrderFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementEditChangeProjectFeatureInfo;
	licenseManagementEditChangeProjectFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementEditChangeProjectFeatureInfo->SetFeatureId("Project");
	licenseManagementEditChangeProjectFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Project"));
	licenseManagementEditChangeProjectFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Project."));
	licenseManagementEditChangeProjectFeatureInfo->SetOptional(false);
	licenseManagementEditChangeProjectFeatureInfo->SetIsPermission(true);

	licenseManagementEditChangeFeatureInfo->InsertSubFeature(licenseManagementEditChangeProjectFeatureInfo.PopPtr());

	licenseManagementEditFeatureInfo->InsertSubFeature(licenseManagementEditChangeFeatureInfo.PopPtr());

	licenseManagementFeatureInfo->InsertSubFeature(licenseManagementEditFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementShareFeatureInfo;
	licenseManagementShareFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementShareFeatureInfo->SetFeatureId("Share");
	licenseManagementShareFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Share Licenses"));
	licenseManagementShareFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows sharing Licenses with related organizations."));
	licenseManagementShareFeatureInfo->SetOptional(false);
	licenseManagementShareFeatureInfo->SetIsPermission(true);
	licenseManagementShareFeatureInfo->SetRequirements(QByteArray("/LicenseManagement/View").split(';'));

	licenseManagementFeatureInfo->InsertSubFeature(licenseManagementShareFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementRevisionsFeatureInfo;
	licenseManagementRevisionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementRevisionsFeatureInfo->SetFeatureId("Revisions");
	licenseManagementRevisionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "License Revisions"));
	licenseManagementRevisionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to the revisions of license."));
	licenseManagementRevisionsFeatureInfo->SetOptional(false);
	licenseManagementRevisionsFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementRevisionsViewFeatureInfo;
	licenseManagementRevisionsViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementRevisionsViewFeatureInfo->SetFeatureId("View");
	licenseManagementRevisionsViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View License Revisions"));
	licenseManagementRevisionsViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing the revisions of license."));
	licenseManagementRevisionsViewFeatureInfo->SetOptional(false);
	licenseManagementRevisionsViewFeatureInfo->SetIsPermission(true);
	licenseManagementRevisionsViewFeatureInfo->SetRequirements(QByteArray("/LicenseManagement/View").split(';'));

	licenseManagementRevisionsFeatureInfo->InsertSubFeature(licenseManagementRevisionsViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementRevisionsRestoreFeatureInfo;
	licenseManagementRevisionsRestoreFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementRevisionsRestoreFeatureInfo->SetFeatureId("Restore");
	licenseManagementRevisionsRestoreFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Restore License Revision"));
	licenseManagementRevisionsRestoreFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows restoring a revision of license."));
	licenseManagementRevisionsRestoreFeatureInfo->SetOptional(false);
	licenseManagementRevisionsRestoreFeatureInfo->SetIsPermission(true);
	licenseManagementRevisionsRestoreFeatureInfo->SetRequirements(QByteArray("/LicenseManagement/Revisions/View").split(';'));

	licenseManagementRevisionsFeatureInfo->InsertSubFeature(licenseManagementRevisionsRestoreFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> licenseManagementRevisionsDeleteFeatureInfo;
	licenseManagementRevisionsDeleteFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	licenseManagementRevisionsDeleteFeatureInfo->SetFeatureId("Delete");
	licenseManagementRevisionsDeleteFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Delete License Revision"));
	licenseManagementRevisionsDeleteFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows deleting a revision of license."));
	licenseManagementRevisionsDeleteFeatureInfo->SetOptional(false);
	licenseManagementRevisionsDeleteFeatureInfo->SetIsPermission(true);
	licenseManagementRevisionsDeleteFeatureInfo->SetRequirements(QByteArray("/LicenseManagement/Revisions/View").split(';'));

	licenseManagementRevisionsFeatureInfo->InsertSubFeature(licenseManagementRevisionsDeleteFeatureInfo.PopPtr());

	licenseManagementFeatureInfo->InsertSubFeature(licenseManagementRevisionsFeatureInfo.PopPtr());

	productInfo.AddFeature("9225718e-3c9c-4e00-83d8-b39152e32a66", *licenseManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> accountManagementFeatureInfo;
	accountManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	accountManagementFeatureInfo->SetObjectUuid("d9372bb1-6d29-4531-ba18-4cf7329d99c8");
	accountManagementFeatureInfo->SetFeatureId("AccountManagement");
	accountManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Account Management"));
	accountManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Account-related operations."));
	accountManagementFeatureInfo->SetOptional(false);
	accountManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementViewPageFeatureInfo;
	accountManagementViewPageFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementViewPageFeatureInfo->SetFeatureId("ViewPage");
	accountManagementViewPageFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Account Page"));
	accountManagementViewPageFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows opening the account page."));
	accountManagementViewPageFeatureInfo->SetOptional(false);
	accountManagementViewPageFeatureInfo->SetIsPermission(true);

	accountManagementFeatureInfo->InsertSubFeature(accountManagementViewPageFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementViewFeatureInfo;
	accountManagementViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementViewFeatureInfo->SetFeatureId("View");
	accountManagementViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Accounts"));
	accountManagementViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Accounts."));
	accountManagementViewFeatureInfo->SetOptional(false);
	accountManagementViewFeatureInfo->SetIsPermission(true);

	accountManagementFeatureInfo->InsertSubFeature(accountManagementViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementViewAllFeatureInfo;
	accountManagementViewAllFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementViewAllFeatureInfo->SetFeatureId("ViewAll");
	accountManagementViewAllFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All Accounts"));
	accountManagementViewAllFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing all Accounts."));
	accountManagementViewAllFeatureInfo->SetOptional(false);
	accountManagementViewAllFeatureInfo->SetIsPermission(true);
	accountManagementViewAllFeatureInfo->SetRequirements(QByteArray("/AccountManagement/View").split(';'));

	accountManagementFeatureInfo->InsertSubFeature(accountManagementViewAllFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementViewHistoryFeatureInfo;
	accountManagementViewHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementViewHistoryFeatureInfo->SetFeatureId("ViewHistory");
	accountManagementViewHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Account History"));
	accountManagementViewHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Account History."));
	accountManagementViewHistoryFeatureInfo->SetOptional(false);
	accountManagementViewHistoryFeatureInfo->SetIsPermission(true);
	accountManagementViewHistoryFeatureInfo->SetRequirements(QByteArray("/AccountManagement/View").split(';'));

	accountManagementFeatureInfo->InsertSubFeature(accountManagementViewHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementEditFeatureInfo;
	accountManagementEditFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementEditFeatureInfo->SetFeatureId("Edit");
	accountManagementEditFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Account"));
	accountManagementEditFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Account."));
	accountManagementEditFeatureInfo->SetOptional(false);
	accountManagementEditFeatureInfo->SetIsPermission(true);
	accountManagementEditFeatureInfo->SetRequirements(QByteArray("/AccountManagement/View").split(';'));

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementEditAddFeatureInfo;
	accountManagementEditAddFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementEditAddFeatureInfo->SetFeatureId("Add");
	accountManagementEditAddFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Account"));
	accountManagementEditAddFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Account."));
	accountManagementEditAddFeatureInfo->SetOptional(false);
	accountManagementEditAddFeatureInfo->SetIsPermission(true);

	accountManagementEditFeatureInfo->InsertSubFeature(accountManagementEditAddFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementEditRemoveFeatureInfo;
	accountManagementEditRemoveFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementEditRemoveFeatureInfo->SetFeatureId("Remove");
	accountManagementEditRemoveFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Account"));
	accountManagementEditRemoveFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Account."));
	accountManagementEditRemoveFeatureInfo->SetOptional(false);
	accountManagementEditRemoveFeatureInfo->SetIsPermission(true);

	accountManagementEditFeatureInfo->InsertSubFeature(accountManagementEditRemoveFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementEditChangeFeatureInfo;
	accountManagementEditChangeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementEditChangeFeatureInfo->SetFeatureId("Change");
	accountManagementEditChangeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account"));
	accountManagementEditChangeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Account."));
	accountManagementEditChangeFeatureInfo->SetOptional(false);
	accountManagementEditChangeFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementEditChangeNameFeatureInfo;
	accountManagementEditChangeNameFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementEditChangeNameFeatureInfo->SetFeatureId("Name");
	accountManagementEditChangeNameFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Name"));
	accountManagementEditChangeNameFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Account Name."));
	accountManagementEditChangeNameFeatureInfo->SetOptional(false);
	accountManagementEditChangeNameFeatureInfo->SetIsPermission(true);

	accountManagementEditChangeFeatureInfo->InsertSubFeature(accountManagementEditChangeNameFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementEditChangeDescriptionFeatureInfo;
	accountManagementEditChangeDescriptionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementEditChangeDescriptionFeatureInfo->SetFeatureId("Description");
	accountManagementEditChangeDescriptionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Description"));
	accountManagementEditChangeDescriptionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Account Description."));
	accountManagementEditChangeDescriptionFeatureInfo->SetOptional(false);
	accountManagementEditChangeDescriptionFeatureInfo->SetIsPermission(true);

	accountManagementEditChangeFeatureInfo->InsertSubFeature(accountManagementEditChangeDescriptionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementEditChangeEmailFeatureInfo;
	accountManagementEditChangeEmailFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementEditChangeEmailFeatureInfo->SetFeatureId("Email");
	accountManagementEditChangeEmailFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Email"));
	accountManagementEditChangeEmailFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Account Email."));
	accountManagementEditChangeEmailFeatureInfo->SetOptional(false);
	accountManagementEditChangeEmailFeatureInfo->SetIsPermission(true);

	accountManagementEditChangeFeatureInfo->InsertSubFeature(accountManagementEditChangeEmailFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementEditChangeCompanyAddressFeatureInfo;
	accountManagementEditChangeCompanyAddressFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementEditChangeCompanyAddressFeatureInfo->SetFeatureId("CompanyAddress");
	accountManagementEditChangeCompanyAddressFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Company Address"));
	accountManagementEditChangeCompanyAddressFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Company Address."));
	accountManagementEditChangeCompanyAddressFeatureInfo->SetOptional(false);
	accountManagementEditChangeCompanyAddressFeatureInfo->SetIsPermission(true);

	accountManagementEditChangeFeatureInfo->InsertSubFeature(accountManagementEditChangeCompanyAddressFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementEditChangeCustomerIdFeatureInfo;
	accountManagementEditChangeCustomerIdFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementEditChangeCustomerIdFeatureInfo->SetFeatureId("CustomerId");
	accountManagementEditChangeCustomerIdFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Customer-ID"));
	accountManagementEditChangeCustomerIdFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Customer-ID."));
	accountManagementEditChangeCustomerIdFeatureInfo->SetOptional(false);
	accountManagementEditChangeCustomerIdFeatureInfo->SetIsPermission(true);

	accountManagementEditChangeFeatureInfo->InsertSubFeature(accountManagementEditChangeCustomerIdFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementEditChangeGroupsFeatureInfo;
	accountManagementEditChangeGroupsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementEditChangeGroupsFeatureInfo->SetFeatureId("Groups");
	accountManagementEditChangeGroupsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Account Groups"));
	accountManagementEditChangeGroupsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Account Groups."));
	accountManagementEditChangeGroupsFeatureInfo->SetOptional(false);
	accountManagementEditChangeGroupsFeatureInfo->SetIsPermission(true);

	accountManagementEditChangeFeatureInfo->InsertSubFeature(accountManagementEditChangeGroupsFeatureInfo.PopPtr());

	accountManagementEditFeatureInfo->InsertSubFeature(accountManagementEditChangeFeatureInfo.PopPtr());

	accountManagementFeatureInfo->InsertSubFeature(accountManagementEditFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementShareFeatureInfo;
	accountManagementShareFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementShareFeatureInfo->SetFeatureId("Share");
	accountManagementShareFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Share Accounts"));
	accountManagementShareFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows sharing Accounts with related organizations."));
	accountManagementShareFeatureInfo->SetOptional(false);
	accountManagementShareFeatureInfo->SetIsPermission(true);
	accountManagementShareFeatureInfo->SetRequirements(QByteArray("/AccountManagement/View").split(';'));

	accountManagementFeatureInfo->InsertSubFeature(accountManagementShareFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementRevisionsFeatureInfo;
	accountManagementRevisionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementRevisionsFeatureInfo->SetFeatureId("Revisions");
	accountManagementRevisionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Account Revisions"));
	accountManagementRevisionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to the revisions of account."));
	accountManagementRevisionsFeatureInfo->SetOptional(false);
	accountManagementRevisionsFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementRevisionsViewFeatureInfo;
	accountManagementRevisionsViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementRevisionsViewFeatureInfo->SetFeatureId("View");
	accountManagementRevisionsViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Account Revisions"));
	accountManagementRevisionsViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing the revisions of account."));
	accountManagementRevisionsViewFeatureInfo->SetOptional(false);
	accountManagementRevisionsViewFeatureInfo->SetIsPermission(true);
	accountManagementRevisionsViewFeatureInfo->SetRequirements(QByteArray("/AccountManagement/View").split(';'));

	accountManagementRevisionsFeatureInfo->InsertSubFeature(accountManagementRevisionsViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementRevisionsRestoreFeatureInfo;
	accountManagementRevisionsRestoreFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementRevisionsRestoreFeatureInfo->SetFeatureId("Restore");
	accountManagementRevisionsRestoreFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Restore Account Revision"));
	accountManagementRevisionsRestoreFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows restoring a revision of account."));
	accountManagementRevisionsRestoreFeatureInfo->SetOptional(false);
	accountManagementRevisionsRestoreFeatureInfo->SetIsPermission(true);
	accountManagementRevisionsRestoreFeatureInfo->SetRequirements(QByteArray("/AccountManagement/Revisions/View").split(';'));

	accountManagementRevisionsFeatureInfo->InsertSubFeature(accountManagementRevisionsRestoreFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> accountManagementRevisionsDeleteFeatureInfo;
	accountManagementRevisionsDeleteFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	accountManagementRevisionsDeleteFeatureInfo->SetFeatureId("Delete");
	accountManagementRevisionsDeleteFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Delete Account Revision"));
	accountManagementRevisionsDeleteFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows deleting a revision of account."));
	accountManagementRevisionsDeleteFeatureInfo->SetOptional(false);
	accountManagementRevisionsDeleteFeatureInfo->SetIsPermission(true);
	accountManagementRevisionsDeleteFeatureInfo->SetRequirements(QByteArray("/AccountManagement/Revisions/View").split(';'));

	accountManagementRevisionsFeatureInfo->InsertSubFeature(accountManagementRevisionsDeleteFeatureInfo.PopPtr());

	accountManagementFeatureInfo->InsertSubFeature(accountManagementRevisionsFeatureInfo.PopPtr());

	productInfo.AddFeature("d9372bb1-6d29-4531-ba18-4cf7329d99c8", *accountManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> organizationManagementFeatureInfo;
	organizationManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	organizationManagementFeatureInfo->SetObjectUuid("f1b9f9b0-7d54-4fd1-9d17-8d2f8c5f2cf0");
	organizationManagementFeatureInfo->SetFeatureId("OrganizationManagement");
	organizationManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Organization Management"));
	organizationManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Organization-related operations."));
	organizationManagementFeatureInfo->SetOptional(false);
	organizationManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> organizationManagementViewPageFeatureInfo;
	organizationManagementViewPageFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	organizationManagementViewPageFeatureInfo->SetFeatureId("ViewPage");
	organizationManagementViewPageFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Page"));
	organizationManagementViewPageFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows opening the organization page."));
	organizationManagementViewPageFeatureInfo->SetOptional(false);
	organizationManagementViewPageFeatureInfo->SetIsPermission(true);

	organizationManagementFeatureInfo->InsertSubFeature(organizationManagementViewPageFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> organizationManagementViewFeatureInfo;
	organizationManagementViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	organizationManagementViewFeatureInfo->SetFeatureId("View");
	organizationManagementViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organizations"));
	organizationManagementViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organizations."));
	organizationManagementViewFeatureInfo->SetOptional(false);
	organizationManagementViewFeatureInfo->SetIsPermission(true);

	organizationManagementFeatureInfo->InsertSubFeature(organizationManagementViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> organizationManagementAddFeatureInfo;
	organizationManagementAddFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	organizationManagementAddFeatureInfo->SetFeatureId("Add");
	organizationManagementAddFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Organization"));
	organizationManagementAddFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Organization."));
	organizationManagementAddFeatureInfo->SetOptional(false);
	organizationManagementAddFeatureInfo->SetIsPermission(true);

	organizationManagementFeatureInfo->InsertSubFeature(organizationManagementAddFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> organizationManagementRemoveFeatureInfo;
	organizationManagementRemoveFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	organizationManagementRemoveFeatureInfo->SetFeatureId("Remove");
	organizationManagementRemoveFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Organization"));
	organizationManagementRemoveFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Organization."));
	organizationManagementRemoveFeatureInfo->SetOptional(false);
	organizationManagementRemoveFeatureInfo->SetIsPermission(true);

	organizationManagementFeatureInfo->InsertSubFeature(organizationManagementRemoveFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> organizationManagementRevisionsFeatureInfo;
	organizationManagementRevisionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	organizationManagementRevisionsFeatureInfo->SetFeatureId("Revisions");
	organizationManagementRevisionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Organization Revisions"));
	organizationManagementRevisionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to the revisions of organization."));
	organizationManagementRevisionsFeatureInfo->SetOptional(false);
	organizationManagementRevisionsFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> organizationManagementRevisionsViewFeatureInfo;
	organizationManagementRevisionsViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	organizationManagementRevisionsViewFeatureInfo->SetFeatureId("View");
	organizationManagementRevisionsViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Revisions"));
	organizationManagementRevisionsViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing the revisions of organization."));
	organizationManagementRevisionsViewFeatureInfo->SetOptional(false);
	organizationManagementRevisionsViewFeatureInfo->SetIsPermission(true);
	organizationManagementRevisionsViewFeatureInfo->SetRequirements(QByteArray("/OrganizationManagement/View").split(';'));

	organizationManagementRevisionsFeatureInfo->InsertSubFeature(organizationManagementRevisionsViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> organizationManagementRevisionsRestoreFeatureInfo;
	organizationManagementRevisionsRestoreFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	organizationManagementRevisionsRestoreFeatureInfo->SetFeatureId("Restore");
	organizationManagementRevisionsRestoreFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Restore Organization Revision"));
	organizationManagementRevisionsRestoreFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows restoring a revision of organization."));
	organizationManagementRevisionsRestoreFeatureInfo->SetOptional(false);
	organizationManagementRevisionsRestoreFeatureInfo->SetIsPermission(true);
	organizationManagementRevisionsRestoreFeatureInfo->SetRequirements(QByteArray("/OrganizationManagement/Revisions/View").split(';'));

	organizationManagementRevisionsFeatureInfo->InsertSubFeature(organizationManagementRevisionsRestoreFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> organizationManagementRevisionsDeleteFeatureInfo;
	organizationManagementRevisionsDeleteFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	organizationManagementRevisionsDeleteFeatureInfo->SetFeatureId("Delete");
	organizationManagementRevisionsDeleteFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Delete Organization Revision"));
	organizationManagementRevisionsDeleteFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows deleting a revision of organization."));
	organizationManagementRevisionsDeleteFeatureInfo->SetOptional(false);
	organizationManagementRevisionsDeleteFeatureInfo->SetIsPermission(true);
	organizationManagementRevisionsDeleteFeatureInfo->SetRequirements(QByteArray("/OrganizationManagement/Revisions/View").split(';'));

	organizationManagementRevisionsFeatureInfo->InsertSubFeature(organizationManagementRevisionsDeleteFeatureInfo.PopPtr());

	organizationManagementFeatureInfo->InsertSubFeature(organizationManagementRevisionsFeatureInfo.PopPtr());

	productInfo.AddFeature("f1b9f9b0-7d54-4fd1-9d17-8d2f8c5f2cf0", *organizationManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> searchManagementFeatureInfo;
	searchManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	searchManagementFeatureInfo->SetObjectUuid("3d98c36e-882a-4a5a-9bff-f932c62b3757");
	searchManagementFeatureInfo->SetFeatureId("SearchManagement");
	searchManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Search management"));
	searchManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Search-related operations."));
	searchManagementFeatureInfo->SetOptional(false);
	searchManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> searchManagementViewPageFeatureInfo;
	searchManagementViewPageFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	searchManagementViewPageFeatureInfo->SetFeatureId("ViewPage");
	searchManagementViewPageFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Search Page"));
	searchManagementViewPageFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows opening the search page."));
	searchManagementViewPageFeatureInfo->SetOptional(false);
	searchManagementViewPageFeatureInfo->SetIsPermission(true);

	searchManagementFeatureInfo->InsertSubFeature(searchManagementViewPageFeatureInfo.PopPtr());

	productInfo.AddFeature("3d98c36e-882a-4a5a-9bff-f932c62b3757", *searchManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> administrationFeatureInfo;
	administrationFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	administrationFeatureInfo->SetObjectUuid("ce4a7f72-5303-4044-b25c-3a02020eebd3");
	administrationFeatureInfo->SetFeatureId("Administration");
	administrationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Administration"));
	administrationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to administration pages."));
	administrationFeatureInfo->SetOptional(false);
	administrationFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersFeatureInfo;
	administrationUsersFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersFeatureInfo->SetFeatureId("Users");
	administrationUsersFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Users"));
	administrationUsersFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to the users."));
	administrationUsersFeatureInfo->SetOptional(false);
	administrationUsersFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersViewPageFeatureInfo;
	administrationUsersViewPageFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersViewPageFeatureInfo->SetFeatureId("ViewPage");
	administrationUsersViewPageFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Users Page"));
	administrationUsersViewPageFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows opening the users page."));
	administrationUsersViewPageFeatureInfo->SetOptional(false);
	administrationUsersViewPageFeatureInfo->SetIsPermission(true);

	administrationUsersFeatureInfo->InsertSubFeature(administrationUsersViewPageFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersViewFeatureInfo;
	administrationUsersViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersViewFeatureInfo->SetFeatureId("View");
	administrationUsersViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Users"));
	administrationUsersViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Users."));
	administrationUsersViewFeatureInfo->SetOptional(false);
	administrationUsersViewFeatureInfo->SetIsPermission(true);

	administrationUsersFeatureInfo->InsertSubFeature(administrationUsersViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersViewHistoryFeatureInfo;
	administrationUsersViewHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersViewHistoryFeatureInfo->SetFeatureId("ViewHistory");
	administrationUsersViewHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View User History"));
	administrationUsersViewHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing User History."));
	administrationUsersViewHistoryFeatureInfo->SetOptional(false);
	administrationUsersViewHistoryFeatureInfo->SetIsPermission(true);
	administrationUsersViewHistoryFeatureInfo->SetRequirements(QByteArray("/Administration/Users/View").split(';'));

	administrationUsersFeatureInfo->InsertSubFeature(administrationUsersViewHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersEditFeatureInfo;
	administrationUsersEditFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersEditFeatureInfo->SetFeatureId("Edit");
	administrationUsersEditFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit User"));
	administrationUsersEditFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing User."));
	administrationUsersEditFeatureInfo->SetOptional(false);
	administrationUsersEditFeatureInfo->SetIsPermission(true);
	administrationUsersEditFeatureInfo->SetRequirements(QByteArray("/Administration/Users/View").split(';'));

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersEditAddFeatureInfo;
	administrationUsersEditAddFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersEditAddFeatureInfo->SetFeatureId("Add");
	administrationUsersEditAddFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add User"));
	administrationUsersEditAddFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding User."));
	administrationUsersEditAddFeatureInfo->SetOptional(false);
	administrationUsersEditAddFeatureInfo->SetIsPermission(true);

	administrationUsersEditFeatureInfo->InsertSubFeature(administrationUsersEditAddFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersEditChangeFeatureInfo;
	administrationUsersEditChangeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersEditChangeFeatureInfo->SetFeatureId("Change");
	administrationUsersEditChangeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change User"));
	administrationUsersEditChangeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing User."));
	administrationUsersEditChangeFeatureInfo->SetOptional(false);
	administrationUsersEditChangeFeatureInfo->SetIsPermission(true);

	administrationUsersEditFeatureInfo->InsertSubFeature(administrationUsersEditChangeFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersEditRemoveFeatureInfo;
	administrationUsersEditRemoveFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersEditRemoveFeatureInfo->SetFeatureId("Remove");
	administrationUsersEditRemoveFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove User"));
	administrationUsersEditRemoveFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing User."));
	administrationUsersEditRemoveFeatureInfo->SetOptional(false);
	administrationUsersEditRemoveFeatureInfo->SetIsPermission(true);

	administrationUsersEditFeatureInfo->InsertSubFeature(administrationUsersEditRemoveFeatureInfo.PopPtr());

	administrationUsersFeatureInfo->InsertSubFeature(administrationUsersEditFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersRevisionsFeatureInfo;
	administrationUsersRevisionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersRevisionsFeatureInfo->SetFeatureId("Revisions");
	administrationUsersRevisionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Users Revisions"));
	administrationUsersRevisionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to the revisions of users."));
	administrationUsersRevisionsFeatureInfo->SetOptional(false);
	administrationUsersRevisionsFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersRevisionsViewFeatureInfo;
	administrationUsersRevisionsViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersRevisionsViewFeatureInfo->SetFeatureId("View");
	administrationUsersRevisionsViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Users Revisions"));
	administrationUsersRevisionsViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing the revisions of users."));
	administrationUsersRevisionsViewFeatureInfo->SetOptional(false);
	administrationUsersRevisionsViewFeatureInfo->SetIsPermission(true);
	administrationUsersRevisionsViewFeatureInfo->SetRequirements(QByteArray("/Administration/Users/View").split(';'));

	administrationUsersRevisionsFeatureInfo->InsertSubFeature(administrationUsersRevisionsViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersRevisionsRestoreFeatureInfo;
	administrationUsersRevisionsRestoreFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersRevisionsRestoreFeatureInfo->SetFeatureId("Restore");
	administrationUsersRevisionsRestoreFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Restore Users Revision"));
	administrationUsersRevisionsRestoreFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows restoring a revision of users."));
	administrationUsersRevisionsRestoreFeatureInfo->SetOptional(false);
	administrationUsersRevisionsRestoreFeatureInfo->SetIsPermission(true);
	administrationUsersRevisionsRestoreFeatureInfo->SetRequirements(QByteArray("/Administration/Users/Revisions/View").split(';'));

	administrationUsersRevisionsFeatureInfo->InsertSubFeature(administrationUsersRevisionsRestoreFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationUsersRevisionsDeleteFeatureInfo;
	administrationUsersRevisionsDeleteFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationUsersRevisionsDeleteFeatureInfo->SetFeatureId("Delete");
	administrationUsersRevisionsDeleteFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Delete Users Revision"));
	administrationUsersRevisionsDeleteFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows deleting a revision of users."));
	administrationUsersRevisionsDeleteFeatureInfo->SetOptional(false);
	administrationUsersRevisionsDeleteFeatureInfo->SetIsPermission(true);
	administrationUsersRevisionsDeleteFeatureInfo->SetRequirements(QByteArray("/Administration/Users/Revisions/View").split(';'));

	administrationUsersRevisionsFeatureInfo->InsertSubFeature(administrationUsersRevisionsDeleteFeatureInfo.PopPtr());

	administrationUsersFeatureInfo->InsertSubFeature(administrationUsersRevisionsFeatureInfo.PopPtr());

	administrationFeatureInfo->InsertSubFeature(administrationUsersFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesFeatureInfo;
	administrationRolesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesFeatureInfo->SetFeatureId("Roles");
	administrationRolesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Roles"));
	administrationRolesFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to the roles."));
	administrationRolesFeatureInfo->SetOptional(false);
	administrationRolesFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesViewPageFeatureInfo;
	administrationRolesViewPageFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesViewPageFeatureInfo->SetFeatureId("ViewPage");
	administrationRolesViewPageFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Roles Page"));
	administrationRolesViewPageFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows opening the roles page."));
	administrationRolesViewPageFeatureInfo->SetOptional(false);
	administrationRolesViewPageFeatureInfo->SetIsPermission(true);

	administrationRolesFeatureInfo->InsertSubFeature(administrationRolesViewPageFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesViewFeatureInfo;
	administrationRolesViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesViewFeatureInfo->SetFeatureId("View");
	administrationRolesViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Roles"));
	administrationRolesViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Roles."));
	administrationRolesViewFeatureInfo->SetOptional(false);
	administrationRolesViewFeatureInfo->SetIsPermission(true);

	administrationRolesFeatureInfo->InsertSubFeature(administrationRolesViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesViewHistoryFeatureInfo;
	administrationRolesViewHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesViewHistoryFeatureInfo->SetFeatureId("ViewHistory");
	administrationRolesViewHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Role History"));
	administrationRolesViewHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Role History."));
	administrationRolesViewHistoryFeatureInfo->SetOptional(false);
	administrationRolesViewHistoryFeatureInfo->SetIsPermission(true);
	administrationRolesViewHistoryFeatureInfo->SetRequirements(QByteArray("/Administration/Roles/View").split(';'));

	administrationRolesFeatureInfo->InsertSubFeature(administrationRolesViewHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesEditFeatureInfo;
	administrationRolesEditFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesEditFeatureInfo->SetFeatureId("Edit");
	administrationRolesEditFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Role"));
	administrationRolesEditFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Role."));
	administrationRolesEditFeatureInfo->SetOptional(false);
	administrationRolesEditFeatureInfo->SetIsPermission(true);
	administrationRolesEditFeatureInfo->SetRequirements(QByteArray("/Administration/Roles/View").split(';'));

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesEditAddFeatureInfo;
	administrationRolesEditAddFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesEditAddFeatureInfo->SetFeatureId("Add");
	administrationRolesEditAddFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Role"));
	administrationRolesEditAddFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Role."));
	administrationRolesEditAddFeatureInfo->SetOptional(false);
	administrationRolesEditAddFeatureInfo->SetIsPermission(true);

	administrationRolesEditFeatureInfo->InsertSubFeature(administrationRolesEditAddFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesEditChangeFeatureInfo;
	administrationRolesEditChangeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesEditChangeFeatureInfo->SetFeatureId("Change");
	administrationRolesEditChangeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Role"));
	administrationRolesEditChangeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Role."));
	administrationRolesEditChangeFeatureInfo->SetOptional(false);
	administrationRolesEditChangeFeatureInfo->SetIsPermission(true);

	administrationRolesEditFeatureInfo->InsertSubFeature(administrationRolesEditChangeFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesEditRemoveFeatureInfo;
	administrationRolesEditRemoveFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesEditRemoveFeatureInfo->SetFeatureId("Remove");
	administrationRolesEditRemoveFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Role"));
	administrationRolesEditRemoveFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Role."));
	administrationRolesEditRemoveFeatureInfo->SetOptional(false);
	administrationRolesEditRemoveFeatureInfo->SetIsPermission(true);

	administrationRolesEditFeatureInfo->InsertSubFeature(administrationRolesEditRemoveFeatureInfo.PopPtr());

	administrationRolesFeatureInfo->InsertSubFeature(administrationRolesEditFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesRevisionsFeatureInfo;
	administrationRolesRevisionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesRevisionsFeatureInfo->SetFeatureId("Revisions");
	administrationRolesRevisionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Roles Revisions"));
	administrationRolesRevisionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to the revisions of roles."));
	administrationRolesRevisionsFeatureInfo->SetOptional(false);
	administrationRolesRevisionsFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesRevisionsViewFeatureInfo;
	administrationRolesRevisionsViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesRevisionsViewFeatureInfo->SetFeatureId("View");
	administrationRolesRevisionsViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Roles Revisions"));
	administrationRolesRevisionsViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing the revisions of roles."));
	administrationRolesRevisionsViewFeatureInfo->SetOptional(false);
	administrationRolesRevisionsViewFeatureInfo->SetIsPermission(true);
	administrationRolesRevisionsViewFeatureInfo->SetRequirements(QByteArray("/Administration/Roles/View").split(';'));

	administrationRolesRevisionsFeatureInfo->InsertSubFeature(administrationRolesRevisionsViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesRevisionsRestoreFeatureInfo;
	administrationRolesRevisionsRestoreFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesRevisionsRestoreFeatureInfo->SetFeatureId("Restore");
	administrationRolesRevisionsRestoreFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Restore Roles Revision"));
	administrationRolesRevisionsRestoreFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows restoring a revision of roles."));
	administrationRolesRevisionsRestoreFeatureInfo->SetOptional(false);
	administrationRolesRevisionsRestoreFeatureInfo->SetIsPermission(true);
	administrationRolesRevisionsRestoreFeatureInfo->SetRequirements(QByteArray("/Administration/Roles/Revisions/View").split(';'));

	administrationRolesRevisionsFeatureInfo->InsertSubFeature(administrationRolesRevisionsRestoreFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationRolesRevisionsDeleteFeatureInfo;
	administrationRolesRevisionsDeleteFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationRolesRevisionsDeleteFeatureInfo->SetFeatureId("Delete");
	administrationRolesRevisionsDeleteFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Delete Roles Revision"));
	administrationRolesRevisionsDeleteFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows deleting a revision of roles."));
	administrationRolesRevisionsDeleteFeatureInfo->SetOptional(false);
	administrationRolesRevisionsDeleteFeatureInfo->SetIsPermission(true);
	administrationRolesRevisionsDeleteFeatureInfo->SetRequirements(QByteArray("/Administration/Roles/Revisions/View").split(';'));

	administrationRolesRevisionsFeatureInfo->InsertSubFeature(administrationRolesRevisionsDeleteFeatureInfo.PopPtr());

	administrationRolesFeatureInfo->InsertSubFeature(administrationRolesRevisionsFeatureInfo.PopPtr());

	administrationFeatureInfo->InsertSubFeature(administrationRolesFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsFeatureInfo;
	administrationGroupsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsFeatureInfo->SetFeatureId("Groups");
	administrationGroupsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Groups"));
	administrationGroupsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to the groups."));
	administrationGroupsFeatureInfo->SetOptional(false);
	administrationGroupsFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsViewPageFeatureInfo;
	administrationGroupsViewPageFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsViewPageFeatureInfo->SetFeatureId("ViewPage");
	administrationGroupsViewPageFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Groups Page"));
	administrationGroupsViewPageFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows opening the groups page."));
	administrationGroupsViewPageFeatureInfo->SetOptional(false);
	administrationGroupsViewPageFeatureInfo->SetIsPermission(true);

	administrationGroupsFeatureInfo->InsertSubFeature(administrationGroupsViewPageFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsViewFeatureInfo;
	administrationGroupsViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsViewFeatureInfo->SetFeatureId("View");
	administrationGroupsViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Groups"));
	administrationGroupsViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Groups."));
	administrationGroupsViewFeatureInfo->SetOptional(false);
	administrationGroupsViewFeatureInfo->SetIsPermission(true);

	administrationGroupsFeatureInfo->InsertSubFeature(administrationGroupsViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsViewHistoryFeatureInfo;
	administrationGroupsViewHistoryFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsViewHistoryFeatureInfo->SetFeatureId("ViewHistory");
	administrationGroupsViewHistoryFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Group History"));
	administrationGroupsViewHistoryFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Group History."));
	administrationGroupsViewHistoryFeatureInfo->SetOptional(false);
	administrationGroupsViewHistoryFeatureInfo->SetIsPermission(true);
	administrationGroupsViewHistoryFeatureInfo->SetRequirements(QByteArray("/Administration/Groups/View").split(';'));

	administrationGroupsFeatureInfo->InsertSubFeature(administrationGroupsViewHistoryFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsEditFeatureInfo;
	administrationGroupsEditFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsEditFeatureInfo->SetFeatureId("Edit");
	administrationGroupsEditFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Group"));
	administrationGroupsEditFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Group."));
	administrationGroupsEditFeatureInfo->SetOptional(false);
	administrationGroupsEditFeatureInfo->SetIsPermission(true);
	administrationGroupsEditFeatureInfo->SetRequirements(QByteArray("/Administration/Groups/View").split(';'));

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsEditAddFeatureInfo;
	administrationGroupsEditAddFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsEditAddFeatureInfo->SetFeatureId("Add");
	administrationGroupsEditAddFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Group"));
	administrationGroupsEditAddFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Group."));
	administrationGroupsEditAddFeatureInfo->SetOptional(false);
	administrationGroupsEditAddFeatureInfo->SetIsPermission(true);

	administrationGroupsEditFeatureInfo->InsertSubFeature(administrationGroupsEditAddFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsEditChangeFeatureInfo;
	administrationGroupsEditChangeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsEditChangeFeatureInfo->SetFeatureId("Change");
	administrationGroupsEditChangeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Group"));
	administrationGroupsEditChangeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Group."));
	administrationGroupsEditChangeFeatureInfo->SetOptional(false);
	administrationGroupsEditChangeFeatureInfo->SetIsPermission(true);

	administrationGroupsEditFeatureInfo->InsertSubFeature(administrationGroupsEditChangeFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsEditRemoveFeatureInfo;
	administrationGroupsEditRemoveFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsEditRemoveFeatureInfo->SetFeatureId("Remove");
	administrationGroupsEditRemoveFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Group"));
	administrationGroupsEditRemoveFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Group."));
	administrationGroupsEditRemoveFeatureInfo->SetOptional(false);
	administrationGroupsEditRemoveFeatureInfo->SetIsPermission(true);

	administrationGroupsEditFeatureInfo->InsertSubFeature(administrationGroupsEditRemoveFeatureInfo.PopPtr());

	administrationGroupsFeatureInfo->InsertSubFeature(administrationGroupsEditFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsRevisionsFeatureInfo;
	administrationGroupsRevisionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsRevisionsFeatureInfo->SetFeatureId("Revisions");
	administrationGroupsRevisionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Groups Revisions"));
	administrationGroupsRevisionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to the revisions of groups."));
	administrationGroupsRevisionsFeatureInfo->SetOptional(false);
	administrationGroupsRevisionsFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsRevisionsViewFeatureInfo;
	administrationGroupsRevisionsViewFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsRevisionsViewFeatureInfo->SetFeatureId("View");
	administrationGroupsRevisionsViewFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Groups Revisions"));
	administrationGroupsRevisionsViewFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing the revisions of groups."));
	administrationGroupsRevisionsViewFeatureInfo->SetOptional(false);
	administrationGroupsRevisionsViewFeatureInfo->SetIsPermission(true);
	administrationGroupsRevisionsViewFeatureInfo->SetRequirements(QByteArray("/Administration/Groups/View").split(';'));

	administrationGroupsRevisionsFeatureInfo->InsertSubFeature(administrationGroupsRevisionsViewFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsRevisionsRestoreFeatureInfo;
	administrationGroupsRevisionsRestoreFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsRevisionsRestoreFeatureInfo->SetFeatureId("Restore");
	administrationGroupsRevisionsRestoreFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Restore Groups Revision"));
	administrationGroupsRevisionsRestoreFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows restoring a revision of groups."));
	administrationGroupsRevisionsRestoreFeatureInfo->SetOptional(false);
	administrationGroupsRevisionsRestoreFeatureInfo->SetIsPermission(true);
	administrationGroupsRevisionsRestoreFeatureInfo->SetRequirements(QByteArray("/Administration/Groups/Revisions/View").split(';'));

	administrationGroupsRevisionsFeatureInfo->InsertSubFeature(administrationGroupsRevisionsRestoreFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> administrationGroupsRevisionsDeleteFeatureInfo;
	administrationGroupsRevisionsDeleteFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	administrationGroupsRevisionsDeleteFeatureInfo->SetFeatureId("Delete");
	administrationGroupsRevisionsDeleteFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Delete Groups Revision"));
	administrationGroupsRevisionsDeleteFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows deleting a revision of groups."));
	administrationGroupsRevisionsDeleteFeatureInfo->SetOptional(false);
	administrationGroupsRevisionsDeleteFeatureInfo->SetIsPermission(true);
	administrationGroupsRevisionsDeleteFeatureInfo->SetRequirements(QByteArray("/Administration/Groups/Revisions/View").split(';'));

	administrationGroupsRevisionsFeatureInfo->InsertSubFeature(administrationGroupsRevisionsDeleteFeatureInfo.PopPtr());

	administrationGroupsFeatureInfo->InsertSubFeature(administrationGroupsRevisionsFeatureInfo.PopPtr());

	administrationFeatureInfo->InsertSubFeature(administrationGroupsFeatureInfo.PopPtr());

	productInfo.AddFeature("ce4a7f72-5303-4044-b25c-3a02020eebd3", *administrationFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> workspaceManagementFeatureInfo;
	workspaceManagementFeatureInfo.SetPtr(new imtlic::CIdentifiableFeatureInfo);
	workspaceManagementFeatureInfo->SetObjectUuid("df22ac46-7253-4b13-a1b8-d4391943adde");
	workspaceManagementFeatureInfo->SetFeatureId("WorkspaceManagement");
	workspaceManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Workspace Management"));
	workspaceManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Workspace-related operations."));
	workspaceManagementFeatureInfo->SetOptional(false);
	workspaceManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> workspaceManagementViewPageFeatureInfo;
	workspaceManagementViewPageFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	workspaceManagementViewPageFeatureInfo->SetFeatureId("ViewPage");
	workspaceManagementViewPageFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Workspace Page"));
	workspaceManagementViewPageFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows opening the workspace page."));
	workspaceManagementViewPageFeatureInfo->SetOptional(false);
	workspaceManagementViewPageFeatureInfo->SetIsPermission(true);

	workspaceManagementFeatureInfo->InsertSubFeature(workspaceManagementViewPageFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> workspaceManagementViewUserActionsFeatureInfo;
	workspaceManagementViewUserActionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	workspaceManagementViewUserActionsFeatureInfo->SetFeatureId("ViewUserActions");
	workspaceManagementViewUserActionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View User Actions"));
	workspaceManagementViewUserActionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing User Actions."));
	workspaceManagementViewUserActionsFeatureInfo->SetOptional(false);
	workspaceManagementViewUserActionsFeatureInfo->SetIsPermission(true);

	workspaceManagementFeatureInfo->InsertSubFeature(workspaceManagementViewUserActionsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> workspaceManagementViewAllUserActionsFeatureInfo;
	workspaceManagementViewAllUserActionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	workspaceManagementViewAllUserActionsFeatureInfo->SetFeatureId("ViewAllUserActions");
	workspaceManagementViewAllUserActionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View All User Actions"));
	workspaceManagementViewAllUserActionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing all User Actions."));
	workspaceManagementViewAllUserActionsFeatureInfo->SetOptional(false);
	workspaceManagementViewAllUserActionsFeatureInfo->SetIsPermission(true);
	workspaceManagementViewAllUserActionsFeatureInfo->SetRequirements(QByteArray("/WorkspaceManagement/ViewUserActions").split(';'));

	workspaceManagementFeatureInfo->InsertSubFeature(workspaceManagementViewAllUserActionsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> workspaceManagementViewAnalyticsFeatureInfo;
	workspaceManagementViewAnalyticsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	workspaceManagementViewAnalyticsFeatureInfo->SetFeatureId("ViewAnalytics");
	workspaceManagementViewAnalyticsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Analytics"));
	workspaceManagementViewAnalyticsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Analytics."));
	workspaceManagementViewAnalyticsFeatureInfo->SetOptional(false);
	workspaceManagementViewAnalyticsFeatureInfo->SetIsPermission(true);

	workspaceManagementFeatureInfo->InsertSubFeature(workspaceManagementViewAnalyticsFeatureInfo.PopPtr());

	productInfo.AddFeature("df22ac46-7253-4b13-a1b8-d4391943adde", *workspaceManagementFeatureInfo.GetPtr());

}


};

