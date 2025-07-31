const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, delay} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickAt(page, 40, 340); // Customers page click
});

test('Start customers page', async ({ page }) => {
  await checkScreenshot(page, 'start_customers.png')
});

test('Customers hidden commands test', async ({ page }) => {
  await clickAt(page, 1385, 120); // Hidden commands click
  await checkScreenshot(page, 'customers_hidden_commands.png')
});

test('Customers filters GUI test', async ({ page }) => {
  await clickAt(page, 1184, 165); // Text filter click
  await page.keyboard.type('ias');
  await checkScreenshot(page, 'customers_text_filter.png')

  await clickAt(page, 250, 165); // Document state filter click
  await checkScreenshot(page, 'customers_document_state_filter.png')

  await clickAt(page, 290, 230); // Select disabled documents
  await checkScreenshot(page, 'customers_document_disabled_filter.png')

  await removeFocus(page);

  await clickAt(page, 130, 165); // Date filter click
  await checkScreenshot(page, 'customers_date_filter.png')

  await clickAt(page, 1383, 207); // Hide filter button click
  await checkScreenshot(page, 'customers_hidden_filter.png')
});

test('Remove customer GUI test', async ({ page }) => {
  await clickAt(page, 280, 245); // First customer in table click
  await clickAt(page, 360, 120); // Remove command click
  await checkScreenshot(page, 'customers_remove_command.png')
});

test('New customer GUI test', async ({ page }) => {
  await clickAt(page, 120, 125); // New command click
  await checkScreenshot(page, 'customers_new_editor_0.png')

  await clickAt(page, 790, 180); // Customer information group hide click
  await checkScreenshot(page, 'customers_new_editor_1.png')

  await clickAt(page, 790, 252); // Account information group hide click
  await checkScreenshot(page, 'customers_new_editor_2.png')

  await clickAt(page, 790, 325); // Company address information group hide click
  await checkScreenshot(page, 'customers_new_editor_3.png')
});

test('Edit customer GUI test', async ({ page }) => {
  await clickAt(page, 280, 245); // First customer in table click

  await clickAt(page, 275, 116); // Edit command click
  await checkScreenshot(page, 'customers_editor.png')

  await clickAt(page, 790, 180); // Customer information group hide click
  await checkScreenshot(page, 'customers_editor_1.png')

  await clickAt(page, 790, 252); // Account information group hide click
  await checkScreenshot(page, 'customers_editor_2.png')

  await clickAt(page, 790, 325); // Company address information group hide click
  await checkScreenshot(page, 'customers_editor_3.png')
});

test('Customers revision dialog GUI test', async ({ page }) => {
  await clickAt(page, 280, 245); // First customer in table click

  await clickAt(page, 1300, 116); // Revision command click
  await checkScreenshot(page, 'customers_revision_dialog.png')

  await clickAt(page, 545, 160); // First revision select
  await checkScreenshot(page, 'customers_first_revision.png')

  await clickAt(page, 556, 193); // Second revision select
  await checkScreenshot(page, 'customers_second_revision.png')
});

test('Customers sort collection GUI test', async ({ page }) => {
  await clickAt(page, 185, 210); // Customer-ID column click
  await checkScreenshot(page, 'customer_id_asc.png')

  await clickAt(page, 185, 210); // Customer-ID column click
  await checkScreenshot(page, 'customer_id_desc.png')

  await clickAt(page, 540, 210); // Account Name column click
  await checkScreenshot(page, 'account_name_asc.png')

  await clickAt(page, 540, 210); // Account Name column click
  await checkScreenshot(page, 'account_name_desc.png')

  await clickAt(page, 875, 210); // Email column click
  await checkScreenshot(page, 'email_asc.png')

  await clickAt(page, 875, 210); // Email column click
  await checkScreenshot(page, 'email_desc.png')

  await clickAt(page, 1160, 210); // Description column click
  await checkScreenshot(page, 'description_asc.png')

  await clickAt(page, 1160, 210); // Description column click
  await checkScreenshot(page, 'description_desc.png')

  await clickAt(page, 807, 785); // Pagination 50 elements click
  await checkScreenshot(page, 'customers_pagination_50.png')
});

async function removeFocus(page) {
  await clickAt(page, 40, 30);
}
