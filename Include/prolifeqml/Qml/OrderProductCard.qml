import QtQuick 2.0
import imtgui 1.0
import Acf 1.0
import imtlicgui 1.0

Rectangle {
    id: root;

    width: 600;

    height: header.height + content.height + 2 * root.border.width;

    color: "transparent";

    border.color: root.selected ? Style.textSelected : Style.borderColor;
    border.width: 1;

    radius: 3;

    property int margin: 10;
    property int productIndex: -1;

    property bool readOnly: false;
    property bool isLicenseConsuming: false;
    property bool isNewDevice: model.IsNewDevice ? model.IsNewDevice : false;
    property bool inUse: model.InUse ? model.InUse : false;
    property bool selected: false;

    property string categoryId: model.CategoryId ? model.CategoryId : "";
    property string title: model.ProductName + " (" + model.CategoryId + ")";

    property ListView productsListView: null;
    property Item orderEditorPtr: null;

    signal removed();
    signal edited();

    onCategoryIdChanged: {
        if (root.categoryId == "Software"){
            cardLoader.sourceComponent = softwareProductCard;
        }
        else if (root.categoryId == "Hardware"){
            cardLoader.sourceComponent = hardwareProductCard;
        }
    }

    onProductsListViewChanged: {
        if (cardLoader.item && cardLoader.item.productsView !== undefined){
            cardLoader.item.productsView = root.productsListView;
        }
    }

    property bool hardwareInUse: hardwareCommandsModel.completed && root.inUse;
    property bool softwareInUse: softwareCommandsModel.completed && root.inUse;

    onHardwareInUseChanged: {
        if (hardwareInUse){
            root.setCommandValue(hardwareCommandsModel, "Edit", "Visible", !root.inUse);
            root.setCommandValue(hardwareCommandsModel, "Remove", "Visible", !root.inUse);
            root.setCommandValue(hardwareCommandsModel, "Lock", "Visible", root.inUse);
        }
    }

    onSoftwareInUseChanged: {
        if (softwareInUse){
            root.setCommandValue(softwareCommandsModel, "Edit", "Visible", !root.inUse);
            root.setCommandValue(softwareCommandsModel, "Remove", "Visible", !root.inUse);
            root.setCommandValue(softwareCommandsModel, "Lock", "Visible", root.inUse);
        }
    }

    onReadOnlyChanged: {
        if (cardLoader.item){
            cardLoader.item.readOnly = root.readOnly;
        }

        if (root.categoryId === "Hardware"){
            root.setIsEnabledCommand(hardwareCommandsModel, "Edit", !root.readOnly);
            root.setIsEnabledCommand(hardwareCommandsModel, "Remove", !root.readOnly);
        }
        else if (root.categoryId === "Software"){
            root.setIsEnabledCommand(softwareCommandsModel, "Edit", !root.readOnly);
            root.setIsEnabledCommand(softwareCommandsModel, "Remove", !root.readOnly);
        }
    }

    function setIsEnabledCommand(commandsModel, commandId, isEnabled){
        for (let i = 0; i < commandsModel.GetItemsCount(); i++){
            let id = commandsModel.GetData("Id", i);
            if (id === commandId){
                commandsModel.SetData("IsEnabled", isEnabled, i);
                break;
            }
        }
    }

    function setCommandValue(commandsModel, commandId, commandKey, commandValue){
        for (let i = 0; i < commandsModel.GetItemsCount(); i++){
            let id = commandsModel.GetData("Id", i);
            if (id === commandId){
                commandsModel.SetData(commandKey, commandValue, i);
                break;
            }
        }
    }

    function showErrorMessage(message){
        errorText.text = message;
        messageItem.visible = true;
        root.border.color = Style.errorTextColor;
    }

    Rectangle {
        id: header;

        anchors.top: parent.top;
        anchors.topMargin: parent.border.width;
        anchors.left: parent.left;
        anchors.leftMargin: parent.border.width;
        anchors.right: parent.right;
        anchors.rightMargin: parent.border.width;

        height: 30;

        color: Style.alternateBaseColor;

        Text {
            id: pairProductTitle;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.right: commands.left;
            anchors.rightMargin: 10;

            text: "#" + (root.productIndex + 1) + " " + root.title;

            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            elide: Text.ElideRight;
            wrapMode: Text.NoWrap;
        }

        SimpleCommandsDecorator {
            id: commands;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 10;

            height: header.height;

            onCommandActivated: {
                if (commandId == "Remove"){
                    root.removed();
                }
                else if (commandId == "Edit"){
                    root.edited();
                }
            }
        }
    }

    Column {
        id: content;

        anchors.top: header.bottom;
        anchors.left: parent.left;
        anchors.leftMargin: parent.border.width;
        anchors.right: parent.right;
        anchors.rightMargin: parent.border.width;

        Loader {
            id: cardLoader;

            width: parent.width;

            onLoaded: {
                cardLoader.item.productCardRoot = root;
                cardLoader.item.readOnly = root.readOnly;

                if (cardLoader.item.productsView !== undefined){
                    cardLoader.item.productsView = root.productsListView;
                }
            }
        }

        Rectangle {
            id: messageItem;

            width: parent.width;
            height: visible ? 20 : 0;

            visible: false;

            color: Style.baseColor;

            Text {
                id: errorText;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.right: parent.right;
                anchors.rightMargin: 10;

                elide: Text.ElideRight;

                color: Style.errorTextColor;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
        }
    }

    Component {
        id: softwareProductCard;

        SoftwareProductCard {}
    }

    Component {
        id: hardwareProductCard;

        HardwareProductCard {}
    }

    TreeItemModel {
        id: softwareCommandsModel;

        property bool completed: false;

        Component.onCompleted: {
            let index = softwareCommandsModel.InsertNewItem();

            softwareCommandsModel.SetData("Id", "Edit", index);
            softwareCommandsModel.SetData("Name", "Edit", index);
            softwareCommandsModel.SetData("Icon", "Icons/Edit", index);
            softwareCommandsModel.SetData("IsEnabled", !root.readOnly, index);
            softwareCommandsModel.SetData("Visible", true, index);

            index = softwareCommandsModel.InsertNewItem();

            softwareCommandsModel.SetData("Id", "Remove", index);
            softwareCommandsModel.SetData("Name", "Remove", index);
            softwareCommandsModel.SetData("Icon", "Icons/Delete", index);
            softwareCommandsModel.SetData("IsEnabled", !root.readOnly, index);
            softwareCommandsModel.SetData("Visible", true, index);

            index = softwareCommandsModel.InsertNewItem();

            softwareCommandsModel.SetData("Id", "Lock", index);
            softwareCommandsModel.SetData("Name", "Lock", index);
            softwareCommandsModel.SetData("Icon", "Icons/Lock", index);
            softwareCommandsModel.SetData("IsEnabled", false, index);
            softwareCommandsModel.SetData("Visible", false, index);

            if (root.categoryId == "Software"){
                commands.commandModel = softwareCommandsModel;
            }

            softwareCommandsModel.completed = true;
        }
    }

    TreeItemModel {
        id: hardwareCommandsModel;

        property bool completed: false;

        Component.onCompleted: {
            let index = hardwareCommandsModel.InsertNewItem();

            hardwareCommandsModel.SetData("Id", "Edit", index);
            hardwareCommandsModel.SetData("Name", "Edit", index);
            hardwareCommandsModel.SetData("Icon", "Icons/Edit", index);
            hardwareCommandsModel.SetData("IsEnabled", !root.readOnly, index);
            hardwareCommandsModel.SetData("Visible", true, index);

            index = hardwareCommandsModel.InsertNewItem();

            hardwareCommandsModel.SetData("Id", "Remove", index);
            hardwareCommandsModel.SetData("Name", "Remove", index);
            hardwareCommandsModel.SetData("Icon", "Icons/Delete", index);
            hardwareCommandsModel.SetData("IsEnabled", !root.readOnly, index);
            hardwareCommandsModel.SetData("Visible", true, index);

            index = hardwareCommandsModel.InsertNewItem();

            hardwareCommandsModel.SetData("Id", "Lock", index);
            hardwareCommandsModel.SetData("Name", "Lock", index);
            hardwareCommandsModel.SetData("Icon", "Icons/Lock", index);
            hardwareCommandsModel.SetData("IsEnabled", false, index);
            hardwareCommandsModel.SetData("Visible", false, index);

            if (root.categoryId == "Hardware"){
                commands.commandModel = hardwareCommandsModel;
            }

            hardwareCommandsModel.completed = true;
        }
    }
} //Card


