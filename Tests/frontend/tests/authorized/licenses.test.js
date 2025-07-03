const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, delay} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickAt(page, 40, 170); // Software page click
});

test('Start software page', async ({ page }) => {
  await checkScreenshot(page, 'start_software.png')
});

test('Filters test', async ({ page }) => {
  await clickAt(page, 215, 170); // License filter CB click
  await checkScreenshot(page, 'software_filters_1.png')

  await clickAt(page, 205, 240); // Select only paired from cb
  await checkScreenshot(page, 'software_filters_2.png')

  await clickAt(page, 215, 170); // License filter CB click
  await clickAt(page, 205, 270); // Select only not paired from cb
  await checkScreenshot(page, 'software_filters_3.png')

  await clickAt(page, 215, 170); // License filter CB click
  await clickAt(page, 205, 300); // Select file created from cb
  await checkScreenshot(page, 'software_filters_4.png')

  await clickAt(page, 540, 175); // Account filter CB click
  await checkScreenshot(page, 'software_filters_5.png')
  await clickAt(page, 540, 175); // Account filter CB click

  await clickAt(page, 1010, 175); // Date filter click
  await checkScreenshot(page, 'software_filters_6.png')
});

test('New software test', async ({ page }) => {
  await clickAt(page, 170, 115); // 'New' command clicked
  await checkScreenshot(page, 'licenses_new_command_filter.png')
});

test('Software editor test', async ({ page }) => {
  await selectFirstSoftware(page)
  await clickAt(page, 290, 120); // 'Edit' command clicked
  await checkScreenshot(page, 'software_editor.png')
});

test('Open order from sofwtare test', async ({ page }) => {
  await clickAt(page, 650, 470); // Select 7 element from table
  await clickAt(page, 1207, 118); // 'Open Order' command clicked
  await checkScreenshot(page, 'software_open_order.png')
});

test('Remove sofwtare test', async ({ page }) => {
  await selectFirstSoftware(page)
  await clickAt(page, 389, 117); // 'Remove' command clicked
  await checkScreenshot(page, 'remove_software_dialog.png')
});

async function selectFirstSoftware(page) {
  await clickAt(page, 600, 255); // Select first element from table
}

async function removeFocus(page) {
  await clickAt(page, 40, 30);
}
