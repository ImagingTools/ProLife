const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, wheelScroll, clickOnPage, clickOnButton} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "Administration")// Administration page click
  await clickOnButton(page, ["AdministrationView", "Users"])// Users subpage click
});

test('Start users page', async ({ page }) => {
  await checkScreenshot(page, 'start_users.png')
});

test('Users sort tests', async ({ page }) => {
  let mask = { x: 1015, y: 200, width: 150, height: 565 }
  await clickAt(page, 1084, 180); // Last Connection column click
  await checkScreenshot(page, 'user_last_connection_asc.png', mask)

  await clickAt(page, 1084, 180); // Last Connection column click
  await checkScreenshot(page, 'user_last_connection_desc.png', mask)

  await clickAt(page, 462, 180); // Email column click
  await checkScreenshot(page, 'user_email_asc.png', mask)

  await clickAt(page, 462, 180); // Email column click
  await checkScreenshot(page, 'user_email_desc.png', mask)

  await clickAt(page, 320, 180); // Name column click
  await checkScreenshot(page, 'user_name_asc.png', mask)

  await clickAt(page, 320, 180); // Name column click
  await checkScreenshot(page, 'user_name_desc.png', mask)
});

test('Users revision command test', async ({ page }) => {
  await clickAt(page, 490, 215); // Select first element
  await clickAt(page, 1130, 85); // Revision command click
  await checkScreenshot(page, 'users_revision_dialog.png')
});

test('Users new command test', async ({ page }) => {
  await clickAt(page, 490, 215); // Select first element
  await clickAt(page, 330, 85); // New command click
  await checkScreenshot(page, 'users_new_editor.png')

  await clickAt(page, 470, 250); // Username text input click
  await page.keyboard.type('Test');

  await clickAt(page, 470, 335); // Name text input click
  await page.keyboard.type('Test');

  await clickAt(page, 470, 415); // Email text input click
  await page.keyboard.type('Test');
  await checkScreenshot(page, 'users_incorrect_email.png')

  await clickAt(page, 470, 415); // Email text input click
  await page.keyboard.type('@mail.ru');
  await checkScreenshot(page, 'users_correct_email.png')

  await clickAt(page, 470, 520); // Password text input click
  await page.keyboard.type('1');

  await clickAt(page, 470, 600); // Confirm password text input click
  await page.keyboard.type('2');
  await checkScreenshot(page, 'users_invalid_password.png')

  await clickAt(page, 470, 600); // Confirm password text input click
  await page.keyboard.press('Control+A');
  await page.keyboard.press('Delete');
  await page.keyboard.type('1');
  await checkScreenshot(page, 'users_valid_password.png')

  await wheelScroll(page, 500)
  await checkScreenshot(page, 'user_new_editor_2.png')

  await wheelScroll(page, 500)
  await checkScreenshot(page, 'user_new_editor_3.png')

  await wheelScroll(page, 500)
  await checkScreenshot(page, 'user_new_editor_4.png')
});

test('Users remove command GUI test', async ({ page }) => {
  await clickAt(page, 490, 215); // Select first element

  await clickAt(page, 525, 85); // Remove command click
  await checkScreenshot(page, 'users_remove_dialog.png')

  await clickAt(page, 866, 444); // Click 'No' button in dialog
  await checkScreenshot(page, 'users_after_no_remove.png')
});

test('Users edit command GUI test', async ({ page }) => {
  await clickAt(page, 490, 215); // Select first element

  await clickAt(page, 425, 85); // Edit command click
  await checkScreenshot(page, 'user_editor_1.png')

  await page.mouse.move(700, 400);
  
  await wheelScroll(page, 500)
  await checkScreenshot(page, 'user_editor_2.png')

  await wheelScroll(page, 500)
  await checkScreenshot(page, 'user_editor_3.png')

  await wheelScroll(page, 500)
  await checkScreenshot(page, 'user_editor_4.png')
});