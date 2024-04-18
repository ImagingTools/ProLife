import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0

ElementView {
    id: root;

    name: model.ProductName;

    property TreeItemModel activeCommandsModel: TreeItemModel {}

    controlComp: Component {
        Item {
            id: item;

            width: row.width + commands.width + commands.anchors.rightMargin+ row.anchors.rightMargin;
            height: 30;

            Row {
                id: row;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: commands.left;
                anchors.rightMargin: Style.size_largeMargin;

                spacing: Style.size_mainMargin;

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter;

                    width: productCategoryText.width + 2* Style.size_smallMargin;
                    height: 20;

                    color: Style.iconColorOnSelected;

                    radius: Style.buttonRadius;

                    Text {
                        id: productCategoryText;

                        anchors.centerIn: parent;

                        text: root.categoryId;

                        color: Style.baseColor;

                        font.family:Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }
                }

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter;

                    width: newText.width + 2* Style.size_smallMargin;
                    height: 20;

                    color: Style.errorColor;

                    radius: Style.buttonRadius;

                    visible: root.isNew

                    Text {
                        id: newText;

                        anchors.centerIn: parent;

                        text: qsTr("New");

                        color: Style.baseColor;

                        font.family:Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }
                }
            }

            SimpleCommandsDecorator {
                id: commands;

                anchors.right: parent.right;
                anchors.rightMargin: Style.size_mainMargin;

                height: 30;

                commandModel: root.activeCommandsModel;

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
    }

//    topComp: Component {
//        Row {
//            spacing: Style.size_mainMargin;

//            Rectangle {
//                anchors.verticalCenter: parent.verticalCenter;

//                width: productCategoryText.width + 2* Style.size_smallMargin;
//                height: productCategoryText.height;

//                color: Style.iconColorOnSelected;

//                radius: Style.buttonRadius;

//                Text {
//                    id: productCategoryText;

//                    anchors.centerIn: parent;

//                    text: root.categoryId;

//                    color: Style.baseColor;

//                    font.family:Style.fontFamily;
//                    font.pixelSize: Style.fontSize_common;
//                }
//            }

//            Rectangle {
//                anchors.verticalCenter: parent.verticalCenter;

//                width: newText.width + 2* Style.size_smallMargin;
//                height: newText.height;

//                color: Style.errorColor;

//                radius: Style.buttonRadius;

//                visible: root.isNew

//                Text {
//                    id: newText;

//                    anchors.centerIn: parent;

//                    text: qsTr("New");

//                    color: Style.baseColor;

//                    font.family:Style.fontFamily;
//                    font.pixelSize: Style.fontSize_common;
//                }
//            }
//        }
//    }

    property bool readOnly: false;
    property bool isLicenseConsuming: false;
    property bool inUse: model.InUse ? model.InUse : false;
    property bool isNew: model.IsNew ? model.IsNew : false;
    property bool selected: false;

    property bool hardwareInUse: hardwareCommandsModel.completed && root.inUse;
    property bool softwareInUse: softwareCommandsModel.completed && root.inUse;

    property string categoryId: model.CategoryId ? model.CategoryId : "";
    property string title: model.ProductName + " (" + model.CategoryId + ")";

    property ListView productsListView: null;
    property Item orderEditorPtr: null;

    signal removed();
    signal edited();

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

    onCategoryIdChanged: {
        if (root.categoryId == "Software"){
            root.bottomComp = softwareProductCard;
        }
        else if (root.categoryId == "Hardware"){
            root.bottomComp = hardwareProductCard;
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
                root.activeCommandsModel = softwareCommandsModel;
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
                root.activeCommandsModel = hardwareCommandsModel;
            }

            hardwareCommandsModel.completed = true;
        }
    }
} //Card


