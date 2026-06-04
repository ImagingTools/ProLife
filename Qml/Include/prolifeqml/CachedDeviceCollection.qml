pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import prolifeSensorsSdl 1.0

CollectionDataProvider {
    id: container;
    commandId: ProlifeSensorsSdlCommandIds.s_devicesList;
    subscriptionCommandId: "OnDevicesCollectionChanged"
    sortByField: DeviceItemTypeMetaInfo.s_name;
    fields: [
        DeviceItemTypeMetaInfo.s_id,
        DeviceItemTypeMetaInfo.s_name,
        DeviceItemTypeMetaInfo.s_deviceType,
        DeviceItemTypeMetaInfo.s_status,
        DeviceItemTypeMetaInfo.s_macAddress,
        DeviceItemTypeMetaInfo.s_serialNumber,
        DeviceItemTypeMetaInfo.s_productUuid,
        DeviceItemTypeMetaInfo.s_licenseUuid,
        DeviceItemTypeMetaInfo.s_licenseId,
        DeviceItemTypeMetaInfo.s_licenseName];
}


