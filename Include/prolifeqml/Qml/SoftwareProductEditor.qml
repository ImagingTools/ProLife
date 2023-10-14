import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtlicgui 1.0

Item {
    id: root;

    property int itemHeight: 30;
    property int margin: 10;

    property TreeItemModel productModel: TreeItemModel {}
    property var productLicensesModel: TreeItemModel{}

    property bool serialNumberEdit: true;

    property alias tableElements: licensesTable.elements;
    property bool readOnly: false;

    onReadOnlyChanged: {
        serialNumberInput.readOnly = root.readOnly;
        licensesTable.readOnly = root.readOnly;
    }

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", root.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", root.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        root.updateHeaders();
    }

    Text {
        id: serialNumberText;

        anchors.top: parent.top;

        height: visible ? licensesText.height : 0;

        text: qsTr("License Number");
        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_common;

        visible: root.serialNumberEdit;
    }

    CustomTextField {
        id: serialNumberInput;

        anchors.top: serialNumberText.bottom;
        anchors.topMargin: root.margin;

        height: visible ? root.itemHeight : 0;
        width: parent.width;

        placeHolderText: qsTr("Enter the license number");

        visible: root.serialNumberEdit;

        radius: 3;

        readOnly: root.readOnly;

        Component.onCompleted: {
            let ok = PermissionsController.checkPermission("ChangeLicense");
            if (!ok){
                ok = PermissionsController.checkPermission("ChangeLisenseNumber");
            }

            serialNumberInput.readOnly = !ok;
        }

        onTextChanged: {
            root.productModel.SetData("SerialNumber", serialNumberInput.text);
        }
    }

    Text {
        id: licensesText;

        anchors.top: serialNumberInput.bottom;
        anchors.topMargin: root.margin;

        text: qsTr("Licenses");
        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_common;
    }

    AuxTable {
        id: licensesTable;

        anchors.top: licensesText.bottom;
        anchors.topMargin: root.margin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: root.margin;

        width: parent.width;

        radius: 0;

        checkable: true;
        canSelectAll: false;
        isMultiCheckable: false;

        readOnly: root.readOnly;

        Component.onCompleted: {
            let ok = PermissionsController.checkPermission("ChangeLicense");

            licensesTable.readOnly = !ok;

            let canEditOrder = PermissionsController.checkPermission("EditOrder");
            if (canEditOrder){
                licensesTable.readOnly = false;
            }
        }

        delegate: Component {
            LicenseInstanceItemDelegate {
                width: licensesTable.width;

                readOnly: licensesTable.readOnly;

                onStateChanged: {
                    if (root.blockUpdatingModel){
                        return;
                    }

                    licensesTable.forceActiveFocus();

                    let state = this.licenseState;
                    let licenseId = model.Id;
                    let licenseName = this.licenseName;
                    let expiration = this.expiration;

                    let activeLicensesModel = root.productModel.GetData("ActiveLicenses");

                    if (state === Qt.Checked){
                        if (!activeLicensesModel){
                            activeLicensesModel = root.productModel.AddTreeModel("ActiveLicenses");
                        }

                        activeLicensesModel.Clear();

                        let index = activeLicensesModel.InsertNewItem();

                        activeLicensesModel.SetData("Id", licenseId, index);
                        activeLicensesModel.SetData("LicenseName", licenseName, index);
                        activeLicensesModel.SetData("Expiration", expiration, index);
                    }
                    else if (state === Qt.Unchecked){
                        if (activeLicensesModel){
                            for (let i = 0; i < activeLicensesModel.GetItemsCount(); i++){
                                let id = activeLicensesModel.GetData("Id", i);
                                if (id === licenseId){
                                    activeLicensesModel.RemoveItem(i);
                                    break;
                                }
                            }
                        }
                    }

                    root.updateGui();
                }

                onDateChanged: {
                    if (root.blockUpdatingModel){
                        return;
                    }

                    let licenseId = model.Id;
                    let expiration = this.expiration;

                    let activeLicensesModel = root.productModel.GetData("ActiveLicenses");
                    if (activeLicensesModel){
                        for (let i = 0; i < activeLicensesModel.GetItemsCount(); i++){
                            let id = activeLicensesModel.GetData("Id", i);
                            if (id === licenseId){
                                activeLicensesModel.SetData("Expiration", expiration, i);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    property bool blockUpdatingModel: false;

    function updateModel(){
        if (root.blockUpdatingModel){
            return;
        }

        root.productModel.SetData("SerialNumber", serialNumberInput.text);
    }

    function updateGui(){
        console.log("updateGui", root.productModel.toJSON());
        blockUpdatingModel = true;

        if (root.productLicensesModel){
            for (let i = 0; i < root.productLicensesModel.GetItemsCount(); i++){
                let licenseId = root.productLicensesModel.GetData("Id", i);
                let licenseName = root.productLicensesModel.GetData("LicenseName", i);

                root.productLicensesModel.SetData("ExpirationState", Qt.Unchecked, i);
                root.productLicensesModel.SetData("LicenseState", Qt.Unchecked, i);
                root.productLicensesModel.SetData("Expiration", "", i);

                if (root.productModel.ContainsKey("ActiveLicenses")){
                    let activeLicensesModel = root.productModel.GetData("ActiveLicenses");
                    for (let j = 0; j < activeLicensesModel.GetItemsCount(); j++){
                        let activeLicenseId = activeLicensesModel.GetData("Id", j);
                        if (licenseId === activeLicenseId){
                            let expiration = activeLicensesModel.GetData("Expiration", j);
                            if (expiration === "Unlimited"){
                                expiration = ""
                            }

                            root.productLicensesModel.SetData("LicenseState", Qt.Checked, i);
                            root.productLicensesModel.SetData("Expiration", expiration, i);

                            if (expiration !== ""){
                                root.productLicensesModel.SetData("ExpirationState", Qt.Checked, i);
                            }
                        }
                    }
                }
            }
        }

        licensesTable.elements = root.productLicensesModel;

        if (root.productModel.ContainsKey("SerialNumber")){
            let serialNumber = root.productModel.GetData("SerialNumber");
            if (serialNumber){
                serialNumberInput.text = serialNumber;
            }
            else{
                serialNumberInput.text = "";
            }
        }

        blockUpdatingModel = false;
    }

    TreeItemModel {
        id: headersModel;
        Component.onCompleted: {
            root.updateHeaders();

//            licensesTable.tableDecorator = tableDecoratorModel;
        }
    }

    function updateHeaders(){
        headersModel.Clear();

        let index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "LicenseName", index)
        headersModel.SetData("Name", qsTr("License Name"), index)

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "LicenseId", index)
        headersModel.SetData("Name", qsTr("License-ID"), index)

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "Expiration", index)
        headersModel.SetData("Name", qsTr("Expiration"), index)

        licensesTable.headers = headersModel;
    }

    TreeItemModel {
        id: tableDecoratorModel;

        Component.onCompleted: {
            var cellWidthModel = tableDecoratorModel.AddTreeModel("CellWidth");

            let index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", 220, index);
        }
    }
}//Container


