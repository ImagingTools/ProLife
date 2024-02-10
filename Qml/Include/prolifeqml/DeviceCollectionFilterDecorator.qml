import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtgui 1.0
import imtauthgui 1.0

DecoratorBase {
    id: mainItem;

    width: baseElement ? baseElement.width: 0;
    height: 40;

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged);
    }

    CollectionDataProvider {
        id: accountsList;

        commandId: "Accounts";

        fields: ["Id", "Name"];

        Component.onCompleted: {
            let ok = PermissionsController.checkPermission("ViewAllSensors")

            accountComboBox.visible = ok;

            if (ok){
                accountsList.updateModel();
            }
        }

        onCollectionModelChanged: {
            accountsList.collectionModel.InsertNewItem(0);

            accountsList.updateComboBoxModel();
        }

        function updateComboBoxModel(){
            accountsList.collectionModel.SetData("Id", "All");
            accountsList.collectionModel.SetData("Name", qsTr("All customers"))

            accountComboBox.model = accountsList.collectionModel;
        }
    }

    function onLocalizationChanged(language){
        mainItem.updateModel();

        accountsList.updateComboBoxModel();
    }

    function updateModel(){
        modelCategogy.Clear();

        let index = modelCategogy.InsertNewItem();
        modelCategogy.SetData("Id", "None", index);
        modelCategogy.SetData("Name", qsTr("Show All Sensors"), index);

        index = modelCategogy.InsertNewItem();
        modelCategogy.SetData("Id", "WithoutLicense", index);
        modelCategogy.SetData("Name", qsTr("Sensors without a license"), index);

        index = modelCategogy.InsertNewItem();
        modelCategogy.SetData("Id", "WithLicense", index);
        modelCategogy.SetData("Name", qsTr("Sensors with license"), index);

        licenseComboBox.model = modelCategogy;
    }

//    onWidthChanged: {
//        if (width - filtermenu.width <= licenseFilterBlock.width + accountFilterBlock.width){
//            licenseFilterBlock.visible = false;
//            accountFilterBlock.visible = false;
//        }
//        else{
//            licenseFilterBlock.visible = true;

//            if (accountFilterBlock.canViewAccountFilter){
//                accountFilterBlock.visible = true;
//            }
//        }
//    }

    TreeItemModel {
        id: modelCategogy;

        Component.onCompleted: {
            mainItem.updateModel();
        }
    }

    Row {
        id: content;

        anchors.left: parent.left;
        anchors.verticalCenter: parent.verticalCenter;

        spacing: Style.size_mainMargin;

        ComboBox {
            id: licenseComboBox;

            height: filtermenu.height;
            width: 200;

            currentIndex: 0;

            radius: 3;

            onCurrentIndexChanged: {
                if (licenseComboBox.currentIndex >= 0){
                    let value = licenseComboBox.model.GetData("Id", licenseComboBox.currentIndex);

                    mainItem.baseElement.filterChanged("LicenseFilter", value);
                }
            }
        }

        ComboBox {
            id: accountComboBox;

            height: filtermenu.height;
            width: 200;

            currentIndex: 0;

            radius: 3;

            shownItemsCount: 15;

            onCurrentIndexChanged: {
                if (accountComboBox.currentIndex > 0){
                    let value = accountComboBox.model.GetData("Id", accountComboBox.currentIndex);

                    mainItem.baseElement.filterChanged("AccountFilter", value);
                }
                else{
                    mainItem.baseElement.filterChanged("AccountFilter", "");
                }
            }
        }

        Row {
            height: content.height;

            anchors.verticalCenter: parent.verticalCenter;

            spacing: Style.size_mainMargin;

            Text {
                anchors.verticalCenter: parent.verticalCenter;

                text: qsTr("Only new sensors");

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CheckBox {
                anchors.verticalCenter: parent.verticalCenter;

                onClicked: {
                    let filterValue = "";
                    if (checkState == Qt.Checked){
                        checkState = Qt.Unchecked;
                    }
                    else{
                        checkState = Qt.Checked;

                        filterValue = "none";
                    }

                    mainItem.baseElement.filterChanged("StatusFilter", filterValue);
                }
            }
        }
    }

//    Item {
//        id: licenseFilterBlock;

//        anchors.verticalCenter: parent.verticalCenter;
//        anchors.left: parent.left;
//        anchors.leftMargin: 10;

//        width: licenseComboBox.width;
//        height: filtermenu.height;

//        ComboBox {
//            id: licenseComboBox;

//            height: filtermenu.height;
//            width: 200;

//            currentIndex: 0;

//            radius: 3;

//            onCurrentIndexChanged: {
//                if (licenseComboBox.currentIndex >= 0){
//                    let value = licenseComboBox.model.GetData("Id", licenseComboBox.currentIndex);

//                    mainItem.baseElement.filterChanged("LicenseFilter", value);
//                }
//            }
//        }

//        ComboBox {
//            id: accountComboBox;

//            height: filtermenu.height;
//            width: 200;

//            currentIndex: 0;

//            radius: 3;

//            shownItemsCount: 15;

//            onCurrentIndexChanged: {
//                if (accountComboBox.currentIndex > 0){
//                    let value = accountComboBox.model.GetData("Id", accountComboBox.currentIndex);

//                    mainItem.baseElement.filterChanged("AccountFilter", value);
//                }
//                else{
//                    mainItem.baseElement.filterChanged("AccountFilter", "");
//                }
//            }
//        }
//    }

//    Item {
//        id: accountFilterBlock;

//        anchors.verticalCenter: parent.verticalCenter;
//        anchors.left: licenseFilterBlock.right;
//        anchors.leftMargin: 10;

//        width: canViewAccountFilter ? accountComboBox.width : 0;
//        height: canViewAccountFilter ? filtermenu.height : 0;

//        property bool canViewAccountFilter: false;

//        Component.onCompleted: {
//            let ok = PermissionsController.checkPermission("ViewAllSensors")
//            accountFilterBlock.canViewAccountFilter = ok;
//            accountFilterBlock.visible = ok;

//            if (ok){
//                accountsList.updateModel();
//            }
//        }

//        CollectionDataProvider {
//            id: accountsList;

//            commandId: "Accounts";

//            fields: ["Id", "Name"];

//            onCollectionModelChanged: {
//                accountsList.collectionModel.InsertNewItem(0);

//                accountFilterBlock.updateModel();
//            }
//        }

//        function updateModel(){
//            accountsList.collectionModel.SetData("Id", "All");
//            accountsList.collectionModel.SetData("Name", qsTr("All customers"))

//            accountComboBox.model = accountsList.collectionModel;
//        }

//        ComboBox {
//            id: accountComboBox;

//            anchors.bottom: parent.bottom;
//            anchors.left: parent.left;

//            height: filtermenu.height;
//            width: 200;

//            currentIndex: 0;

//            radius: 3;

//            shownItemsCount: 15;

//            onCurrentIndexChanged: {
//                if (accountComboBox.currentIndex > 0){
//                    let value = accountComboBox.model.GetData("Id", accountComboBox.currentIndex);

//                    mainItem.baseElement.filterChanged("AccountFilter", value);
//                }
//                else{
//                    mainItem.baseElement.filterChanged("AccountFilter", "");
//                }
//            }
//        }
//    }

//    Text {
//        id: titleInstanceId;

//        anchors.verticalCenter: parent.verticalCenter;
//        anchors.right: filtermenu.left;
//        anchors.rightMargin: 10;

//        visible: false;

//        text: qsTr("Only new sensors!");

//        color: Style.errorTextColor;
//        font.family: Style.fontFamily;
//        font.pixelSize: Style.fontSize_common;
//    }

    FilterPanelDecorator {
        id: filtermenu

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;

        baseElement: mainItem.baseElement;

        width: 325;
    }
}

