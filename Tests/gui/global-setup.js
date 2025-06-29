const { chromium } = require('@playwright/test');
const { clickAt, delay, login} = require('./tests/utils');

module.exports = async () => {
  const browser = await chromium.launch();

  const context = await browser.newContext({
    viewport: { width: 1400, height: 800 },  // Указываем размер экрана
  });

  const page = await context.newPage();

  await page.goto('http://localhost:7778');

  await page.waitForLoadState('networkidle'); 

  await delay()

  await login(page, "su", "1")

  await delay()
  await page.waitForLoadState('networkidle'); // Всё догрузилось

  // Сохраняем состояние
  await context.storageState({ path: './storageState.json' });

  await browser.close();
};
