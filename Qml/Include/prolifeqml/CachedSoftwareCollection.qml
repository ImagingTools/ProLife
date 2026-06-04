pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import prolifeLicensesSdl 1.0

CollectionDataProvider {
	id: container;
	commandId: ProlifeLicensesSdlCommandIds.s_softwareProductsList;
	subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
	sortByField: SoftwareProductItemTypeMetaInfo.s_name;
	fields: [
		SoftwareProductItemTypeMetaInfo.s_id,
		SoftwareProductItemTypeMetaInfo.s_name,
		SoftwareProductItemTypeMetaInfo.s_productName,
		SoftwareProductItemTypeMetaInfo.s_licenseUuid,
		SoftwareProductItemTypeMetaInfo.s_licenseId,
		SoftwareProductItemTypeMetaInfo.s_licenseName,
		SoftwareProductItemTypeMetaInfo.s_serialNumber,
		SoftwareProductItemTypeMetaInfo.s_productUuid,
		SoftwareProductItemTypeMetaInfo.s_expiration
	];
}


