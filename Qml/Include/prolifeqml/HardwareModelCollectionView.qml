import QtQuick 2.12
import Acf 1.0
import imtlicgui 1.0
import imtguigql 1.0

// Products of the "Hardware" category: the server restricts the list by the productCategory header.
ProductCollectionView {
	id: hardwareModelCollectionView;

	fixedCategoryId: "Hardware";

	// A hardware model has no features: it is described by its configurations.
	productEditorComp: Component {
		HardwareModelEditor {
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "Product";
				}
			}
		}
	}

	// The page has its own ID ("HardwareModels") and hands it over as the collection ID;
	// the data still lives in the product collection.
	onCollectionIdChanged: {
		if (hardwareModelCollectionView.collectionId !== "Products"){
			hardwareModelCollectionView.collectionId = "Products"
		}
	}

	// Documents opened here belong to this page, not to the software products page.
	onCommandsDelegateChanged: {
		if (hardwareModelCollectionView.commandsDelegate){
			hardwareModelCollectionView.commandsDelegate.documentManagerId = "HardwareModels"
		}
	}

	function getHeaders(){
		let headers = {}
		headers["productCategory"] = "Hardware"

		return headers
	}
}
