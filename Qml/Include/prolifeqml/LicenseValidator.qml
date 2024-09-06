import QtQuick 2.15
import Acf 1.0
import imtdocgui 1.0
import imtauthgui 1.0

DocumentValidator {
    id: root;

    function isValid(data){
        if (!documentModel){
            data.message = qsTr("Document model is invalid")
            return false;
        }

        let inUse = documentModel.m_inUse;
        if (inUse){
            data.message = qsTr("The product cannot be edited as it is in use");

            return false;
        }

        if (documentModel.m_productId === ""){
            data.message = qsTr("Please select a product");

            return false;
        }

        if (documentModel.m_licenseUuid === ""){
            data.message = qsTr("Please select a license");

            return false;
        }

        if (documentModel.m_serialNumber === ""){
            data.message = qsTr("Please enter the software-ID");

            return false;
        }

        return true;
    }
}


