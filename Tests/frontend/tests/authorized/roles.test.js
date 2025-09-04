const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, wheelScroll, delay} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickAt(page, 40, 750); // Administration page click
  await clickAt(page, 173, 85); // Roles subpage click
});

test('Start roles page', async ({ page }) => {
  await checkScreenshot(page, 'start_roles.png')
});

test('Roles sort tests', async ({ page }) => {
  await clickAt(page, 390, 180); // Role Name column click
  await checkScreenshot(page, 'role_name_asc.png')

  await clickAt(page, 390, 180); // Role Name column click
  await checkScreenshot(page, 'role_name_desc.png')

  await clickAt(page, 700, 180); // Role ID column click
  await checkScreenshot(page, 'role_id_asc.png')

  await clickAt(page, 700, 180); // Role ID column click
  await checkScreenshot(page, 'role_id_desc.png')

  await clickAt(page, 1000, 180); // Role Description column click
  await checkScreenshot(page, 'role_description_asc.png')

  await clickAt(page, 1000, 180); // Role Description column click
  await checkScreenshot(page, 'role_description_desc.png')

  await clickAt(page, 787, 786); // Pagination 50 click
  await checkScreenshot(page, 'role_pagination_50.png')
});

test('Roles filter tests', async ({ page }) => {
  await clickAt(page, 1000, 135); // Text filter click
  await page.keyboard.type('admin');
  await checkScreenshot(page, 'roles_text_filter.png')

  await clickAt(page, 420, 135); // Document state filter click
  await checkScreenshot(page, 'roles_filters_document_state.png')

  await clickAt(page, 45, 25); // Remove focus

  await clickAt(page, 300, 135); // Date filter click
  await checkScreenshot(page, 'roles_filters_date.png', { x: 275, y: 415, width: 240, height: 100 })
});

test('Roles remove command GUI test', async ({ page }) => {
  await clickAt(page, 672, 215); // Select first element
  await clickAt(page, 525, 85); // Remove command click
  await checkScreenshot(page, 'roles_remove_dialog.png')

  await clickAt(page, 866, 444); // Click 'No' button in dialog
  await checkScreenshot(page, 'roles_after_no_remove.png')
});

test('Roles New command GUI test', async ({ page }) => {
  await clickAt(page, 326, 86); // New command click
  await checkScreenshot(page, 'new_role_editor.png')
  
  await clickAt(page, 534, 252); // Role Name field click
  await page.keyboard.type('Test');
  await checkScreenshot(page, 'role_name_input_field.png')

  await clickAt(page, 230, 115); // Undo command click
  await checkScreenshot(page, 'new_role_after_undo.png')

  await clickAt(page, 330, 115); // Redo command click
  await checkScreenshot(page, 'new_role_after_redo.png')

  await clickAt(page, 152, 568); // Click first checkbox (Parent Roles)
  await checkScreenshot(page, 'new_role_editor_first_checkbox.png')

  await clickAt(page, 152, 532); // Click all select checkbox (Parent Roles)
  await checkScreenshot(page, 'new_role_editor_all_select_checkbox.png')

  await clickAt(page, 152, 532); // Click all deselect checkbox (Parent Roles)
  await checkScreenshot(page, 'new_role_editor_all_deselect_checkbox.png')

  await clickAt(page, 787, 180); // General group hide
  await checkScreenshot(page, 'new_role_editor_hide_general.png')

  await clickAt(page, 167, 323); // Select first permission
  await checkScreenshot(page, 'new_role_editor_select_permission.png')

  await clickAt(page, 167, 323); // Deselect first permission
  await checkScreenshot(page, 'new_role_editor_deselect_permission.png')

  await clickAt(page, 107, 70); // Close dirty document
  await checkScreenshot(page, 'new_role_editor_close.png')
  await clickAt(page, 900, 440); // Cancel click

  await clickAt(page, 107, 70); // Close dirty document
  await clickAt(page, 834, 443); // No click
  await checkScreenshot(page, 'new_role_editor_after_closing.png')
});

test('Roles Edit command GUI test', async ({ page }) => {
  await clickAt(page, 1000, 135); // Text filter click
  await page.keyboard.type('sensor production manager');
  await delay(100);
  await clickAt(page, 665, 215); // Select first element
  await delay(100);
  await clickAt(page, 425, 85); // Edit command click
  await checkScreenshot(page, 'roles_editor_1.png');

  await page.mouse.move(700, 400);
  await wheelScroll(page, 1000);
  await checkScreenshot(page, 'roles_editor_2.png');
});


