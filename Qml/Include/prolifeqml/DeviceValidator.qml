// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtdocgui 1.0
import imtauthgui 1.0

DocumentValidator {
    id: root;

    property RegularExpressionValidator regularExpressionValidator: RegularExpressionValidator {
        id: macAddressRegExp;

        regularExpression: /^([0-9A-Fa-f]{2}[:]){5}([0-9A-Fa-f]{2})$/;
    }

    function isValid(data){
        if (!documentModel){
            data.message = qsTr("Document model is invalid")
            return false;
        }

        //Check mac address valid
        let macAddress = documentModel.m_macAddress;
        if (macAddress !== ""){
            let regExp = new RegExp(regularExpressionValidator.regularExpression);
            if (!regExp.test(macAddress)){
                data.message = qsTr("MAC-Address invalid")
                return false;
            }
        }

        //Check device type valid
        let deviceType = documentModel.m_deviceType;
        if (String(deviceType) === ""){
            data.message = qsTr("Please select a device type")

            return false;
        }

        //Check device configuration valid
        let configurationType = documentModel.m_licenseName;
        if (String(configurationType) === ""){
            data.message = qsTr("Please select a device configuration")

            return false;
        }

        return true;
    }
}


