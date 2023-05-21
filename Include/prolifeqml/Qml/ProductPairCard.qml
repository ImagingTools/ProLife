import QtQuick 2.0
import imtgui 1.0
import Acf 1.0
import imtlicgui 1.0

Rectangle {
    id: card;

    height: Math.max(softwareProduct.height, hardwareProduct.height)

    color: Style.imagingToolsGradient2;

    radius: 10;

    // Software
    property string softwareId: softwareProduct.productId;
    property string licenseName: "";

    // Hardware
    property string hardwareId: hardwareProduct.productId;
    property string macAddress: "";
    property string serialNumber: "";

    property int productIndex: -1;

    property TreeItemModel softwareProductModel: model.SoftwareProduct;
    property TreeItemModel hardwareProductModel: model.HardwareProduct;

    property TreeItemModel commandsModel: null;

    property TreeItemModel devicesModel: TreeItemModel {};

    property LicensesProvider licensesProvider: null;
    property ListView productsView: null;

    property bool readOnly: false;

    signal softwareEdited();
    signal hardwareEdited();

    onProductsViewChanged: {
        softwareProduct.productsView = card.productsView;
    }

    onSoftwareProductModelChanged: {
        console.log("onHardwareProductModelChanged", softwareProductModel.toJSON());
        if (card.softwareProductModel != null){
            if (softwareProductModel.ContainsKey("ProductId")){
                let productId = softwareProductModel.GetData("ProductId");
                softwareProduct.productId = productId;
            }

            if (softwareProductModel.ContainsKey("ActiveLicenses")){
                let licensesModel = softwareProductModel.GetData("ActiveLicenses");

                softwareProduct.licensesModel = licensesModel;
            }
        }
    }

    onHardwareProductModelChanged: {
        console.log("onHardwareProductModelChanged", hardwareProductModel.toJSON());

        if (card.hardwareProductModel != null){
            if (hardwareProductModel.ContainsKey("ProductId")){
                let productId = hardwareProductModel.GetData("ProductId");
                hardwareProduct.productId = productId;
            }

            if (card.hardwareProductModel.ContainsKey("DeviceId")){
                let deviceId = card.hardwareProductModel.GetData("DeviceId");
                hardwareProduct.deviceId = deviceId;
            }
        }
    }

    SoftwareProductCard {
        id: softwareProduct;

        anchors.top: card.top;
        anchors.left: card.left;
//        anchors.bottom: card.bottom;

        width: card.width / 2 - 15;

        licensesProvider: card.licensesProvider;
        readOnly: card.readOnly;
        productsView: card.productsView;
        commmandsVisible: true;

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
//        anchors.bottom: card.bottom;

        height: softwareProduct.height;

        devicesModel: card.devicesModel;

        readOnly: card.readOnly;
        commmandsVisible: true;

        onEdited: {
            card.hardwareEdited();
        }
    }
} //Card


