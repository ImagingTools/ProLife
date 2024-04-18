import QtQuick 2.15
import Acf 1.0
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
        let canChange = PermissionsController.checkPermission("ChangeOrder");
        if (!canChange){
            data.message = qsTr("Permission denied")

            return false;
        }

        let orderId = "";
        if (documentModel.ContainsKey("OrderId")){
            orderId = documentModel.GetData("OrderId");
        }

        let ok1 = root.test("\\d{5}", orderId) && orderId.length === 5;
        let ok2 = root.test("\\d{10}", orderId) && orderId.length === 10;

        if (!ok1 && !ok2){
            data.message = qsTr("Delivery-ID invalid")

            return false;
        }

//        let purchaseId = "";
//        if (documentModel.ContainsKey("PurchaseId")){
//            purchaseId = documentModel.GetData("PurchaseId");
//        }

//        if (purchaseId === ""){
//            data.message = qsTr("Purchase Order-ID cannot be empty");

//            return false;
//        }

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


