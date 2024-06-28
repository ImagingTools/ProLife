import QtQuick 2.15
import Acf 1.0
import imtcolgui 1.0

CollectionFilter {
    id: root;

    function setAccountFilter(accountId){
        if (accountId === ""){
            removeFilterById("CustomerUuid");
        }
        else{
            addAdditionalFilter("CustomerUuid", accountId);
        }

        filterChanged();
    }
}

