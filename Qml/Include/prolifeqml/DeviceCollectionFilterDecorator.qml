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

	property alias licenseCb: licenseComboBox;
	property alias accountCb: accountComboBox;

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged);
        updateText();
        checkWidth();
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

    CollectionDataProvider {
        id: accountsList;

        commandId: "AccountsList";
        subscriptionCommandId: "OnAccountsCollectionChanged"

        fields: ["Id", "Name"];

        Component.onCompleted: {
            let ok = PermissionsController.checkPermission("ViewAllSensors")

            accountComboBox.visible = ok;

            if (ok){
                accountsList.updateModel();
            }
        }

        onCollectionModelChanged: {
            accountsList.collectionModel.insertNewItem(0);

            accountsList.updateComboBoxModel();
        }

        function updateComboBoxModel(){
            accountsList.collectionModel.setData("Id", "All");
            accountsList.collectionModel.setData("Name", qsTr("All customers"))

            accountsList.collectionModel.refresh();

            accountComboBox.model = accountsList.collectionModel;
        }
    }

    function onLocalizationChanged(language){
        mainItem.updateModel();

        accountsList.updateComboBoxModel();

        updateText()
    }

    function updateModel(){
        modelCategogy.clear();

        let index = modelCategogy.insertNewItem();
        modelCategogy.setData("Id", "None", index);
        modelCategogy.setData("Name", qsTr("Show all sensors"), index);

        index = modelCategogy.insertNewItem();
        modelCategogy.setData("Id", "WithoutLicense", index);
        modelCategogy.setData("Name", qsTr("Sensors without a license"), index);

        index = modelCategogy.insertNewItem();
        modelCategogy.setData("Id", "WithLicense", index);
        modelCategogy.setData("Name", qsTr("Sensors with license"), index);

        modelCategogy.refresh();

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
            width: 250;

            currentIndex: 0;

            radius: 3;

            onCurrentIndexChanged: {
                if (licenseComboBox.currentIndex >= 0){
                    let value = licenseComboBox.model.getData("Id", licenseComboBox.currentIndex);

                    mainItem.baseElement.filterChanged("LicenseFilter", value);
                }
            }
        }

        ComboBox {
            id: accountComboBox;
            width: 300;
            height: filtermenu.height;
            currentIndex: 0;
            radius: 3;
            shownItemsCount: 15;
            onCurrentIndexChanged: {
                if (accountComboBox.currentIndex > 0){
                    let value = accountComboBox.model.getData("Id", accountComboBox.currentIndex);

                    mainItem.baseElement.filterChanged("AccountFilter", value);
                }
                else{
                    mainItem.baseElement.filterChanged("AccountFilter", "");
                }
            }
        }

        Row {
            id: row;
            anchors.verticalCenter: parent.verticalCenter;
            height: filtermenu.height;
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
                widthFromDecorator: true;
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

        width: contentWidth;
    }
}

