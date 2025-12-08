const {expect } = require('@playwright/test');
const delay = (time = 5000) => new Promise(resolve => setTimeout(resolve, time));
const fs = require('fs');
const path = require('path');

var frameCount = 0;


const clickOnElement = async (page, path) => {
  // Находим элемент по пути и ждём появления
  const locator = page.locator(createStrPath(path) + ' [objectName="MouseArea"][visible]').first();
  await locator.waitFor({ timeout: 5000 });

  // Скроллим к нему (иногда нужно)
  await locator.scrollIntoViewIfNeeded();

  // Получаем реальный DOM-element-хэндл
  const element = await locator.elementHandle();
  if (!element) {
    throw new Error("ElementHandle is null for path: " + path);
  }

  // Получаем координаты
  const box = await element.boundingBox();
  if (!box) {
    throw new Error("boundingBox is null for path: " + path);
  }

  // Вычисляем центр
  const x = box.x + box.width / 2;
  const y = box.y + box.height / 2;

  // Кликаем по координатам
  await clickAt(page, x, y)
};

const clickOnPage = async (page, pageId) => {
  await clickOnElement(page, ["MenuPanel", pageId + "Button"])
}

const clickOnCommand = async (page, commandId) => {
  await clickOnElement(page, ["CommandsView", commandId + "Button"])
}

const clickOnButton = async (page, buttonPath) => {
  await clickOnElement(page, buttonPath)
}

function createStrPath(path) {
  let result = ""

  for (let i = 0; i < path.length; ++i){
    if (result.length > 0){
      result += ' '
    }
    result += '[objectName="' + path[i] + '"]'
  }

  return result
}

const fillTextInput = async (page, text, path) => {
  const textInput = await page.$(createStrPath(path) + ' [objectName="TextInput"] input')
  if (textInput) {
    const rect = await textInput.boundingBox()
    await clickAt(rect.x + rect.width / 2, rect.y + rect.height / 2);
    await page.keyboard.type(text);
  }  
}

const selectComboBox = async (page, selectedText, path) => {
  await clickOnElement(page, path)
  await clickOnElement(page, ["PopupMenuDialog", selectedText])
}

const reloadPage = async (page, url = 'http://localhost:7778') => {
  await page.goto(url);
  await waitForPageStability(page);
};

const clickAt = async (page, x, y) => {
  await page.mouse.click(x, y);
  await waitForPageStability(page);
};

const wheelScroll = async (page, deltaY) => {
  await page.mouse.wheel(0, deltaY);
};

const checkScreenshot = async (page, filename, maskParams) => {
  await addMask(page, maskParams);
  await waitForPageStability(page);
  await expect(page).toHaveScreenshot(filename, { fullPage: true, threshold: 0.05, maxDiffPixelRatio: 0});
  await removeMask(page);
};

async function login(page, username, password) {
  await reloadPage(page);

  await clickAt(page, 700, 386); // Click 'Login' input field
  await page.keyboard.type(username);

  await clickAt(page, 685, 455); // Click 'Password' input field
  await page.keyboard.type(password);

  await delay(500);

  await clickAt(page, 700, 600); // Click 'Login' button
}

async function waitForDomStability(page, options = {}) {
  const {
    idleTime = 500,
    timeout = 5000,
    checkInterval = 100,
  } = options;

  const startTime = Date.now();
  let lastHtml = '';
  let stableSince = Date.now();

  while (Date.now() - startTime < timeout) {
    try {
      const currentHtml = await page.evaluate(() => document.documentElement.outerHTML);

      if (currentHtml !== lastHtml) {
        lastHtml = currentHtml;
        stableSince = Date.now();
      } else if (Date.now() - stableSince >= idleTime) {
        return;
      }

      await page.waitForTimeout(checkInterval);
    } catch (e) {
      if (e.message.includes('Execution context was destroyed')) {
        // Страница ушла — ждём новую загрузку и сбрасываем состояние
        await page.waitForLoadState('domcontentloaded');
        lastHtml = '';
        stableSince = Date.now();
        continue;
      }
      throw e;
    }
  }

  // throw new Error(`Page did not stabilize within ${timeout}ms`);
}

async function waitForPageStability(page, options = {}) {
  const {
    maxTotalTime = 5000,
    domStableTime = 800,
    networkIdleTimeout = 2000,
    debug = false,
  } = options;

  await waitForDomStability(page, {
    idleTime: domStableTime,
    timeout: maxTotalTime,
    checkInterval: 100
  });

  if (debug) {
    console.log('Page stabilized');
  }
}

const addMask = async (page, maskParams) => {
  if (!maskParams) return;

  await page.evaluate(({ x, y, width, height }) => {
    const mask = document.createElement('div');
    Object.assign(mask.style, {
      position: 'fixed',
      top: (typeof y === 'number' ? y + 'px' : y) || '0px',
      left: (typeof x === 'number' ? x + 'px' : x) || '0px',
      width: (typeof width === 'number' ? width + 'px' : width) || '100px',
      height: (typeof height === 'number' ? height + 'px' : height) || '100px',
      backgroundColor: '#000000',
      zIndex: '9999',
      pointerEvents: 'none',
    });
    mask.setAttribute('data-mask', 'true');
    document.body.appendChild(mask);
  }, maskParams);

  await page.waitForFunction(() => !!document.querySelector('div[data-mask="true"]'));
};

const removeMask = async (page) => {
  await page.evaluate(() => {
    const mask = document.querySelector('div[data-mask="true"]');
    if (mask) mask.remove();
  });
};

module.exports = { delay, reloadPage, clickAt, checkScreenshot, login, wheelScroll, waitForPageStability, clickOnPage, clickOnCommand, selectComboBox, fillTextInput, clickOnButton};