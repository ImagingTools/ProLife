const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, wheelScroll} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickAt(page, 40, 750); // Administration page click
  await clickAt(page, 173, 165); // Groups subpage click
});

test('Start groups page', async ({ page }) => {
  await checkScreenshot(page, 'start_groups.png')
});

test('Groups sort tests', async ({ page }) => {
  await clickAt(page, 914, 180); // Description column click
  await checkScreenshot(page, 'groups_description_asc.png')

  await clickAt(page, 914, 180); // Description column click
  await checkScreenshot(page, 'groups_description_desc.png')

  await clickAt(page, 440, 180); // Name column click
  await checkScreenshot(page, 'groups_name_asc.png')

  await clickAt(page, 440, 180); // Name column click
  await checkScreenshot(page, 'groups_name_desc.png')
});

test('Groups revision command test', async ({ page }) => {
  await clickAt(page, 490, 215); // Select first element
  await clickAt(page, 1130, 85); // Revision command click
  await checkScreenshot(page, 'groups_revision_dialog.png')
});

test('Groups new command test', async ({ page }) => {
  await clickAt(page, 490, 215); // Select first element
  await clickAt(page, 330, 85); // New command click
  await checkScreenshot(page, 'groups_new_editor.png')

  await clickAt(page, 470, 250); // Group Name text input click
  await page.keyboard.type('Test');
  await checkScreenshot(page, 'groups_new_editor_name_field.png')

  await clickAt(page, 470, 325); // Description input click
  await page.keyboard.type('Test');
  await checkScreenshot(page, 'groups_new_editor_description_field.png')

  await clickAt(page, 153, 451); // CLick checkbox all parent groups
  await checkScreenshot(page, 'groups_new_editor_checkbox_all.png')

  await clickAt(page, 226, 115); // Undo command click
  await checkScreenshot(page, 'groups_new_editor_after_undo_1.png')

  await clickAt(page, 226, 115); // Undo command click
  await checkScreenshot(page, 'groups_new_editor_after_undo_2.png')

  await clickAt(page, 226, 115); // Undo command click
  await checkScreenshot(page, 'groups_new_editor_after_undo_3.png')

  await clickAt(page, 315, 115); // Redo command click
  await checkScreenshot(page, 'groups_new_editor_after_redo_1.png')

  await clickAt(page, 315, 115); // Redo command click
  await checkScreenshot(page, 'groups_new_editor_after_redo_2.png')

  await clickAt(page, 315, 115); // Redo command click
  await checkScreenshot(page, 'groups_new_editor_after_redo_3.png')

  await wheelScroll(page, 500)
  await checkScreenshot(page, 'groups_new_editor_2.png')

  await wheelScroll(page, 500)
  await checkScreenshot(page, 'groups_new_editor_3.png')

  await wheelScroll(page, 500)
  await checkScreenshot(page, 'groups_new_editor_4.png')

  await wheelScroll(page, 500)
  await checkScreenshot(page, 'groups_new_editor_5.png')

  await clickAt(page, 107, 70); // Close dirty document
  await checkScreenshot(page, 'new_group_editor_close.png')
  await clickAt(page, 900, 440); // Cancel click

  await clickAt(page, 107, 70); // Close dirty document
  await clickAt(page, 834, 443); // No click
  await checkScreenshot(page, 'new_group_editor_after_closing.png')
});

test('Groups remove command GUI test', async ({ page }) => {
  await clickAt(page, 490, 215); // Select first element

  await clickAt(page, 525, 85); // Remove command click
  await checkScreenshot(page, 'groups_remove_dialog.png')

  await clickAt(page, 866, 444); // Click 'No' button in dialog
  await checkScreenshot(page, 'groups_after_no_remove.png')
});

test('Groups edit command GUI test', async ({ page }) => {
  await clickAt(page, 490, 215); // Select first element

  await clickAt(page, 425, 85); // Edit command click
  await checkScreenshot(page, 'group_editor_1.png')

  await page.mouse.move(700, 400);
  
  await wheelScroll(page, 700)
  await checkScreenshot(page, 'group_editor_2.png')

  await wheelScroll(page, 700)
  await checkScreenshot(page, 'group_editor_3.png')

  await wheelScroll(page, 700)
  await checkScreenshot(page, 'group_editor_4.png')
});

test('Groups filter tests', async ({ page }) => {
  await clickAt(page, 1000, 135); // Text filter click
  await page.keyboard.type('ias');
  await checkScreenshot(page, 'groups_text_filter.png')

  await clickAt(page, 420, 135); // Document state filter click
  await checkScreenshot(page, 'groups_filters_document_state.png')

  await clickAt(page, 45, 25); // Remove focus

  await clickAt(page, 300, 135); // Date filter click
  await checkScreenshot(page, 'groups_filters_date.png', { x: 275, y: 325, width: 240, height: 100 })
});