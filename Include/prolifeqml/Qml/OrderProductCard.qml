import QtQuick 2.0
import imtgui 1.0
import Acf 1.0
import imtlicgui 1.0

Rectangle {
    id: root;

    height: cardLoader.height + headerBlock.height + 30;
    width: 500;

    color: Style.imagingToolsGradient2;
    border.color: root.selected ? Style.textSelected : "transparent";

    radius: 10;

    property bool readOnly: false;
    property bool isLicenseConsuming: false;

    property bool selected: false;
    property string categoryId: model.CategoryId;

    property TreeItemModel devicesModel: TreeItemModel {};

    property LicensesProvider licensesProvider: null;
    property ListView productsListView: null;

    property int productIndex: -1;

    signal removed();
    signal edited();
    signal createLicenseFile();
    signal unlinked();

    signal pairEdited(string categoryId);

    onLicensesProviderChanged: {
        console.log("root onLicensesProviderChanged", root.licensesProvider);

        if (root.licensesProvider != null){
            if (root.categoryId !== "Hardware"){
                console.log("cardLoader.item", cardLoader.item);
                if (cardLoader.item){
                    cardLoader.item.licensesProvider = root.licensesProvider;
                }
            }
        }
    }

    onProductsListViewChanged: {
        if (cardLoader.item && cardLoader.item.productsView !== undefined){
            cardLoader.item.productsView = root.productsListView;
        }
    }

    onReadOnlyChanged: {
        if (cardLoader.item){
            cardLoader.item.readOnly = root.readOnly;
        }

        if (root.categoryId === "Pair"){
            root.setIsEnabledCommand(pairCommandsModel, "Unlink", !root.readOnly);
            root.setIsEnabledCommand(pairCommandsModel, "Remove", !root.readOnly);
        }
        else if (root.categoryId === "Hardware"){
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

    onCategoryIdChanged: {
        if (root.categoryId == "Pair"){
            cardLoader.sourceComponent = productPairCard;
        }
        else if (root.categoryId == "Software"){
            cardLoader.sourceComponent = softwareProductCard;
        }
        else if (root.categoryId == "Hardware"){
            cardLoader.sourceComponent = hardwareProductCard;
        }
    }

    Rectangle {
        id: headerBlock;

        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;
        anchors.top: parent.top;
        anchors.topMargin: 10;

        height: 13;

        color: "transparent";

        Text {
            id: pairProductTitle;

            anchors.left: parent.left;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: commands.left;
            anchors.rightMargin: 10;

            text: "#" + (root.productIndex + 1) + " " + root.categoryId;

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

            height: headerBlock.height;

            onCommandActivated: {
                if (commandId == "Remove"){
                    root.removed();
                }
                else if (commandId == "Edit"){
                    root.edited();
                }
                else if (commandId == "CreateLicenseFile"){
                    root.createLicenseFile();
                }
                else if (commandId == "Unlink"){
                    root.unlinked();
                }
            }
        }
    }

    Loader {
        id: cardLoader;

        anchors.top: headerBlock.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;
//        anchors.bottom: parent.bottom;
//        anchors.bottomMargin: 10;
//        width: parent.width;

        onHeightChanged: {
            console.log("cardLoader onHeightChanged", height);
        }

        onLoaded: {
            console.log("cardLoader onLoaded");

            if (root.categoryId === "Pair"){
                cardLoader.item.devicesModel = root.devicesModel;

                let hardwareId = cardLoader.item.hardwareId;
                let softwareId = cardLoader.item.softwareId;

                pairProductTitle.text = "#" + (root.productIndex + 1) + " Software & Hardware";

                cardLoader.item.licensesProvider = root.licensesProvider;
            }
            else if (root.categoryId === "Hardware"){
                cardLoader.item.devicesModel = root.devicesModel;
            }
            else if (root.categoryId === "Software"){
                cardLoader.item.licensesProvider = root.licensesProvider;
            }

            cardLoader.item.readOnly = root.readOnly;

            if (cardLoader.item.productsView !== undefined){
                cardLoader.item.productsView = root.productsListView;
            }

            cardLoader.height = cardLoader.item.height;
        }
    }

    Component {
        id: productPairCard;

        ProductPairCard {

            onSoftwareEdited: {
                root.pairEdited("Software");
            }

            onHardwareEdited: {
                root.pairEdited("Hardware");
            }
        }
    }

    Component {
        id: softwareProductCard;

        SoftwareProductCard {
        }
    }

    Component {
        id: hardwareProductCard;

        HardwareProductCard {

        }
    }

    TreeItemModel {
        id: pairCommandsModel;

        Component.onCompleted: {
            let index = pairCommandsModel.InsertNewItem();

            pairCommandsModel.SetData("Id", "Unlink", index);
            pairCommandsModel.SetData("Name", "Unlink", index);
            pairCommandsModel.SetData("Icon", "Unlink", index);
            pairCommandsModel.SetData("IsEnabled", !root.readOnly, index);
            pairCommandsModel.SetData("Visible", true, index);

            index = pairCommandsModel.InsertNewItem();

            pairCommandsModel.SetData("Id", "CreateLicenseFile", index);
            pairCommandsModel.SetData("Name", "Create License File", index);
            pairCommandsModel.SetData("Icon", "Key", index);
            pairCommandsModel.SetData("IsEnabled", root.isLicenseConsuming, index);
            pairCommandsModel.SetData("Visible", true, index);

            index = pairCommandsModel.InsertNewItem();

            pairCommandsModel.SetData("Id", "Remove", index);
            pairCommandsModel.SetData("Name", "Remove", index);
            pairCommandsModel.SetData("Icon", "Close", index);
            pairCommandsModel.SetData("IsEnabled", !root.readOnly, index);
            pairCommandsModel.SetData("Visible", true, index);

            console.log("pairCommandsModel onCompleted");

            if (root.categoryId == "Pair"){
                commands.commandModel = pairCommandsModel;
            }
        }
    }

    TreeItemModel {
        id: softwareCommandsModel;

        Component.onCompleted: {
            let index = softwareCommandsModel.InsertNewItem();

            softwareCommandsModel.SetData("Id", "Edit", index);
            softwareCommandsModel.SetData("Name", "Edit", index);
            softwareCommandsModel.SetData("Icon", "Edit", index);
            softwareCommandsModel.SetData("IsEnabled", !root.readOnly, index);
            softwareCommandsModel.SetData("Visible", true, index);

            index = softwareCommandsModel.InsertNewItem();

            softwareCommandsModel.SetData("Id", "Remove", index);
            softwareCommandsModel.SetData("Name", "Remove", index);
            softwareCommandsModel.SetData("Icon", "Close", index);
            softwareCommandsModel.SetData("IsEnabled", !root.readOnly, index);
            softwareCommandsModel.SetData("Visible", true, index);

            if (root.categoryId == "Software"){
                commands.commandModel = softwareCommandsModel;
            }
        }
    }

    TreeItemModel {
        id: hardwareCommandsModel;

        Component.onCompleted: {
            let index = hardwareCommandsModel.InsertNewItem();

            hardwareCommandsModel.SetData("Id", "Edit", index);
            hardwareCommandsModel.SetData("Name", "Edit", index);
            hardwareCommandsModel.SetData("Icon", "Edit", index);
            hardwareCommandsModel.SetData("IsEnabled", !root.readOnly, index);
            hardwareCommandsModel.SetData("Visible", true, index);

            index = hardwareCommandsModel.InsertNewItem();

            hardwareCommandsModel.SetData("Id", "Remove", index);
            hardwareCommandsModel.SetData("Name", "Remove", index);
            hardwareCommandsModel.SetData("Icon", "Close", index);
            hardwareCommandsModel.SetData("IsEnabled", !root.readOnly, index);
            hardwareCommandsModel.SetData("Visible", true, index);

            if (root.categoryId == "Hardware"){
                commands.commandModel = hardwareCommandsModel;
            }
        }
    }
} //Card


