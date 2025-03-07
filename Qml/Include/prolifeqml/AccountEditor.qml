import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import prolifeAccountsSdl 1.0

ViewBase {
	id: accountEditorContainer;

	width: bodyColumn.width;
	height: bodyColumn.height;

	// anchors.fill: parent;

	property int radius: 3;

	property int textInputHeight: 30;

	property AccountData accountData: model ? model : null;

	Component.onCompleted: {
		groupCollectionDataProvider.updateModel();
	}

	LocalizationEvent {
		onLocalizationChanged: {
			bodyColumn.updateHeaders();
		}
	}

	function setReadOnly(readOnly){
		accountNameInput.readOnly = readOnly;
		accountDescriptionInput.readOnly = readOnly;
		countryInput.readOnly = readOnly;
		streetInput.readOnly = readOnly;
		postalCodeInput.readOnly = readOnly;
		cityInput.readOnly = readOnly;
		emailInput.readOnly = readOnly;
		groupsElement.table.readOnly = readOnly;
	}

	function updateGui(){
		console.log("AccountEditor.qml updateGui", accountData);

		accountNameInput.text = accountData.m_name;
		accountDescriptionInput.text = accountData.m_description;
		emailInput.text = accountData.m_email;
		customerIdInput.text = accountData.m_customerId;

		countryInput.text = accountData.m_country;
		postalCodeInput.text = accountData.m_postalCode;
		cityInput.text = accountData.m_city;
		streetInput.text = accountData.m_street;

		let groupIds = accountData.m_groups.split(';');
		groupsElement.table.uncheckAll();
		if (groupsElement.table.elements){
			for (let i = 0; i < groupsElement.table.elements.getItemsCount(); i++){
				let id = groupsElement.table.elements.getData("Id", i);
				if (groupIds.includes(id)){
					groupsElement.table.checkItem(i);
				}
			}
		}
	}

	function updateModel(){
		accountData.m_name = accountNameInput.text;
		accountData.m_description = accountDescriptionInput.text;
		accountData.m_email = emailInput.text;
		accountData.m_customerId = customerIdInput.text;

		accountData.m_country = countryInput.text;
		accountData.m_postalCode = postalCodeInput.text;
		accountData.m_city = cityInput.text;
		accountData.m_street = streetInput.text;

		let selectedGroupIds = []
		let indexes = groupsElement.table.getCheckedItems();
		for (let index of indexes){
			let id = groupsElement.table.elements.getData("Id", index);
			selectedGroupIds.push(id)
		}

		selectedGroupIds.sort()
		let groups = selectedGroupIds.join(';');
		accountData.m_groups = groups;
	}

	GroupCollectionDataProvider {
		id: groupCollectionDataProvider;
		onCollectionModelChanged: {
			if (groupsElement.table){
				groupsElement.table.elements = groupCollectionDataProvider.collectionModel;

				accountEditorContainer.doUpdateGui();
			}
		}
	}

	Column {
		id: bodyColumn;

		width: 700;

		spacing: Style.sizeLargeMargin;

		GroupHeaderView {
			width: parent.width;

			title: qsTr("Customer Information");
			groupView: customerInformationGroup;
		}

		GroupElementView {
			id: customerInformationGroup;

			width: parent.width;

			TextInputElementView {
				id: customerIdInput;

				name: qsTr("Customer-ID");
				placeHolderText: qsTr("Enter the customer-ID");

				onEditingFinished: {
					accountEditorContainer.doUpdateModel();
				}

				KeyNavigation.tab: accountNameInput;
				KeyNavigation.backtab: groupsElement;

				Component.onCompleted: {
					let ok = PermissionsController.checkPermission("ChangeCustomerId");

					customerIdInput.readOnly = !ok;
				}
			}
		}

		GroupHeaderView {
			width: parent.width;

			title: qsTr("Account Information");
			groupView: accountInformationGroup;
		}

		GroupElementView {
			id: accountInformationGroup;

			width: parent.width;

			TextInputElementView {
				id: accountNameInput;

				name: qsTr("Account Name");
				placeHolderText: qsTr("Enter the account name");
				textInputValidator: accountNameRegexp;
				showErrorWhenInvalid: true;
				errorText: qsTr("Please enter the account name");

				onEditingFinished: {
					accountEditorContainer.doUpdateModel();
				}

				KeyNavigation.tab: accountDescriptionInput;
				KeyNavigation.backtab: customerIdInput;

				Component.onCompleted: {
					let ok = PermissionsController.checkPermission("ChangeAccountName");

					accountNameInput.readOnly = !ok;
				}

				RegularExpressionValidator {
					id: accountNameRegexp;
					regularExpression: /^(?!\s*$).+/;
				}
			}

			TextInputElementView {
				id: accountDescriptionInput;

				name: qsTr("Account Description");
				placeHolderText: qsTr("Enter the account description");

				onEditingFinished: {
					accountEditorContainer.doUpdateModel();
				}

				KeyNavigation.tab: emailInput;
				KeyNavigation.backtab: accountNameInput;

				Component.onCompleted: {
					let ok = PermissionsController.checkPermission("ChangeAccountDescription");

					accountDescriptionInput.readOnly = !ok;
				}
			}

			TextInputElementView {
				id: emailInput;

				width: parent.width;

				name: qsTr("Email");
				placeHolderText: qsTr("Enter the email");

				textInputValidator: mailValid;

				onEditingFinished: {
					accountEditorContainer.doUpdateModel();
				}

				KeyNavigation.tab: countryInput;
				KeyNavigation.backtab: accountDescriptionInput;

				Component.onCompleted: {
					let ok = PermissionsController.checkPermission("ChangeAccountEmail");

					emailInput.readOnly = !ok;
				}
			}

			RegularExpressionValidator {
				id: mailValid;

				regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
			}
		}

		GroupHeaderView {
			width: parent.width;

			title: qsTr("Company Address");
			groupView: companyAddressGroup;
		}

		GroupElementView {
			id: companyAddressGroup;

			width: parent.width;

			TextInputElementView {
				id: countryInput;

				name: qsTr("Country");
				placeHolderText: qsTr("Enter the country");

				onEditingFinished: {
					accountEditorContainer.doUpdateModel();
				}

				KeyNavigation.tab: cityInput;
				KeyNavigation.backtab: emailInput;

				Component.onCompleted: {
					let ok = PermissionsController.checkPermission("ChangeCompanyAddress");

					countryInput.readOnly = !ok;
				}
			}

			TextInputElementView {
				id: cityInput;

				name: qsTr("City");
				placeHolderText: qsTr("Enter the city");

				onEditingFinished: {
					accountEditorContainer.doUpdateModel();
				}

				KeyNavigation.tab: postalCodeInput;
				KeyNavigation.backtab: countryInput;

				Component.onCompleted: {
					let ok = PermissionsController.checkPermission("ChangeCompanyAddress");

					cityInput.readOnly = !ok;
				}
			}

			TextInputElementView {
				id: postalCodeInput;

				name: qsTr("Postal Code");
				placeHolderText: qsTr("Enter the postal code");

				onEditingFinished: {
					accountEditorContainer.doUpdateModel();
				}

				KeyNavigation.tab: streetInput;
				KeyNavigation.backtab: cityInput;

				Component.onCompleted: {
					let ok = PermissionsController.checkPermission("ChangeCompanyAddress");

					postalCodeInput.readOnly = !ok;
				}
			}

			TextInputElementView {
				id: streetInput;

				name: qsTr("Street");
				placeHolderText: qsTr("Enter the street");

				onEditingFinished: {
					accountEditorContainer.doUpdateModel();
				}

				KeyNavigation.tab: groupsElement;
				KeyNavigation.backtab: postalCodeInput;

				Component.onCompleted: {
					let ok = PermissionsController.checkPermission("ChangeCompanyAddress");

					streetInput.readOnly = !ok;
				}
			}
		}

		TreeItemModel {
			id: headersModel;

			Component.onCompleted: {
				bodyColumn.updateHeaders();
			}
		}

		function updateHeaders(){
			headersModel.clear();

			headersModel.insertNewItem();

			headersModel.setData("Id", "Name");
			headersModel.setData("Name", qsTr("Group Name"));

			groupsElement.table.headers = headersModel;
		}

		Text {
			id: titleText;

			color: Style.textColor;
			font.family: Style.fontFamilyBold;
			font.pixelSize: Style.fontSizeXXLarge;

			text: qsTr("Group Information");
		}

		TableElementView {
			id: groupsElement;

			width: parent.width;

			name: qsTr("Groups");

			KeyNavigation.tab: customerIdInput;
			KeyNavigation.backtab: streetInput;

			// table.checkable: true;

			onTableChanged: {
				if (groupsElement.table){
					groupsElement.table.checkable = true;
					groupsElement.table.elements = groupCollectionDataProvider.collectionModel;

					let ok = PermissionsController.checkPermission("ChangeAccountGroups");
					groupsElement.table.readOnly = !ok;
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
	}//Body column
}// Account Editor container
