import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0

ElementView {
    id: root;

    name: model.ProductName;

    clip: false;

    property TreeItemModel activeCommandsModel: TreeItemModel {}

    property bool expanded: true;

    controlComp: Component {
        Item {
            id: item;

            width: commands.width + commands.anchors.rightMargin;
            height: 30;

            SimpleCommandsDecorator {
                id: commands;

                anchors.right: parent.right;
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

    bottomComp: root.categoryId == "Software" ? softwareProductCard : hardwareProductCard;

    Row {
        id: row;

        anchors.verticalCenter: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: Style.size_mainMargin;

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

    property bool readOnly: false;
    property bool inUse: model.InUse ? model.InUse : false;
    property bool isNew: model.IsNew ? model.IsNew : false;

    property bool hardwareInUse: softwareCommandsModel.completed && root.inUse;
    property bool softwareInUse: softwareCommandsModel.completed && root.inUse;

    property string categoryId: model.CategoryId ? model.CategoryId : "";

    signal removed();
    signal edited();

    onHardwareInUseChanged: {
        if (hardwareInUse){
            root.setCommandValue(softwareCommandsModel, "Edit", "Visible", !root.inUse);
            root.setCommandValue(softwareCommandsModel, "Remove", "Visible", !root.inUse);
            root.setCommandValue(softwareCommandsModel, "Lock", "Visible", root.inUse);
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
        root.setIsEnabledCommand(softwareCommandsModel, "Edit", !root.readOnly);
        root.setIsEnabledCommand(softwareCommandsModel, "Remove", !root.readOnly);
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

        SoftwareProductCard {
            visible: root.expanded;
        }
    }

    Component {
        id: hardwareProductCard;

        HardwareProductCard {
            visible: root.expanded;
        }
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

            root.activeCommandsModel = softwareCommandsModel;
            softwareCommandsModel.completed = true;
        }
    }
} //Card


