import QtQuick 2.0
import imtgui 1.0
import Acf 1.0
import imtlicgui 1.0

Rectangle {
    id: root;

    width: 600;

    height: header.height + content.height /*+ 2 * root.margin*/ + 2 * root.border.width;

    color: "transparent";

    border.color: root.selected ? Style.textSelected : Style.borderColor;
    border.width: 1;

    radius: 3;

    property bool readOnly: false;
    property bool isLicenseConsuming: false;

    property bool isNewDevice: model.IsNewDevice ? model.IsNewDevice : false;

    property int margin: 10;

    property bool selected: false;
    property string categoryId: model.CategoryId ? model.CategoryId : "";
    property string title: model.ProductId + " (" + root.categoryId + ")";

    property TreeItemModel devicesModel: TreeItemModel {};

    property LicensesProvider licensesProvider: null;
    property ListView productsListView: null;
    property Item orderEditorPtr: null;

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
                else if (commandId == "CreateLicenseFile"){
                    root.createLicenseFile();
                }
                else if (commandId == "Unlink"){
                    root.unlinked();
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

                if (root.categoryId === "Pair"){
                    cardLoader.item.devicesModel = root.devicesModel;

                    let hardwareId = cardLoader.item.hardwareId;
                    let softwareId = cardLoader.item.softwareId;

                    let softwareProduct = model.SoftwareProduct;
                    let hardwareProduct = model.HardwareProduct;

                    let softwareProductId = "";
                    let softwareSerialNumber = "";
                    if (softwareProduct){
                        softwareProductId = softwareProduct.GetData("ProductId");
                        softwareSerialNumber = softwareProduct.GetData("SerialNumber");
                    }

                    let hardwareProductId = "";
                    if (hardwareProduct){
                        hardwareProductId = hardwareProduct.GetData("ProductId");
                    }

                    if (softwareSerialNumber && softwareSerialNumber != ""){
                        root.title = softwareProductId + ' (' + softwareSerialNumber +')' + ' & ' + hardwareProductId;
                    }
                    else{
                        root.title = softwareProductId + ' & ' + hardwareProductId;
                    }

                    cardLoader.item.licensesProvider = root.licensesProvider;
                }
                else if (root.categoryId === "Hardware"){
                }
                else if (root.categoryId === "Software"){
                    cardLoader.item.licensesProvider = root.licensesProvider;
                }

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

//            pairCommandsModel.SetData("Id", "Unlink", index);
//            pairCommandsModel.SetData("Name", "Unlink", index);
//            pairCommandsModel.SetData("Icon", "Unlink", index);
//            pairCommandsModel.SetData("IsEnabled", !root.readOnly, index);
//            pairCommandsModel.SetData("Visible", true, index);

//            index = pairCommandsModel.InsertNewItem();

//            pairCommandsModel.SetData("Id", "CreateLicenseFile", index);
//            pairCommandsModel.SetData("Name", "Create License File", index);
//            pairCommandsModel.SetData("Icon", "Key", index);
//            pairCommandsModel.SetData("IsEnabled", root.isLicenseConsuming, index);
//            pairCommandsModel.SetData("Visible", true, index);

//            index = pairCommandsModel.InsertNewItem();

            pairCommandsModel.SetData("Id", "Remove", index);
            pairCommandsModel.SetData("Name", "Remove", index);
            pairCommandsModel.SetData("Icon", "Remove", index);
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
            softwareCommandsModel.SetData("Icon", "Remove", index);
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
            hardwareCommandsModel.SetData("Icon", "Remove", index);
            hardwareCommandsModel.SetData("IsEnabled", !root.readOnly, index);
            hardwareCommandsModel.SetData("Visible", true, index);

            if (root.categoryId == "Hardware"){
                commands.commandModel = hardwareCommandsModel;
            }
        }
    }
} //Card


