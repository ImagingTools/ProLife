import QtQuick 2.15
import Acf 1.0
import imtcolgui 1.0

CollectionFilter {
    id: root;

    function setDeviceStatusFilter(status){
        if (status === ""){
            filterModel.RemoveData("ObjectFilter");
        }
        else{
            let objectFilter = filterModel.GetData("ObjectFilter");
            if (!objectFilter){
                objectFilter = filterModel.AddTreeModel("ObjectFilter")
            }

            objectFilter.SetData("Key", "Status");
            objectFilter.SetData("Value", status);
        }

        filterChanged();
    }

    function setLicenseFilter(licenseId){
        let licenceFilterModel = filterModel.GetData("LicenseFilter")
        if (!licenceFilterModel){
            licenceFilterModel = filterModel.AddTreeModel("LicenseFilter")
        }

        licenceFilterModel.SetData("Key", "Status");
        licenceFilterModel.SetData("Value", licenseId);

        filterChanged();
    }

    function setAccountFilter(accountId){
        if (accountId !== ""){
            let accountFilterModel = filterModel.GetData("AccountFilter")
            if (!accountFilterModel){
                accountFilterModel = filterModel.AddTreeModel("AccountFilter")
            }

            accountFilterModel.SetData("Id", accountId);
        }
        else{
            filterModel.RemoveData("AccountFilter");
        }

        filterChanged();
    }
}

