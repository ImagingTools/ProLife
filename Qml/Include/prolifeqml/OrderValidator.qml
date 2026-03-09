// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtdocgui 1.0
import imtauthgui 1.0

DocumentValidator {
    id: root;

    property RegularExpressionValidator regularExpressionValidator: RegularExpressionValidator {
          regularExpression: /(\d{5})/g;
    }

    function test(regex, text){
        let re = new RegExp(regex)
        if (re){
            return re.test(text);
        }

        return false;
    }

    function isValid(data){
        if (!documentModel){
            data.message = qsTr("Document model is invalid")
            return false;
        }

        let orderId = documentModel.m_orderId;

        let ok1 = root.test("\\d{5}", orderId) && orderId.length === 5;
        let ok2 = root.test("\\d{8}", orderId) && orderId.length === 8;

        if (!ok1 && !ok2){
            data.message = qsTr("Delivery-ID invalid")

            return false;
        }

        if (documentModel.m_customerId === ""){
            data.message = qsTr("Customer cannot be empty");

            return false;
        }

        return true;
    }
}


