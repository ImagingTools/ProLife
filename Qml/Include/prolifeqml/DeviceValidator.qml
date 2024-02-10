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
        //Check mac address valid
        let macAddress = "";
        if (documentModel.ContainsKey("MacAddress")){
            macAddress = documentModel.GetData("MacAddress");
        }

        if (macAddress !== ""){
            let regExp = new RegExp(regularExpressionValidator.regularExpression);
            if (!regExp.test(macAddress)){
                data.message = qsTr("MAC-Address invalid")
                return false;
            }
        }

        //Check device type valid
        let deviceType = "";
        if (documentModel.ContainsKey("DeviceType")){
            deviceType = documentModel.GetData("DeviceType");
        }

        if (String(deviceType) === ""){
            data.message = qsTr("Please select a device type")

            return false;
        }

        //Check device configuration valid
        let configurationType = "";
        if (documentModel.ContainsKey("LicenseName")){
            configurationType = documentModel.GetData("LicenseName");
        }

        if (String(configurationType) === ""){
            data.message = qsTr("Please select a device configuration")

            return false;
        }

        return true;
    }
}


