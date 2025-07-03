const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot } = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickAt(page, 40, 240); // Orders page click
});

test('Start orders page', async ({ page }) => {
  await checkScreenshot(page, 'start_orders.png')
});

test('Orders filters test', async ({ page }) => {
  await clickAt(page, 210, 175); // Account filter CB click
  await checkScreenshot(page, 'orders_account_filter.png')
});

test('New order test', async ({ page }) => {
  await clickAt(page, 160, 120); // 'New' command clicked
  await checkScreenshot(page, 'orders_new_editor.png')

  await clickAt(page, 505, 250); // Delivery-ID text input clicked
  await page.keyboard.type('Testr');
  await checkScreenshot(page, 'orders_new_editor_0.png')

  await clickAt(page, 505, 250); // Delivery-ID text input clicked
  await page.keyboard.press('Control+A');
  await page.keyboard.press('Delete');
  await page.keyboard.type('4444');
  await checkScreenshot(page, 'orders_new_editor_1.png')

  await clickAt(page, 505, 250); // Delivery-ID text input clicked
  await page.keyboard.press('Control+A');
  await page.keyboard.press('Delete');
  await page.keyboard.type('444444');
  await checkScreenshot(page, 'orders_new_editor_2.png')

  await clickAt(page, 505, 250); // Delivery-ID text input clicked
  await page.keyboard.press('Control+A');
  await page.keyboard.press('Delete');
  await page.keyboard.type('44444');
  await checkScreenshot(page, 'orders_new_editor_3.png')

  await clickAt(page, 505, 250); // Delivery-ID text input clicked
  await page.keyboard.press('Control+A');
  await page.keyboard.press('Delete');
  await page.keyboard.type('44444444');
  await checkScreenshot(page, 'orders_new_editor_4.png')

  await clickAt(page, 505, 490); // Customers CB click
  await checkScreenshot(page, 'orders_new_editor_5.png')

  await clickAt(page, 505, 530); // Select first customer
  await checkScreenshot(page, 'orders_new_editor_6.png')

  await clickAt(page, 800, 650); // 'Add new product' button clicked
  await checkScreenshot(page, 'orders_new_editor_7.png')

  await clickAt(page, 700, 250); // Product cb click
  await checkScreenshot(page, 'orders_new_editor_8.png')

  await clickAt(page, 700, 285); // Select first elem
  await checkScreenshot(page, 'orders_new_editor_9.png')

  await clickAt(page, 700, 415); // License cb click
  await checkScreenshot(page, 'orders_new_editor_10.png')

  await clickAt(page, 700, 450); // Select first elem
  await checkScreenshot(page, 'orders_new_editor_11.png')

  await clickAt(page, 1045, 340); // New license check box click
  await checkScreenshot(page, 'orders_new_editor_12.png')

  await clickAt(page, 700, 415); // License type combobox click
  await checkScreenshot(page, 'orders_new_editor_13.png')

  await clickAt(page, 700, 445); // Select first elem
  await checkScreenshot(page, 'orders_new_editor_14.png')

  await clickAt(page, 775, 570); // Expiration check box click
  await checkScreenshot(page, 'orders_new_editor_15.png')

  await clickAt(page, 1020, 85); // Hardware category click
  await checkScreenshot(page, 'orders_new_editor_16.png')

  await clickAt(page, 700, 250); // Product combobox click
  await checkScreenshot(page, 'orders_new_editor_17.png')

  await clickAt(page, 700, 285); // Select first elem
  await checkScreenshot(page, 'orders_new_editor_18.png')

  await clickAt(page, 700, 415); // Hardware-ID combobox click
  await checkScreenshot(page, 'orders_new_editor_19.png')

  await clickAt(page, 700, 450); // Select first elem
  await checkScreenshot(page, 'orders_new_editor_20.png')

  await clickAt(page, 1047, 340); // New sensor checkbox click
  await checkScreenshot(page, 'orders_new_editor_21.png')

  await clickAt(page, 700, 415); // Type combobox click
  await checkScreenshot(page, 'orders_new_editor_22.png')

  await clickAt(page, 700, 450); // Select first elem
  await checkScreenshot(page, 'orders_new_editor_23.png')

  await clickAt(page, 915, 85); // Software category click
  await checkScreenshot(page, 'orders_new_editor_24.png')
});

