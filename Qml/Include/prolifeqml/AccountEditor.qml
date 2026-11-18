import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import prolifeAccountsSdl 1.0

ViewBase {
	id: accountEditorContainer;

	anchors.fill: parent;
	contentColor: Style.baseColor

	property AccountData accountData: model

	// On the vendor page the company is edited with the rights of the hardware management
	property bool isVendorPage: false
	property string permissionArea: isVendorPage ? "/HardwareManagement" : "/AccountManagement"

	function canChange(field){
		return PermissionsController.checkPermission(isVendorPage ? "/HardwareManagement/Edit/Change" : "/AccountManagement/Edit/Change/" + field);
	}

	function getHeaders(){
		return {};
	}

	Component.onCompleted: {
		groupCollectionDataProvider.updateModel();
	}

	GroupCollectionDataProvider {
		id: groupCollectionDataProvider;
		onCollectionModelChanged: {
			var groupsPageInstance = multiPageView.getPageById("Groups")
			if (groupsPageInstance && groupsPageInstance.table){
				groupsPageInstance.tableConnEnabled = false;
				groupsPageInstance.table.elements = groupCollectionDataProvider.collectionModel;
				groupsPageInstance.tableConnEnabled = true;
			}

			accountEditorContainer.doUpdateGui();
		}
	}

	function updateGui(){
		var customerPageInstance = multiPageView.getPageById("CustomerInformation")
		if (customerPageInstance)
			customerPageInstance.updateGui()
		var accountPageInstance = multiPageView.getPageById("AccountInformation")
		if (accountPageInstance)
			accountPageInstance.updateGui()
		var addressPageInstance = multiPageView.getPageById("CompanyAddress")
		if (addressPageInstance)
			addressPageInstance.updateGui()
		var groupsPageInstance = multiPageView.getPageById("Groups")
		if (groupsPageInstance)
			groupsPageInstance.updateGui()
	}

	function updateModel(){
		if (!accountEditorContainer.accountData){
			return
		}

		var customerPageInstance = multiPageView.getPageById("CustomerInformation")
		if (customerPageInstance)
			customerPageInstance.updateModel()
		var accountPageInstance = multiPageView.getPageById("AccountInformation")
		if (accountPageInstance)
			accountPageInstance.updateModel()
		var addressPageInstance = multiPageView.getPageById("CompanyAddress")
		if (addressPageInstance)
			addressPageInstance.updateModel()
		var groupsPageInstance = multiPageView.getPageById("Groups")
		if (groupsPageInstance)
			groupsPageInstance.updateModel()
	}

	function setReadOnly(readOnly){
		var customerPageInstance = multiPageView.getPageById("CustomerInformation")
		if (customerPageInstance)
			customerPageInstance.applyReadOnly(readOnly)
		var accountPageInstance = multiPageView.getPageById("AccountInformation")
		if (accountPageInstance)
			accountPageInstance.applyReadOnly(readOnly)
		var addressPageInstance = multiPageView.getPageById("CompanyAddress")
		if (addressPageInstance)
			addressPageInstance.applyReadOnly(readOnly)
		var groupsPageInstance = multiPageView.getPageById("Groups")
		if (groupsPageInstance)
			groupsPageInstance.applyReadOnly(readOnly)
	}

	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			// Short nav labels - the full wording stays as each page's own section
			// header, the same split DeviceEditor uses ("Device" tab / "Device
			// Information" header). Page ids stay untouched, they are the API.
			if (!accountEditorContainer.isVendorPage){
				multiPageView.addPage("CustomerInformation", qsTr("Customer"), customerPageComp, "Icons/Assignment")
			}
			multiPageView.addPage("AccountInformation", qsTr("Account"), accountPageComp, "Icons/Settings")
			multiPageView.addPage("CompanyAddress", qsTr("Address"), addressPageComp, "Icons/Organization")
			// The groups and the history belong to the customers page
			if (!accountEditorContainer.isVendorPage){
				multiPageView.addPage("Groups", qsTr("Groups"), groupsPageComp, "Icons/MultipleUser")
				if (PermissionsController.checkPermission(accountEditorContainer.permissionArea + "/Revisions/View")){
					multiPageView.addPage("History", qsTr("History"), historyPageComp, "Icons/History")
				}
			}
			multiPageView.currentIndex = 0
		}

		Component.onCompleted: {
			multiPageView.updatePages()
		}
	}

	Component {
		id: customerPageComp

		Item {
			id: customerPage
			anchors.fill: parent

			function updateGui(){
				if (!accountEditorContainer.accountData){
					return
				}

				customerIdInput.text = accountEditorContainer.accountData.m_customerId;
			}

			function updateModel(){
				if (!accountEditorContainer.accountData){
					return
				}

				accountEditorContainer.accountData.m_customerId = customerIdInput.text;
			}

			function applyReadOnly(readOnly){
				customerIdInput.readOnly = readOnly || !accountEditorContainer.canChange("CustomerId");
			}

			Component.onCompleted: {
				customerPage.updateGui();
				customerPage.applyReadOnly(accountEditorContainer.readOnly);
			}

			CustomScrollbar {
				id: scrollbar;
				z: parent.z + 1;
				anchors.right: parent.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				secondSize: Style.marginM;
				targetItem: flickable;
			}

			Flickable {
				id: flickable;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: scrollbar.left;
				anchors.rightMargin: Style.marginXL;
				contentHeight: bodyColumn.height + 2 * Style.marginXL;

				boundsBehavior: Flickable.StopAtBounds;
				clip: true;

				Column {
					id: bodyColumn;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: Math.min(parent.width, Style.contentWidthMax);
					spacing: Style.marginXL;

					GroupHeaderView {
						width: parent.width;
						title: qsTr("Customer Information");
						groupView: customerInformationGroup;
					}

					GroupElementView {
						id: customerInformationGroup;
						objectName: "CustomerInformationGroup";
						width: parent.width;

						TextInputElementView {
							id: customerIdInput;
							objectName: "CustomerIdInput";

							name: qsTr("Customer-ID");
							placeHolderText: qsTr("Enter the customer-ID");

							onEditingFinished: {
								accountEditorContainer.doUpdateModel();
							}
						}
					}
				}
			}
		}
	}

	Component {
		id: accountPageComp

		Item {
			id: accountPage
			anchors.fill: parent

			function updateGui(){
				if (!accountEditorContainer.accountData){
					return
				}

				accountNameInput.text = accountEditorContainer.accountData.m_name;
				accountDescriptionInput.text = accountEditorContainer.accountData.m_description;
				emailInput.text = accountEditorContainer.accountData.m_email;
				websiteInput.text = accountEditorContainer.accountData.m_website;
				manufacturerSwitch.checked = accountEditorContainer.accountData.m_isManufacturer;
				supplierSwitch.checked = accountEditorContainer.accountData.m_isSupplier;
			}

			function updateModel(){
				if (!accountEditorContainer.accountData){
					return
				}

				accountEditorContainer.accountData.m_name = accountNameInput.text;
				accountEditorContainer.accountData.m_description = accountDescriptionInput.text;
				accountEditorContainer.accountData.m_email = emailInput.text;
				accountEditorContainer.accountData.m_website = websiteInput.text;

				// The customer page keeps the roles: a company saved there is a customer
				if (accountEditorContainer.isVendorPage){
					accountEditorContainer.accountData.m_isManufacturer = manufacturerSwitch.checked;
					accountEditorContainer.accountData.m_isSupplier = supplierSwitch.checked;
				}
			}

			function applyReadOnly(readOnly){
				accountNameInput.readOnly = readOnly || !accountEditorContainer.canChange("Name");
				accountDescriptionInput.readOnly = readOnly || !accountEditorContainer.canChange("Description");
				emailInput.readOnly = readOnly || !accountEditorContainer.canChange("Email");
				websiteInput.readOnly = readOnly || !accountEditorContainer.canChange("Name");

				manufacturerSwitch.readOnly = readOnly || !accountEditorContainer.canChange("Name");
				supplierSwitch.readOnly = readOnly || !accountEditorContainer.canChange("Name");
			}

			Component.onCompleted: {
				accountPage.updateGui();
				accountPage.applyReadOnly(accountEditorContainer.readOnly);
			}

			CustomScrollbar {
				id: scrollbar;
				z: parent.z + 1;
				anchors.right: parent.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				secondSize: Style.marginM;
				targetItem: flickable;
			}

			Flickable {
				id: flickable;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: scrollbar.left;
				anchors.rightMargin: Style.marginXL;
				contentHeight: bodyColumn.height + 2 * Style.marginXL;

				boundsBehavior: Flickable.StopAtBounds;
				clip: true;

				Column {
					id: bodyColumn;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: Math.min(parent.width, Style.contentWidthMax);
					spacing: Style.marginXL;

					GroupHeaderView {
						width: parent.width;
						title: qsTr("Account Information");
						groupView: accountInformationGroup;
					}

					GroupElementView {
						id: accountInformationGroup;
						objectName: "AccountInformationGroup";
						width: parent.width;

						TextInputElementView {
							id: accountNameInput;
							objectName: "AccountNameInput";

							name: qsTr("Account Name");
							placeHolderText: qsTr("Enter the account name");
							textInputValidator: accountNameRegexp;
							showErrorWhenInvalid: true;
							errorText: qsTr("Please enter the account name");

							onEditingFinished: {
								accountEditorContainer.doUpdateModel();
							}

							RegularExpressionValidator {
								id: accountNameRegexp;
								regularExpression: /^(?!\s*$).+/;
							}
						}

						TextInputElementView {
							id: accountDescriptionInput;
							objectName: "AccountDescriptionInput";

							name: qsTr("Account Description");
							placeHolderText: qsTr("Enter the account description");

							onEditingFinished: {
								accountEditorContainer.doUpdateModel();
							}

						}

						TextInputElementView {
							id: emailInput;
							objectName: "EmailInput";

							width: parent.width;

							name: qsTr("Email");
							placeHolderText: qsTr("Enter the email");

							textInputValidator: mailValid;
							showErrorWhenInvalid: true;
							errorText: qsTr("Please enter a valid email");

							onEditingFinished: {
								accountEditorContainer.doUpdateModel();
							}

						}

						TextInputElementView {
							id: websiteInput;
							objectName: "WebsiteInput";

							name: qsTr("Website");
							placeHolderText: qsTr("Enter the website");

							onEditingFinished: {
								accountEditorContainer.doUpdateModel();
							}
						}

						RegularExpressionValidator {
							id: mailValid;
							regularExpression: /^(\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*)?$/;
						}
					}

					GroupHeaderView {
						width: parent.width;
						title: qsTr("Roles");
						groupView: rolesGroup;
						visible: accountEditorContainer.isVendorPage;
					}

					GroupElementView {
						id: rolesGroup;
						objectName: "RolesGroup";
						width: parent.width;
						visible: accountEditorContainer.isVendorPage;

						SwitchElementView {
							id: manufacturerSwitch;
							objectName: "ManufacturerSwitch";
							name: qsTr("Manufacturer");
							description: qsTr("Makes hardware components: offered as the manufacturer of a component");

							onCheckedChanged: {
								if (accountEditorContainer.accountData && manufacturerSwitch.checked !== accountEditorContainer.accountData.m_isManufacturer){
									accountEditorContainer.doUpdateModel();
								}
							}
						}

						SwitchElementView {
							id: supplierSwitch;
							objectName: "SupplierSwitch";
							name: qsTr("Supplier");
							description: qsTr("Sells hardware components: offered as a supplier of a component");

							onCheckedChanged: {
								if (accountEditorContainer.accountData && supplierSwitch.checked !== accountEditorContainer.accountData.m_isSupplier){
									accountEditorContainer.doUpdateModel();
								}
							}
						}
					}
				}
			}
		}
	}

	Component {
		id: addressPageComp

		Item {
			id: addressPage
			anchors.fill: parent

			function updateGui(){
				if (!accountEditorContainer.accountData){
					return
				}

				countryInput.text = accountEditorContainer.accountData.m_country;
				postalCodeInput.text = accountEditorContainer.accountData.m_postalCode;
				cityInput.text = accountEditorContainer.accountData.m_city;
				streetInput.text = accountEditorContainer.accountData.m_street;
			}

			function updateModel(){
				if (!accountEditorContainer.accountData){
					return
				}

				accountEditorContainer.accountData.m_country = countryInput.text;
				accountEditorContainer.accountData.m_postalCode = postalCodeInput.text;
				accountEditorContainer.accountData.m_city = cityInput.text;
				accountEditorContainer.accountData.m_street = streetInput.text;
			}

			function applyReadOnly(readOnly){
				let canChange = accountEditorContainer.canChange("CompanyAddress");
				countryInput.readOnly = readOnly || !canChange;
				cityInput.readOnly = readOnly || !canChange;
				postalCodeInput.readOnly = readOnly || !canChange;
				streetInput.readOnly = readOnly || !canChange;
			}

			Component.onCompleted: {
				addressPage.updateGui();
				addressPage.applyReadOnly(accountEditorContainer.readOnly);
			}

			CustomScrollbar {
				id: scrollbar;
				z: parent.z + 1;
				anchors.right: parent.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				secondSize: Style.marginM;
				targetItem: flickable;
			}

			Flickable {
				id: flickable;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: scrollbar.left;
				anchors.rightMargin: Style.marginXL;
				contentHeight: bodyColumn.height + 2 * Style.marginXL;

				boundsBehavior: Flickable.StopAtBounds;
				clip: true;

				Column {
					id: bodyColumn;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: Math.min(parent.width, Style.contentWidthMax);
					spacing: Style.marginXL;

					GroupHeaderView {
						width: parent.width;
						title: qsTr("Company Address");
						groupView: companyAddressGroup;
					}

					GroupElementView {
						id: companyAddressGroup;
						objectName: "CompanyAddressGroup";
						width: parent.width;

						TextInputElementView {
							id: countryInput;
							objectName: "CountryInput";

							name: qsTr("Country");
							placeHolderText: qsTr("Enter the country");

							onEditingFinished: {
								accountEditorContainer.doUpdateModel();
							}

						}

						TextInputElementView {
							id: cityInput;
							objectName: "CityInput";

							name: qsTr("City");
							placeHolderText: qsTr("Enter the city");

							onEditingFinished: {
								accountEditorContainer.doUpdateModel();
							}

						}

						TextInputElementView {
							id: postalCodeInput;
							objectName: "PostalCodeInput";

							name: qsTr("Postal Code");
							placeHolderText: qsTr("Enter the postal code");

							onEditingFinished: {
								accountEditorContainer.doUpdateModel();
							}

						}

						TextInputElementView {
							id: streetInput;
							objectName: "StreetInput";

							name: qsTr("Street");
							placeHolderText: qsTr("Enter the street");

							onEditingFinished: {
								accountEditorContainer.doUpdateModel();
							}

						}
					}
				}
			}
		}
	}

	Component {
		id: groupsPageComp

		Item {
			id: groupsPage
			anchors.fill: parent

			readonly property var table: groupsElement.table
			property alias tableConnEnabled: tableConn.enabled

			function updateGui(){
				if (!accountEditorContainer.accountData){
					return
				}

				let groupIds = []
				if (accountEditorContainer.accountData.m_groups){
					groupIds = accountEditorContainer.accountData.m_groups.split(';');
				}

				tableConn.enabled = false;
				groupsElement.table.uncheckAll();
				if (groupsElement.table.elements){
					for (let i = 0; i < groupsElement.table.elements.getItemsCount(); i++){
						let id = groupsElement.table.elements.getData("id", i);
						if (groupIds.includes(id)){
							groupsElement.table.checkItem(i);
						}
					}
				}
				tableConn.enabled = true;
			}

			function updateModel(){
				if (!accountEditorContainer.accountData){
					return
				}

				let selectedGroupIds = []
				let indexes = groupsElement.table.getCheckedItems();
				for (let index of indexes){
					let id = groupsElement.table.elements.getData("id", index);
					selectedGroupIds.push(id)
				}

				selectedGroupIds.sort()
				accountEditorContainer.accountData.m_groups = selectedGroupIds.join(';');
			}

			function applyReadOnly(readOnly){
				if (groupsElement.table){
					groupsElement.table.readOnly = readOnly || !accountEditorContainer.canChange("Groups");
				}
			}

			function updateHeaders(){
				headersModel.clear();
				headersModel.insertNewItem();
				headersModel.setData("id", "name");
				headersModel.setData("name", qsTr("Group Name"));
				groupsElement.table.headers = headersModel;
			}

			LocalizationEvent {
				onLocalizationChanged: {
					groupsPage.updateHeaders();
				}
			}

			Component.onCompleted: {
				groupsPage.updateGui();
				groupsPage.applyReadOnly(accountEditorContainer.readOnly);
			}

			TreeItemModel {
				id: headersModel;

				Component.onCompleted: {
					groupsPage.updateHeaders();
				}
			}

			CustomScrollbar {
				id: scrollbar;
				z: parent.z + 1;
				anchors.right: parent.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				secondSize: Style.marginM;
				targetItem: flickable;
			}

			Flickable {
				id: flickable;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: scrollbar.left;
				anchors.rightMargin: Style.marginXL;
				contentHeight: bodyColumn.height + 2 * Style.marginXL;

				boundsBehavior: Flickable.StopAtBounds;
				clip: true;

				Column {
					id: bodyColumn;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: Math.min(parent.width, Style.contentWidthMax);
					spacing: Style.marginXL;

					GroupHeaderView {
						width: parent.width;
						title: qsTr("Groups");
						groupView: groupsGroup;
					}

					GroupElementView {
						id: groupsGroup;
						objectName: "GroupsGroup";
						width: parent.width;

						TableElementView {
							id: groupsElement;
							objectName: "GroupsTable";

							name: qsTr("Groups");

							onTableChanged: {
								if (groupsElement.table){
									groupsElement.table.checkable = true;

									tableConn.enabled = false;
									groupsElement.table.elements = groupCollectionDataProvider.collectionModel;
									tableConn.enabled = true;
									groupsPage.applyReadOnly(accountEditorContainer.readOnly);
								}
							}

							Connections {
								id: tableConn;
								target: groupsElement.table;

								function onCheckedItemsChanged(){
									accountEditorContainer.doUpdateModel();
								}
							}
						}
					}
				}
			}
		}
	}

	Component {
		id: historyPageComp

		Item {
			id: historyPage
			anchors.fill: parent

			Item {
				id: centeredContainer
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				anchors.horizontalCenter: parent.horizontalCenter
				width: Math.min(parent.width - Style.marginXL * 2, Style.contentWidthMax)
			}

			GroupHeaderView {
				id: historyHeader
				anchors.left: centeredContainer.left
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.right: centeredContainer.right
				title: qsTr("History") + " (" + historyView.revisionsCount + ")"
			}

			DocumentHistoryView {
				id: historyView
				revisionsPermissionPath: accountEditorContainer.permissionArea + "/Revisions"
				anchors.left: centeredContainer.left
				anchors.top: historyHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.right: centeredContainer.right
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				documentId: accountEditorContainer.accountData ? accountEditorContainer.accountData.m_id : "";
				collectionId: accountEditorContainer.isVendorPage ? "HardwareVendors" : "Accounts";

				function getHeaders(){
					return accountEditorContainer.getHeaders()
				}
			}
		}
	}
}
