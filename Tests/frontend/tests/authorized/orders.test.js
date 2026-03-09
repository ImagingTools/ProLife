// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, clickOnPage, selectComboBox, delay, wheelScroll} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "Orders"); // Orders page click
});

test('Start orders page', async ({ page }) => {
  await checkScreenshot(page, 'start_orders.png')
});

test('Orders filters test', async ({ page }) => {
  await selectComboBox(page, "IAS", ["FilterPanel", "CustomersFilter"])// Customers filter CB click
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

  await clickAt(page, 700, 610); // License type combobox click
  await checkScreenshot(page, 'orders_new_editor_13.png')

  await clickAt(page, 700, 640); // Select first elem
  await checkScreenshot(page, 'orders_new_editor_14.png')

  await wheelScroll(page, 500)

  await clickAt(page, 765, 600); // Expiration check box click
  await checkScreenshot(page, 'orders_new_editor_15.png', {path: ["ExpirationControl"]})

  await wheelScroll(page, -500)

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

  await clickAt(page, 700, 465); // Type combobox click
  await checkScreenshot(page, 'orders_new_editor_22.png')

  await clickAt(page, 700, 500); // Select first elem
  await checkScreenshot(page, 'orders_new_editor_23.png')

  await clickAt(page, 915, 85); // Software category click
  await checkScreenshot(page, 'orders_new_editor_24.png')
});

test('Edit order test (add new hardware product)', async ({ page }) => {
  await clickAt(page, 1190, 170); // 'Text filter clicked
  await page.keyboard.type('10942314');
  await checkScreenshot(page, 'orders_edit_h_1.png')

  await clickAt(page, 700, 255); // First element click
  await clickAt(page, 265, 115); // Edit command click
  await checkScreenshot(page, 'orders_edit_h_2.png')

  await clickAt(page, 790, 180); // Hide order information group
  await checkScreenshot(page, 'orders_edit_h_3.png')

  await clickAt(page, 767, 250); // Compact view button click
  await checkScreenshot(page, 'orders_edit_h_4.png')

  await clickAt(page, 767, 250); // Detailed view button click
  await checkScreenshot(page, 'orders_edit_h_5.png')

  await clickAt(page, 750, 325); // First product edit click
  await checkScreenshot(page, 'orders_edit_h_6.png')

  await clickAt(page, 1050, 770); // Cancel editor click
  await checkScreenshot(page, 'orders_edit_h_7.png')

  await clickAt(page, 750, 550); // Second product edit click
  await checkScreenshot(page, 'orders_edit_h_8.png')

  await clickAt(page, 1050, 770); // Cancel editor click

  await clickAt(page, 800, 250); // New Product click
  await clickAt(page, 1015, 85); // Hardware category click
  await clickAt(page, 700, 250); // Product CB click
  await clickAt(page, 700, 290); // First item click from CB
  await clickAt(page, 1045, 335); // New Sensor switch click
  await clickAt(page, 700, 470); // Types CB click
  await clickAt(page, 700, 505); // First item click from CB
  await clickAt(page, 700, 545); // MAC Address text input click
  await page.keyboard.type('8c:1f:64:50:19:1b'); // Mac Address already exists in order
  await checkScreenshot(page, 'orders_edit_h_9.png')

  await clickAt(page, 700, 545); // MAC Address text input click
  await page.keyboard.press('Control+A');
  await page.keyboard.press('Delete');
  await page.keyboard.type('8c:1f:64:50:19:20'); // Valid Mac Address
  await checkScreenshot(page, 'orders_edit_h_10.png')

  await clickAt(page, 700, 625); // Serial Number text input click
  await page.keyboard.type('S8C1F6450191B'); // Serial Number already exists in order
  await checkScreenshot(page, 'orders_edit_h_11.png')

  await clickAt(page, 700, 625); // Serial Number text input click
  await page.keyboard.press('Control+A');
  await page.keyboard.press('Delete');
  await page.keyboard.type('77777');
  await checkScreenshot(page, 'orders_edit_h_12.png')

  await clickAt(page, 980, 770); // Ok button click
  await checkScreenshot(page, 'orders_edit_h_13.png')

  await clickAt(page, 750, 320); // Click edit for new hardware product
  await checkScreenshot(page, 'orders_edit_h_14.png')
});

test('Edit order test (add new software product)', async ({ page }) => {
  await clickAt(page, 1190, 170); // 'Text filter clicked
  await page.keyboard.type('33333');
  await checkScreenshot(page, 'orders_edit_s_0.png')
  await clickAt(page, 700, 255); // First element click
  await clickAt(page, 265, 115); // Edit command click

  await clickAt(page, 790, 180); // Hide order information group

  await clickAt(page, 750, 325); // First product edit click
  await checkScreenshot(page, 'orders_edit_s_1.png')

  await clickAt(page, 1050, 770); // Cancel editor click

  await clickAt(page, 750, 550); // Second product edit click
  await checkScreenshot(page, 'orders_edit_s_2.png')

  await clickAt(page, 1050, 770); // Cancel editor click

  await clickAt(page, 800, 250); // New Product click
  await clickAt(page, 700, 250); // Product CB click
  await clickAt(page, 700, 290); // First item click from CB
  await clickAt(page, 1045, 335); // New License switch click
  await clickAt(page, 700, 610); // Licenses CB click
  await clickAt(page, 700, 640); // First item click from CB
  await clickAt(page, 700, 690); // Software-ID text input click
  await page.keyboard.type('9989897'); // Software-ID already exists in order
  await delay(500);
  await checkScreenshot(page, 'orders_edit_s_3.png')

  await clickAt(page, 700, 690); // Software-ID text input click
  await page.keyboard.press('Control+A');
  await page.keyboard.press('Delete');
  await page.keyboard.type('9989810'); // Valid Software-ID
  await checkScreenshot(page, 'orders_edit_s_4.png')

  await clickAt(page, 980, 770); // Ok button click
  await checkScreenshot(page, 'orders_edit_s_5.png')

  await clickAt(page, 750, 320); // Click edit for new software product
  await checkScreenshot(page, 'orders_edit_s_6.png')
});
