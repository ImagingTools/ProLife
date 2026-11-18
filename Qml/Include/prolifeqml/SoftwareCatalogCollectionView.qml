import QtQuick 2.12
import Acf 1.0
import imtlicgui 1.0

// Products of the "Software" category: the server restricts the list by the productCategory header.
ProductCollectionView {
	id: softwareCatalogCollectionView;

	fixedCategoryId: "Software";

	function getHeaders(){
		let headers = {}
		headers["productCategory"] = "Software"

		return headers
	}
}
