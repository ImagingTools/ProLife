import QtQuick 2.0
import imtgui 1.0
import Acf 1.0
import imtlicgui 1.0

Rectangle {
    id: card;

    height: Math.max(softwareProduct.height, hardwareProduct.height)

    color: Style.imagingToolsGradient2;

    radius: 3;

    // Software
    property string softwareId: softwareProduct.productId;
    property string licenseName: "";

    // Hardware
    property string hardwareId: hardwareProduct.productId;
    property string macAddress: "";
    property string serialNumber: "";

    property int productIndex: -1;

    property TreeItemModel softwareProductModel: model.SoftwareProduct ? model.SoftwareProduct : null;
    property TreeItemModel hardwareProductModel: model.HardwareProduct ? model.HardwareProduct : null;

    property TreeItemModel commandsModel: null;

    property TreeItemModel devicesModel: TreeItemModel {};

    property LicensesProvider licensesProvider: null;
    property ListView productsView: null;

    property Item productCardRoot: null;

    property bool readOnly: false;

    signal softwareEdited();
    signal hardwareEdited();

//    onProductsViewChanged: {
//        softwareProduct.productsView = card.productsView;
//    }

    onSoftwareProductModelChanged: {
        if (card.softwareProductModel != null){
            if (softwareProductModel.ContainsKey("ProductId")){
                let productId = softwareProductModel.GetData("ProductId");
                softwareProduct.productId = productId;
            }

            if (softwareProductModel.ContainsKey("SerialNumber")){
                let serialNumber = softwareProductModel.GetData("SerialNumber");
                softwareProduct.serialNumber = serialNumber;
            }

            if (softwareProductModel.ContainsKey("ActiveLicenses")){
                let licensesModel = softwareProductModel.GetData("ActiveLicenses");

                softwareProduct.licensesModel = licensesModel;
            }
        }
    }

    onHardwareProductModelChanged: {
        if (card.hardwareProductModel != null){
            if (hardwareProductModel.ContainsKey("ProductId")){
                let productId = hardwareProductModel.GetData("ProductId");
                hardwareProduct.productId = productId;
            }

            if (card.hardwareProductModel.ContainsKey("IsNewDevice")){
                let isNewDevice = card.hardwareProductModel.GetData("IsNewDevice");
                hardwareProduct.isNewDevice = isNewDevice;
            }

            if (card.hardwareProductModel.ContainsKey("MacAddress")){
                let macAddress = card.hardwareProductModel.GetData("MacAddress");

                if (hardwareProduct.isNewDevice){
                    hardwareProduct.macAddress = qsTr("New Sensor")
                }
                else{
                    hardwareProduct.macAddress = macAddress;
                }
            }

            if (card.hardwareProductModel.ContainsKey("SerialNumber")){
                let serialNumber = card.hardwareProductModel.GetData("SerialNumber");
                if (hardwareProduct.isNewDevice){
                    hardwareProduct.serialNumber = qsTr("New Sensor")
                }
                else{
                    hardwareProduct.serialNumber = serialNumber;
                }
            }

            if (card.hardwareProductModel.ContainsKey("ModelTypeId")){
                let modelType = card.hardwareProductModel.GetData("ModelTypeId");
                hardwareProduct.modelType = modelType;
            }

            if (card.hardwareProductModel.ContainsKey("DeviceNotExists")){
                hardwareProduct.notExists = card.hardwareProductModel.GetData("DeviceNotExists")
            }
        }
    }

    SoftwareProductCard {
        id: softwareProduct;

        anchors.top: card.top;
        anchors.left: card.left;

        width: card.width / 2;
        height: Math.max(hardwareProduct.contentHeight, softwareProduct.contentHeight);

        licensesProvider: card.licensesProvider;
        readOnly: card.readOnly;
        commmandsVisible: true;

        productCardRoot: card.productCardRoot;

        onEdited: {
            card.softwareEdited();
        }
    }

    HardwareProductCard {
        id: hardwareProduct;

        anchors.top: card.top;
        anchors.left: softwareProduct.right;
        anchors.leftMargin: 10;
        anchors.right: card.right;

        height: Math.max(hardwareProduct.contentHeight, softwareProduct.contentHeight);

        productCardRoot: card.productCardRoot;

        readOnly: card.readOnly;
        commmandsVisible: true;

        onEdited: {
            card.hardwareEdited();
        }
    }
} //Card


