// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, clickOnPage, clickOnButton, clickOnCommand, selectComboBox} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "SoftwareProducts"); // SoftwareProducts page click
});

test('Start software page', async ({ page }) => {
  await checkScreenshot(page, 'start_software.png')
});

test('Filters test', async ({ page }) => {
  await selectComboBox(page, "Showonlypairedlicenses", ["FilterPanel", "LicenseStatusFilter"])
  await checkScreenshot(page, 'software_filters_2.png')

  await selectComboBox(page, "Showonlynotpairedlicenses", ["FilterPanel", "LicenseStatusFilter"])
  await checkScreenshot(page, 'software_filters_3.png')

  await selectComboBox(page, "Showthelicensesforwhichthefilewascreated", ["FilterPanel", "LicenseStatusFilter"])
  await checkScreenshot(page, 'software_filters_4.png')

  await selectComboBox(page, "QUISS", ["FilterPanel", "CustomersFilter"])// Customers filter CB click
  await checkScreenshot(page, 'software_filters_5.png')

  await clickOnButton(page, ["FilterPanel", "CreationDateFilter"]) // Date filter CB click
  await checkScreenshot(page, 'software_filters_6.png', { path: ["TimeRangeView"] })
});

test('New software test', async ({ page }) => {
  await clickOnCommand(page, "New") // 'New' command clicked
  await checkScreenshot(page, 'licenses_new_command_filter.png')
});

test('Software editor test', async ({ page }) => {
  await selectFirstSoftware(page)
  await clickOnCommand(page, "Edit") // 'New' command clicked
  await checkScreenshot(page, 'software_editor.png')
});

test('Remove sofwtare test', async ({ page }) => {
  await selectFirstSoftware(page)
  await clickOnCommand(page, "Remove") // 'New' command clicked
  await checkScreenshot(page, 'remove_software_dialog.png')
});

async function selectFirstSoftware(page) {
  await clickAt(page, 600, 255); // Select first element from table
}