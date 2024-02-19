import QtQuick 2.15
import Acf 1.0
import imtcolgui 1.0

CollectionFilter {
    id: root;

    function setLicenseFilter(value){
        let objectFilter = filterModel.GetData("ObjectFilter");
        if (!objectFilter){
            objectFilter = filterModel.AddTreeModel("ObjectFilter")
        }

        objectFilter.SetData("LicenseFilter", value);

        filterChanged();
    }

    function setAccountFilter(accountId){
        let objectFilter = filterModel.GetData("ObjectFilter");
        if (!objectFilter){
            objectFilter = filterModel.AddTreeModel("ObjectFilter")
        }

        objectFilter.SetData("AccountFilter", accountId);

        filterChanged();
    }
}

