import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtdocgui 1.0
import imtauthgui 1.0

DocumentValidator {
    id: root;

    function isValid(data){
        if (!documentModel){
            data.message = qsTr("Document model is invalid")
            return false;
        }

        // Check factory number
        let factoryNumber = documentModel.m_factoryNumber;
        if (String(factoryNumber) === ""){
            data.message = qsTr("Please enter a factory number")
            return false;
        }

        return true;
    }
}

