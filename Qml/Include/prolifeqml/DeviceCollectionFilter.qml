import QtQuick 2.15
import Acf 1.0
import imtcolgui 1.0

CollectionFilter {
    id: root;

    function setDeviceStatusFilter(status){
        if (status == ""){
            removeFilterById("Status");
        }
        else{
            addAdditionalFilter("Status", status);
        }
    }

    function setLicenseFilter(licenseId){
        if (licenseId === ""){
            removeFilterById("LicenseStatus");
        }
        else{
            addAdditionalFilter("LicenseStatus", licenseId);
        }
    }

    function setAccountFilter(accountId){
        if (accountId == ""){
            removeFilterById("CustomerUuid");
        }
        else{
            addAdditionalFilter("CustomerUuid", accountId);
        }
    }
}

