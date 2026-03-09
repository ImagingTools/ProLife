// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

const { chromium } = require('@playwright/test');
const { waitForPageStability, delay, login} = require('./tests/utils');

module.exports = async () => {
  const browser = await chromium.launch();

  const context = await browser.newContext({
    viewport: { width: 1400, height: 800 },  // Указываем размер экрана
  });

  const page = await context.newPage();

  await page.goto('http://localhost:7778');
  await waitForPageStability(page)

  await login(page, "su", "1")

  await waitForPageStability(page)

  // Сохраняем состояние
  await context.storageState({ path: './storageState.json' });

  await browser.close();
};
