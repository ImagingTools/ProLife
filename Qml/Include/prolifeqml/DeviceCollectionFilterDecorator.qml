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
        checkWidth();
        updateText()
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged);
    }

    onWidthChanged: {
        checkWidth();
    }

    function checkWidth(){
        if (width - filtermenu.width <= content.width + 2 * content.spacing){
            content.visible = false;
        }
        else{
            content.visible = true;
        }
    }

    function updateText(){
        onlyNewSensorsText.text = qsTr("Only new sensors");
    }

    Rectangle{
        anchors.fill: parent;

        color: Style.backgroundColor;
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

            accountsList.collectionModel.Refresh();

            accountComboBox.model = accountsList.collectionModel;
        }
    }

    function onLocalizationChanged(language){
        mainItem.updateModel();

        accountsList.updateComboBoxModel();

        updateText()
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

        modelCategogy.Refresh();

        licenseComboBox.model = modelCategogy;
    }

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
            id: row;

            height: filtermenu.height;

            anchors.verticalCenter: parent.verticalCenter;

            spacing: Style.size_mainMargin;

            Text {
                id: onlyNewSensorsText;

                anchors.verticalCenter: parent.verticalCenter;

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

    FilterPanelDecorator {
        id: filtermenu

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;

        baseElement: mainItem.baseElement;

        width: 325;
    }
}

