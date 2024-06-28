import QtQuick 2.15
import Acf 1.0
import imtcolgui 1.0

CollectionFilter {
    id: root;

    function setLicenseFilter(value){
        if (value == ""){
            removeFilterById("LicenseFilter");
        }
        else{
            addAdditionalFilter("LicenseFilter", value);
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

