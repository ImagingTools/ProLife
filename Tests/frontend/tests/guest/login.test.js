// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, login} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
});

test('Login window should match screenshot', async ({ page }) => {
	await checkScreenshot(page, 'login_window.png');
});

test('Should show error on invalid login', async ({ page }) => {
  await login(page, "su", "2");
  await checkScreenshot(page, 'invalid_login_window.png');
});

test('Should go through password recovery flow', async ({ page }) => {
  let defaultMask = { x: 580, y: 400, width: 100, height: 40 };

  await clickAt(page, 797, 491); // Click 'Recovery Password' button
  await checkScreenshot(page, 'password_recovery_window.png');

  await clickAt(page, 666, 250); // Click 'Email' input field
  await page.keyboard.type('su@mail.ru');

  await clickAt(page, 870, 675); // Click 'Next' button

  await checkScreenshot(page, 'confirm_email.png');

  await clickAt(page, 920, 677, defaultMask); // Click 'Yes' button

  await checkScreenshot(page, 'check_code_window.png', defaultMask);
});

test('Should open register window', async ({ page }) => {
  await clickAt(page, 813, 520); // Click 'Register user' button
  await checkScreenshot(page, 'register_user_window.png');
});

test('Superuser authorization', async ({ page }) => {
  await login(page, "su", "1");
  await checkScreenshot(page, 'succesful_login.png');
});
