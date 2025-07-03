const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, login, wheelScroll, delay} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
});

test('Start sensors page', async ({ page }) => {
  await checkScreenshot(page, 'start_sensors.png')
});

test('Hidden commands test', async ({ page }) => {
  await clickAt(page, 1184, 116); // Hidden commands click
  await checkScreenshot(page, 'sensors_hidden_commands.png')
});

test('Select sensor test', async ({ page }) => {
  await selectFirstSensor(page)
  await checkScreenshot(page, 'select_first_element.png')

  await clickAt(page, 600, 575); // Select 10 element from table
  await checkScreenshot(page, 'select_10_element.png')
});

test('Revision command test', async ({ page }) => {
  await selectFirstSensor(page)

  await clickAt(page, 1100, 115); // 'Revision' command click
  await checkScreenshot(page, 'revision_dialog.png')

  await clickAt(page, 580, 150); // Select first revision
  await checkScreenshot(page, 'first_revision.png')
});

test('CreateLicenseFile command test', async ({ page }) => {
});

test('Sensors filters test', async ({ page }) => {
  await page.setViewportSize({ width: 1500, height: 800 });

  await clickAt(page, 1075, 170); // 'Text Filter' click
  await page.keyboard.type("ias");
  await checkScreenshot(page, 'sensors_text_filter.png')

  await clickAt(page, 915, 170); // 'Date Filter' click
  await checkScreenshot(page, 'sensors_date_filter_dialog.png')

  await clickAt(page, 873, 392); // Last year click
  await checkScreenshot(page, 'sensors_date_filter.png')

  await clickAt(page, 800, 170); // Only new sensors checkbox click
  await checkScreenshot(page, 'sensors_new_sensors_filter.png')

  await clickAt(page, 190, 170); // Show all sensors combobox click
  await checkScreenshot(page, 'sensors_licenses_filter.png')

  await clickAt(page, 190, 235); // Sensor without licenses click
  await checkScreenshot(page, 'sensors_without_licenses_filter.png')

  await clickAt(page, 190, 170); // Show all sensors combobox click
  await clickAt(page, 190, 265); // Sensor with licenses click
  await checkScreenshot(page, 'sensors_with_licenses_filter.png')

  await clickAt(page, 1242, 170); // Clear text filter
  await clickAt(page, 800, 170); // Only new sensors clear filter
  await clickAt(page, 923, 170); // Date clear filter
  await checkScreenshot(page, 'sensors_clear_filters.png')

  await clickAt(page, 480, 170); // Account filter combobox click
  await checkScreenshot(page, 'sensors_account_filter_dialog.png')

  await clickAt(page, 480, 510); // Quiss account select
  await checkScreenshot(page, 'sensors_quiss_account_filter.png')
});

test('Sensors sort filters 1', async ({ page }) => {
  await clickAt(page, 130, 220); // Status column click ASC
  await checkScreenshot(page, 'sensors_status_column_asc.png')

  await clickAt(page, 130, 220); // Status column click DESC
  await checkScreenshot(page, 'sensors_status_column_desc.png')

  await clickAt(page, 220, 220); // Product column click ASC
  await checkScreenshot(page, 'sensors_product_column_asc.png')

  await clickAt(page, 220, 220); // Product column click DESC
  await checkScreenshot(page, 'sensors_product_column_desc.png')

  await clickAt(page, 405, 220); // Name column click ASC
  await checkScreenshot(page, 'sensors_name_column_asc.png')

  await clickAt(page, 405, 220); // Name column click DESC
  await checkScreenshot(page, 'sensors_name_column_desc.png')

  await clickAt(page, 500, 220); // HardwareId column click ASC
  await checkScreenshot(page, 'sensors_hardware_column_asc.png')

  await clickAt(page, 500, 220); // HardwareId column click DESC
  await checkScreenshot(page, 'sensors_hardware_column_desc.png')
});

test('Sensors sort filters 2', async ({ page }) => {
  await clickAt(page, 580, 220); // Serial Number column click ASC
  await checkScreenshot(page, 'sensors_serial_number_column_asc.png')

  await clickAt(page, 580, 220); // Serial Number column click DESC
  await checkScreenshot(page, 'sensors_serial_number_column_desc.png')

  await clickAt(page, 765, 220); // Purchase-ID column click ASC
  await checkScreenshot(page, 'sensors_purchase_id_column_asc.png')

  await clickAt(page, 765, 220); // Purchase-ID column click DESC
  await checkScreenshot(page, 'sensors_purchase_id_column_desc.png')
});

test('Sensors sort filters 3', async ({ page }) => {
  await clickAt(page, 850, 220); // Project column click ASC
  await checkScreenshot(page, 'sensors_project_column_asc.png')

  await clickAt(page, 850, 220); // Project column click DESC
  await checkScreenshot(page, 'sensors_project_column_desc.png')

  await clickAt(page, 915, 220); // Customer column click ASC
  await checkScreenshot(page, 'sensors_customer_column_asc.png')

  await clickAt(page, 915, 220); // Customer column click DESC
  await checkScreenshot(page, 'sensors_customer_column_desc.png')

  await clickAt(page, 1000, 220); // Added column click ASC
  await checkScreenshot(page, 'sensors_added_column_asc.png')

  await clickAt(page, 1000, 220); // Added column click DESC
  await checkScreenshot(page, 'sensors_added_column_desc.png')

  await clickAt(page, 1130, 220); // Last Modified column click ASC
  await checkScreenshot(page, 'sensors_last_modified_column_asc.png')

  await clickAt(page, 1130, 220); // Last Modified column click DESC
  await checkScreenshot(page, 'sensors_last_modified_column_desc.png')
});

test('Sensors pagination test', async ({ page }) => {
  await clickAt(page, 783, 785); // 50 elements on the page click
  await checkScreenshot(page, 'sensors_pagination_50.png')

  await clickAt(page, 486, 787); // 2 page click
  await checkScreenshot(page, 'sensors_page_2.png')
});

test('Sensors new command test', async ({ page }) => {
  await clickAt(page, 170, 115); // New command click
  await checkScreenshot(page, 'sensors_new_editor.png')

  await clickAt(page, 575, 250); // Device type combobox click
  await checkScreenshot(page, 'sensors_new_editor_device_type.png')

  await clickAt(page, 575, 290); // Select first element from combobox
  await checkScreenshot(page, 'sensors_new_editor_device_type_first_select.png')

  await clickAt(page, 575, 335); // Hardware configuration combobox click
  await checkScreenshot(page, 'sensors_new_editor_configuration.png')

  await clickAt(page, 575, 430); // Select 3 element from combobox
  await checkScreenshot(page, 'sensors_new_editor_configuration_3_select.png')

  await clickAt(page, 575, 575); // Mac Address text input click
  await page.keyboard.type('12345');
  await checkScreenshot(page, 'sensors_new_editor_mac_address_error1.png')

  await clickAt(page, 575, 575); // Mac Address text input click
  await page.keyboard.press('Control+A');
  await page.keyboard.press('Delete');
  await page.keyboard.type('xx:xx:xx:xx:xx:xx');
  await checkScreenshot(page, 'sensors_new_editor_mac_address_error2.png')

  await clickAt(page, 575, 575); // Mac Address text input click
  await page.keyboard.press('Control+A');
  await page.keyboard.press('Delete');
  await page.keyboard.type('11:11:11:11:11:11');
  await checkScreenshot(page, 'sensors_new_editor_mac_address_valid.png')

  await clickAt(page, 575, 495); // Serial Number text input click
  await page.keyboard.type('11:11:11:11:11:11');
  await checkScreenshot(page, 'sensors_new_editor_serial_number_enter.png')

  await clickAt(page, 855, 443); // Set production status to finished dialog YES click
  await clickAt(page, 790, 180); // Device Information group hide
  await checkScreenshot(page, 'sensors_new_editor_2.png')

  await clickAt(page, 575, 325); // Order-ID combobox click
  await page.keyboard.type('766');
  await checkScreenshot(page, 'sensors_new_editor_orders_after_filter.png')

  await clickAt(page, 575, 360); // FIrst element click
  await checkScreenshot(page, 'sensors_new_editor_orders_after_select.png')

  await clickAt(page, 750, 325); // Clear order click
  await checkScreenshot(page, 'sensors_new_editor_after_order_clear.png')

  await clickAt(page, 550, 115); // Bind command click
  await checkScreenshot(page, 'sensors_new_editor_bind.png')

  await clickAt(page, 865, 445); // OK click

  await clickAt(page, 700, 115); // Create license file command click
  await checkScreenshot(page, 'sensors_new_editor_clf.png')

  await clickAt(page, 865, 445); // OK click

  await clickAt(page, 880, 115); // Transfer licenses command click
  await checkScreenshot(page, 'sensors_new_editor_tl.png')

  await clickAt(page, 865, 445); // OK click
});

test('Sensor editor test', async ({ page }) => {
  await selectFirstSensor(page)
  await clickAt(page, 290, 115); // 'Edit' command click
  await checkScreenshot(page, 'sensor_editor.png')

  await clickAt(page, 1366, 178); // Hide document history
  await checkScreenshot(page, 'sensor_without_history.png')

  await clickAt(page, 553, 250); // Device Type combobox click
  await checkScreenshot(page, 'sensor_device_type.png')

  await removeFocus(page)

  await clickAt(page, 558, 335); // Device Configuration combobox click
  await checkScreenshot(page, 'sensor_device_configuration.png')

  await removeFocus(page)

  await clickAt(page, 787, 180); // Hide device information
  await checkScreenshot(page, 'sensor_hide_device_info.png')

  await clickAt(page, 525, 325); // Order-ID combobox click
  await checkScreenshot(page, 'sensor_order_id.png')

  await removeFocus(page)

  await clickAt(page, 509, 405); // Status combobox click
  await checkScreenshot(page, 'sensor_production_status.png')

  await clickAt(page, 487, 493); // Select defect status
  await checkScreenshot(page, 'sensor_select_defect_status.png')

  await clickAt(page, 240, 120); // 'Undo' command click
  await checkScreenshot(page, 'sensor_after_undo.png')

  await clickAt(page, 330, 120); // 'Redo' command click
  await checkScreenshot(page, 'sensor_after_redo.png')

  await clickAt(page, 550, 115); // Bind command click
  await checkScreenshot(page, 'sensors_editor_bind_without_mac_address.png')

  await clickAt(page, 865, 445); // OK click

  await clickAt(page, 700, 115); // Create license file command click
  await checkScreenshot(page, 'sensors_editor_clf_err.png')

  await clickAt(page, 865, 445); // OK click

  await clickAt(page, 880, 115); // Transfer licenses command click
  await checkScreenshot(page, 'sensors_editor_tl.png')
  await clickAt(page, 865, 445); // OK click

  await clickAt(page, 575, 575); // Mac Address text input click
  await page.keyboard.type('22:88:44:11:14:34');
  await checkScreenshot(page, 'sensors_new_editor_mac_address_valid.png')

  await clickAt(page, 140, 120); // 'Save' command click
  await checkScreenshot(page, 'sensor_after_save.png')

  await clickAt(page, 787, 180); // Show device information
  await checkScreenshot(page, 'sensor_show_device_info.png')

  await clickAt(page, 530, 70); // Close document (tab)
  await checkScreenshot(page, 'sensors_after_closing.png')
});

test('Bind command test', async ({ page }) => {
  await clickAt(page, 1000, 170); // Filter text input click
  await page.keyboard.type("8c:1f:64:50:19:0b");
  await selectFirstSensor(page)

  await clickAt(page, 535, 115); // 'Bind' command click
  await checkScreenshot(page, 'bind_sensors_1.png')

  await clickAt(page, 480, 165); // Product combobox click
  await checkScreenshot(page, 'bind_sensors_2.png')

  await clickAt(page, 480, 200); // Select first element
  await checkScreenshot(page, 'bind_sensors_3.png')

  await clickAt(page, 125, 335); // Select first license
  await checkScreenshot(page, 'bind_sensors_4.png')

  await clickAt(page, 125, 370); // Select second license
  await checkScreenshot(page, 'bind_sensors_5.png')

  await clickAt(page, 125, 410); // Deselect second license
  await checkScreenshot(page, 'bind_sensors_6.png')

  await clickAt(page, 695, 430); // Bind first lic
  await checkScreenshot(page, 'bind_sensors_7.png')

  await clickAt(page, 960, 335); // Select binded license
  await clickAt(page, 695, 480); // Unbind license
  await checkScreenshot(page, 'bind_sensors_8.png')

  await clickAt(page, 125, 335); // Select first license
  await clickAt(page, 695, 430);// Bind
  await checkScreenshot(page, 'bind_sensors_9.png')

  await clickAt(page, 1225, 730); // Apply button click
  await checkScreenshot(page, 'bind_sensors_10.png')

  await clickAt(page, 695, 400); // Project text input click
  await page.keyboard.type('Test');
  await checkScreenshot(page, 'bind_sensors_11.png')

  await clickAt(page, 730, 480); // Ok button click
  await checkScreenshot(page, 'bind_sensors_12.png')
});

test('Transfer licenses command test', async ({ page }) => {
  await clickAt(page, 1000, 170); // Filter text input click
  await page.keyboard.type("8c:1f:64:50:19:0b");
  await selectFirstSensor(page)

  await clickAt(page, 290, 115); // 'Edit' command click
  await clickAt(page, 880, 115); // 'Transfer Licenses' command click
  await checkScreenshot(page, 'sensors_transfer_licenses_1.png')

  await clickAt(page, 410, 200); // Select first element
  await checkScreenshot(page, 'sensors_transfer_licenses_2.png')

  await clickAt(page, 1305, 730); // Close button click
  await checkScreenshot(page, 'sensors_transfer_licenses_3.png')

  await clickAt(page, 525, 70); // Close document from tab
  await checkScreenshot(page, 'sensors_transfer_licenses_4.png')
});

async function selectFirstSensor(page) {
  await clickAt(page, 600, 255); // Select first element from table
}

async function removeFocus(page) {
  await clickAt(page, 40, 30);
}
