import QtQuick 2.15
import Acf 1.0
import imtdocgui 1.0

DocumentValidator {
    id: root;

    property RegularExpressionValidator regularExpressionValidator: RegularExpressionValidator {
          regularExpression: /(\d{5})/g;
    }

    function isValid(data){
        let orderId = "";
        if (documentModel.ContainsKey("OrderId")){
            orderId = documentModel.GetData("OrderId");
        }

//        let regExp = new RegExp(regularExpressionValidator.regularExpression);
//        if (!regExp.test(orderId)){
//            data.message = qsTr("ERP Order-ID invalid")

//            return false;
//        }

        let purchaseId = "";
        if (documentModel.ContainsKey("PurchaseId")){
            purchaseId = documentModel.GetData("PurchaseId");
        }

        if (purchaseId === ""){
            data.message = qsTr("Purchase Order-ID cannot be empty");

            return false;
        }

        let customerId = "";
        if (documentModel.ContainsKey("CustomerId")){
            customerId = documentModel.GetData("CustomerId");
        }

        if (customerId === ""){
            data.message = qsTr("Customer cannot be empty");

            return false;
        }

        return true;
    }
}


