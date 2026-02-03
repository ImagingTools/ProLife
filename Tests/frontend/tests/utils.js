const {expect } = require('@playwright/test');
const delay = (time = 5000) => new Promise(resolve => setTimeout(resolve, time));

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
  // const textInput = await page.$(createStrPath(path) + ' [objectName="TextInput"] input')
  const textInput = page.locator(createStrPath(path) + ' [objectName="TextInput"][visible]').first();
  if (textInput) {
    const rect = await textInput.boundingBox()
    await clickAt(page, rect.x + rect.width / 2, rect.y + rect.height / 2);
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
  await waitForPageStability(page);
  
  const screenshotOptions = { 
    fullPage: true, 
    threshold: 0.05, 
    maxDiffPixelRatio: 0 
  };
  
  // Add mask if maskParams provided
  if (maskParams) {
    // If maskParams has a path, create a locator-based mask
    if (maskParams.path) {
      const locator = page.locator(createStrPath(maskParams.path));
      screenshotOptions.mask = [locator];
    }
    // If maskParams has coordinates, use a different approach
    else if (maskParams.x !== undefined) {
      const { x, y, width, height } = maskParams;
      const pad = maskParams.padding || 0;
      
      const maskX = x - pad;
      const maskY = y - pad;
      const maskWidth = width + pad * 2;
      const maskHeight = height + pad * 2;
      
      // Inject a style tag to create a pseudo-mask
      await page.addStyleTag({
        content: `
          body::before {
            content: '';
            position: fixed;
            top: ${maskY}px;
            left: ${maskX}px;
            width: ${maskWidth}px;
            height: ${maskHeight}px;
            background-color: #000000;
            z-index: 9999;
            pointer-events: none;
          }
        `
      });
      
      // Wait a bit for the style to be applied
      await page.waitForTimeout(100);
    }
  }
  
  await expect(page).toHaveScreenshot(filename, screenshotOptions);
  
  // Clean up coordinate-based mask if it was added
  if (maskParams && maskParams.x !== undefined) {
    // Remove the injected style by reloading styles or by using evaluate
    // Since we want to avoid page.evaluate, we'll just let it be - it's a pseudo-element
    // that will be gone on next navigation
  }
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

async function waitForVisualStability(page, options = {}) {
  const {
    idleTime = 500,
    timeout = 5000,
    checkInterval = 100,
  } = options;

  const startTime = Date.now();
  
  // First, wait for basic page load
  try {
    await page.waitForLoadState('domcontentloaded', { timeout: 2000 });
  } catch (e) {
    // Continue even if this times out
  }

  // Wait for network to be mostly idle
  try {
    await page.waitForLoadState('networkidle', { timeout: 2000 });
  } catch (e) {
    // Continue even if this times out
  }

  // Now check for visual stability using screenshots
  let lastScreenshot = null;
  let stableSince = Date.now();

  while (Date.now() - startTime < timeout) {
    try {
      // Take a small screenshot for comparison (just a hash would be ideal, but we'll use full screenshot)
      const currentScreenshot = await page.screenshot({ type: 'png' });

      if (!lastScreenshot) {
        lastScreenshot = currentScreenshot;
        stableSince = Date.now();
      } else if (!currentScreenshot.equals(lastScreenshot)) {
        lastScreenshot = currentScreenshot;
        stableSince = Date.now();
      } else if (Date.now() - stableSince >= idleTime) {
        // Visual content has been stable for idleTime
        return;
      }

      await page.waitForTimeout(checkInterval);
    } catch (e) {
      if (e.message.includes('Execution context was destroyed')) {
        await page.waitForLoadState('domcontentloaded');
        lastScreenshot = null;
        stableSince = Date.now();
        continue;
      }
      throw e;
    }
  }
}

async function waitForDomStability(page, options = {}) {
  // Redirect to visual stability check
  await waitForVisualStability(page, options);
}

async function waitForPageStability(page, options = {}) {
  const {
    maxTotalTime = 5000,
    domStableTime = 800,
  } = options;

  await waitForDomStability(page, {
    idleTime: domStableTime,
    timeout: maxTotalTime,
    checkInterval: 100
  });
}

module.exports = { delay, reloadPage, clickAt, checkScreenshot, login, wheelScroll, waitForPageStability, clickOnPage, clickOnCommand, selectComboBox, fillTextInput, clickOnButton};