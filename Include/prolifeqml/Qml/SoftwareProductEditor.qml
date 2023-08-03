import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0

Item {
    id: root;

    property int itemHeight: 30;
    property int margin: 10;

    property TreeItemModel productModel: TreeItemModel {}
    property var productLicensesModel: TreeItemModel{}

    property bool serialNumberEdit: true;

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

        onEditingFinished: {
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

        onHeadersChanged: {
            //licensesTable.tableDecorator = tableDecoratorModel;
        }

        delegate: Component {
            LicenseInstanceItemDelegate {
                width: licensesTable.width;

                onStateChanged: {
                    if (root.blockUpdatingModel){
                        return;
                    }

                    let state = this.licenseState;
                    let licenseId = this.licenseId;
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
                        activeLicensesModel.SetData("Name", licenseName, index);
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

                    let licenseId = this.licenseId;
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
    }

    function updateGui(){
        blockUpdatingModel = true;

        if (root.productLicensesModel){
            for (let i = 0; i < root.productLicensesModel.GetItemsCount(); i++){
                let licenseId = root.productLicensesModel.GetData("Id", i);
                let licenseName = root.productLicensesModel.GetData("Name", i);

                root.productLicensesModel.SetData("ExpirationState", Qt.Unchecked, i);
                root.productLicensesModel.SetData("LicenseState", Qt.Unchecked, i);
                root.productLicensesModel.SetData("Expiration", "", i);

                if (root.productModel.ContainsKey("ActiveLicenses")){
                    let activeLicensesModel = root.productModel.GetData("ActiveLicenses");
                    for (let j = 0; j < activeLicensesModel.GetItemsCount(); j++){
                        let activeLicenseId = activeLicensesModel.GetData("Id", j);
                        if (licenseId === activeLicenseId){
//                            activeLicensesModel.SetData("Name", licenseName, j);

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
            serialNumberInput.text = root.productModel.GetData("SerialNumber");
        }

        blockUpdatingModel = false;
    }

    TreeItemModel {
        id: headersModel;
        Component.onCompleted: {
             root.updateHeaders();
        }
    }

    function updateHeaders(){
        headersModel.Clear();

        let index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "Name", index)
        headersModel.SetData("Name", qsTr("License Name"), index)

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "Id", index)
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


