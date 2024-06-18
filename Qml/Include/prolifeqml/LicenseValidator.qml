import QtQuick 2.15
import Acf 1.0
import imtdocgui 1.0
import imtauthgui 1.0

DocumentValidator {
    id: root;

    function isValid(data){
        if (!documentModel){
            data.message = "Unknown error. Model is invalid.";

            return false;
        }

        if (documentModel.containsKey("InUse")){
            let inUse = documentModel.getData("InUse");
            if (inUse){
                data.message = qsTr("The product cannot be edited as it is in use");

                return false;
            }
        }

        let productId = "";
        if (documentModel.containsKey("ProductId")){
            productId = documentModel.getData("ProductId");
        }

        if (productId === ""){
            data.message = qsTr("Please select a product");

            return false;
        }

        let licenseUuid = "";
        if (documentModel.containsKey("LicenseUuid")){
            licenseUuid = documentModel.getData("LicenseUuid");
        }

        if (licenseUuid === ""){
            data.message = qsTr("Please select a license");

            return false;
        }

        let serialNumber = "";
        if (documentModel.containsKey("SerialNumber")){
            serialNumber = documentModel.getData("SerialNumber");
        }

        if (serialNumber === ""){
            data.message = qsTr("Please enter the software-ID");

            return false;
        }

        return true;
    }
}


