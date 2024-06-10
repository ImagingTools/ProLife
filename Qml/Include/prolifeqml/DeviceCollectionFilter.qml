import QtQuick 2.15
import Acf 1.0
import imtcolgui 1.0

CollectionFilter {
    id: root;

    function setDeviceStatusFilter(status){
        if (status === ""){
            filterModel.removeData("ObjectFilter");
        }
        else{
            let objectFilter = filterModel.getData("ObjectFilter");
            if (!objectFilter){
                objectFilter = filterModel.addTreeModel("ObjectFilter")
            }

            objectFilter.setData("Key", "Status");
            objectFilter.setData("Value", status);
        }

        filterChanged();
    }

    function setLicenseFilter(licenseId){
        let licenceFilterModel = filterModel.getData("LicenseFilter")
        if (!licenceFilterModel){
            licenceFilterModel = filterModel.addTreeModel("LicenseFilter")
        }

        licenceFilterModel.setData("Key", "Status");
        licenceFilterModel.setData("Value", licenseId);

        filterChanged();
    }

    function setAccountFilter(accountId){
        if (accountId !== ""){
            let accountFilterModel = filterModel.getData("AccountFilter")
            if (!accountFilterModel){
                accountFilterModel = filterModel.addTreeModel("AccountFilter")
            }

            accountFilterModel.setData("Id", accountId);
        }
        else{
            filterModel.removeData("AccountFilter");
        }

        filterChanged();
    }
}

