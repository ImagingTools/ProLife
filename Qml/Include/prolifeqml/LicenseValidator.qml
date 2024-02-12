import QtQuick 2.15
import Acf 1.0
import imtdocgui 1.0

DocumentValidator {
    id: root;

    property RegularExpressionValidator regularExpressionValidator: RegularExpressionValidator {
        id: macAddressRegExp;

        regularExpression: /^([0-9A-Fa-f]{2}[:]){5}([0-9A-Fa-f]{2})$/;
    }

    function isValid(data){
        if (!documentModel){
            data.message = "Unknown error. Model is invalid.";

            return false;
        }

        let productId = "";
        if (documentModel.ContainsKey("ProductId")){
            productId = documentModel.GetData("ProductId");
        }

        if (productId === ""){
            data.message = qsTr("Please select a product");

            return false;
        }

        let licenseUuid = "";
        if (documentModel.ContainsKey("LicenseUuid")){
            licenseUuid = documentModel.GetData("LicenseUuid");
        }

        if (licenseUuid === ""){
            data.message = qsTr("Please select a license");

            return false;
        }

        let serialNumber = "";
        if (documentModel.ContainsKey("SerialNumber")){
            serialNumber = documentModel.GetData("SerialNumber");
        }

        if (serialNumber === ""){
            data.message = qsTr("Please enter the serial number");

            return false;
        }

        return true;
    }
}


