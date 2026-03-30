// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

const { test } = require('@playwright/test');
const { reloadPage, checkScreenshot, clickOnPage, clickOnCommand, selectComboBox, clickOnButton} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "Workspace"); // Workspace page click
});

test('Start workspace page', async ({ page }) => {
  await checkScreenshot(page, '0-workspace-start.png')
});

test('User Actions', async ({ page }) => {
  await clickOnCommand(page, "UserActions"); // UserActions tab click
  await checkScreenshot(page, '1-workspace-user-actions.png')
});

test('Graphics Time Filter', async ({ page }) => {
  await clickOnButton(page, ["TimeFilterDelegate"]) // Date filter click
  await clickOnButton(page, ["TimeFilterParamView", "Month_Current"]) // This Month click
  await checkScreenshot(page, '2-workspace-time-filter.png')
});

test('Customer Filter', async ({ page }) => {
  await selectComboBox(page, "QUISS", ["CustomerFilterDelegate"])
  await checkScreenshot(page, '3-workspace-customer-filter.png')
});

test('Create New / View All', async ({ page }) => {
  await clickOnButton(page, ["SoftwareProductsInfo", "ViewAllButton"]) // Software 'View All' button click
  await checkScreenshot(page, '4-workspace-software-view-all.png')
  await clickOnPage(page, "Workspace"); // Workspace page click

  await clickOnButton(page, ["SoftwareProductsInfo", "CreateNewButton"]) // Software 'Create New' button click
  await checkScreenshot(page, '5-workspace-software-create-new.png')
  await clickOnPage(page, "Workspace"); // Workspace page click

  await clickOnButton(page, ["OrdersInfo", "ViewAllButton"]) // Order 'View All' button click
  await checkScreenshot(page, '8-workspace-order-view-all.png')
  await clickOnPage(page, "Workspace"); // Workspace page click

  await clickOnButton(page, ["OrdersInfo", "CreateNewButton"]) // Order 'Create New' button click
  await checkScreenshot(page, '9-workspace-order-create-new.png')
  await clickOnPage(page, "Workspace"); // Workspace page click
});

test('Analytics', async ({ page }) => {
  await clickOnCommand(page, "Analytics"); // Analytics tab click
  await checkScreenshot(page, '10-workspace-analytics.png')
});

