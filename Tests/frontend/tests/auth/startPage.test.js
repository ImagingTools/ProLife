const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
});

test('Start page window should match screenshot', async ({ page }) => {
  await checkScreenshot(page, 'start_page_window.png')
});

test('User popup visibility test', async ({ page }) => {
  await clickUserButton(page); // User button click

  await checkScreenshot(page, 'user_popup.png')

  await clickAt(page, 1209, 61); // Profile click

  await page.waitForLoadState('networkidle'); 

  await checkScreenshot(page, 'user_profile.png')

  await clickAt(page, 970, 524); // Change password click

  await checkScreenshot(page, 'change_password_dialog.png')
});

test('Settings test', async ({ page }) => {
  await clickSettingsButton(page); // Settings button click

  await checkScreenshot(page, 'settings_dialog.png')

  await clickAt(page, 310, 205); // Page about click

  await checkScreenshot(page, 'settings_about_page.png')
});

test('User popup logout test', async ({ page }) => {
  await clickUserButton(page);// User button click

  await clickAt(page, 1200, 96); // Logout click

  await checkScreenshot(page, 'user_logout.png')
});

async function clickUserButton(page) {
  await clickAt(page, 1339, 25); // User button click
}

async function clickSettingsButton(page) {
  await clickAt(page, 1371, 25);
}