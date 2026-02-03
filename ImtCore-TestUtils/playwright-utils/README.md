# @imtcore/playwright-utils

Общие утилиты для тестирования с использованием Playwright в проектах ImtCore.

## Установка

```bash
npm install @imtcore/playwright-utils --save-dev
```

## Использование

### Базовый пример

```javascript
const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot, login } = require('@imtcore/playwright-utils');

test('Example test', async ({ page }) => {
  await reloadPage(page);
  await login(page, 'username', 'password');
  await checkScreenshot(page, 'test-screenshot.png');
});
```

### Global Setup

Для настройки глобального состояния перед запуском тестов:

```javascript
// global-setup.js
const { createGlobalSetup } = require('@imtcore/playwright-utils');

module.exports = createGlobalSetup({
  baseURL: 'http://localhost:7778',
  username: 'su',
  password: '1',
  viewport: { width: 1400, height: 800 },
  storageStatePath: './storageState.json'
});
```

Затем в `playwright.config.js`:

```javascript
module.exports = defineConfig({
  globalSetup: require.resolve('./global-setup.js'),
  // ... остальные настройки
});
```

## API

### Утилиты для взаимодействия со страницей

#### `clickAt(page, x, y)`
Клик по координатам с ожиданием стабильности страницы.

```javascript
await clickAt(page, 700, 386);
```

#### `clickOnElement(page, path)`
Клик по элементу, найденному по пути objectName.

```javascript
await clickOnElement(page, ["MenuPanel", "UsersButton"]);
```

#### `clickOnPage(page, pageId)`
Клик по странице в меню.

```javascript
await clickOnPage(page, "Users");
```

#### `clickOnCommand(page, commandId)`
Клик по команде.

```javascript
await clickOnCommand(page, "Create");
```

#### `clickOnButton(page, buttonPath)`
Клик по кнопке по пути.

```javascript
await clickOnButton(page, ["Dialog", "OkButton"]);
```

### Работа с формами

#### `fillTextInput(page, text, path)`
Заполнение текстового поля.

```javascript
await fillTextInput(page, "Test User", ["UserDialog", "NameInput"]);
```

#### `selectComboBox(page, selectedText, path)`
Выбор значения из выпадающего списка.

```javascript
await selectComboBox(page, "Administrator", ["UserDialog", "RoleComboBox"]);
```

### Навигация и ожидание

#### `reloadPage(page, url = 'http://localhost:7778')`
Перезагрузка страницы с ожиданием стабильности.

```javascript
await reloadPage(page);
await reloadPage(page, 'http://localhost:8080');
```

#### `waitForPageStability(page, options = {})`
Ожидание стабильности DOM.

```javascript
await waitForPageStability(page, {
  maxTotalTime: 5000,
  domStableTime: 800
});
```

#### `delay(time = 5000)`
Простая задержка.

```javascript
await delay(1000); // Задержка 1 секунда
```

### Скриншоты

#### `checkScreenshot(page, filename, maskParams)`
Создание скриншота с маской (опционально).

```javascript
// Без маски
await checkScreenshot(page, 'page.png');

// С маской по координатам
await checkScreenshot(page, 'page.png', {
  x: 100,
  y: 100,
  width: 200,
  height: 50
});

// С маской по пути элемента
await checkScreenshot(page, 'page.png', {
  path: ["Dialog", "DateField"],
  padding: 10
});
```

### Авторизация

#### `login(page, username, password)`
Авторизация пользователя.

```javascript
await login(page, "su", "1");
```

### Прокрутка

#### `wheelScroll(page, deltaY)`
Прокрутка колесом мыши.

```javascript
await wheelScroll(page, 100); // Прокрутка вниз
await wheelScroll(page, -100); // Прокрутка вверх
```

## Конфигурация Playwright

Пример конфигурации для использования с этими утилитами:

```javascript
// playwright.config.js
const { defineConfig } = require('@playwright/test');

module.exports = defineConfig({
  timeout: 0,
  testDir: './tests',
  globalSetup: require.resolve('./global-setup.js'),
  use: {
    headless: true,
    viewport: { width: 1400, height: 800 },
    screenshot: 'only-on-failure',
    baseURL: 'http://localhost:7778',
  },
  projects: [
    {
      name: 'authorized',
      testMatch: /tests\/authorized\/.*\.test\.js/,
      use: {
        storageState: 'storageState.json',
      },
    },
    {
      name: 'guest',
      testIgnore: /tests\/authorized\/.*\.test\.js/,
    }
  ],
});
```

## Лицензия

UNLICENSED - Только для внутреннего использования ImagingTools
