import QtQuick 2.0
import QtWebSockets 1.15
import Acf 1.0
//import "../qml"
//import "/Users/viktor/ImagingTools/ItDevelopment_mac/ImtCore/Include/imtqml/Resources/qml"
//import "../../../../../ImtCore/Include/imtqml/Resources/qml"
//import "../qml"
import imtgui 1.0

//@using { src.Acf.Style }

//@using { src.imtqml.GqlModel }
//@using { src.imtqml.TreeItemModel }
//@using { src.imtqml.JSONListModel }

//@using { src.imtgui.AuxComponents.TopPanel }
//@using { src.imtgui.AuxComponents.MenuPanel }
//@using { src.imtgui.AuxComponents.TabPanel }
//@using { src.imtgui.AuxComponents.Table }
//@using { src.imtgui.AuxComponents.Button }
//@using { src.imtgui.AuxComponents.TreeView }
//@using { src.imtgui.AuxComponents.TreeItemDelegate }
//@using { src.imtgui.AuxComponents.Preference }
//@using { src.imtgui.AuxComponents.PreferenceDialog }
//@using { src.imtgui.AuxComponents.PopupMenuDialog }
//@using { src.imtgui.AuxComponents.SettingsTextInput }
//@using { src.imtgui.AuxComponents.SettingsIntegerInput }
//@using { src.imtgui.AuxComponents.SettingsComboBox }
//@using { src.imtgui.AuxComponents.DatabaseInput }
//@using { src.imtgui.ThumbnailDecorator }
//@using { src.imtgui.MouseTest }

//@using { src.imtlicgui.PackageCollectionView }
//@using { src.imtauthgui.AccountCollectionView }
//@using { src.imtlicgui.ProductCollectionView }
//@using { src.imtlicgui.InstallationCollectionView }

//@using { src.imtlicgui.PackagesMultiDocView }
//@using { src.imtlicgui.TestMultiDocView }
//@using { src.imtlicgui.ProductsMultiDocView }
//@using { src.imtauthgui.AccountsMultiDocView }
//@using { src.imtlicgui.InstallationsMultiDocView }

//@using { src.imtlicgui.EditDialog }
//@using { src.imtgui.AuxComponents.MessageDialog }
//@using { src.imtgui.AuxComponents.InputDialog }

//@using { src.imtlicgui.PackageView }
//@using { src.imtlicgui.ProductView }
//@using { src.imtlicgui.InstallationEditor }
//@using { src.imtauthgui.AccountEditor }

//@using { src.imtgui.CustomTextField }


Rectangle {
    id: window;

    width: 300;
    height: 500;

    anchors.fill: parent;

    color: "#5d5d5d";
//    Component.onCompleted: {
//        console.log("ThumbnailDecorator onCompleted", PageEnum.ID);
//        thumbnailDecorator.updateModels();
//    }

//    IconStyle {
//        id: IconStyle;
//    }

    Style {
        id: Style;

        Component.onCompleted: {
            Style.mainFontSource = "../Fonts/Ubuntu-Light.ttf";
            Style.boldFontSource = "../Fonts/Ubuntu-Bold.ttf";
        }
    }

    ThumbnailDecorator {
        id: thumbnailDecorator;
        anchors.fill: parent;

//        pagesModel: pagesModel.model
        Component.onCompleted: {
            console.log("ThumbnailDecorator onCompleted", MeterEnum.ID);
            thumbnailDecorator.updateModels();
        }

        onWidthChanged: {
            console.log("width", thumbnailDecorator.width);
        }

    }

}


