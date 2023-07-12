import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionView {
    id: container;

    visibleMetaInfo: false;

    function selectItem(id, name, index){
        console.log("selectItem", id, name, index);
        if (index < 0){
            return;
        }

        let editorPath = container.getEditorPath();
        let commandId = container.getEditorCommandId();

        let elementsModel = container.table.elements;

        let orderUuid = elementsModel.GetData("OrderUuid", index);

        container.documentManager.openDocument(id,
                                               {"Id": id, "OrderUuid": orderUuid,  "Name": name, "CommandsId": commandId, "Source": editorPath});
    }
}
