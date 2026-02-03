# Playwright тестирование - Руководство

## Содержание
- [Введение](#введение)
- [Структура проекта](#структура-проекта)
- [Установка и настройка](#установка-и-настройка)
- [Конфигурация](#конфигурация)
- [Два подхода к созданию тестов](#два-подхода-к-созданию-тестов)
  - [1. Клик по координатам](#1-клик-по-координатам)
  - [2. Использование objectName в QML](#2-использование-objectname-в-qml)
- [Вспомогательные функции](#вспомогательные-функции)
- [Создание новых тестов](#создание-новых-тестов)
- [Примеры тестов](#примеры-тестов)
- [Запуск тестов](#запуск-тестов)
- [Лучшие практики](#лучшие-практики)

## Введение

Этот проект использует [Playwright](https://playwright.dev/) для автоматизированного тестирования пользовательского интерфейса. Playwright - это современный фреймворк для тестирования веб-приложений, который поддерживает множество браузеров и предоставляет мощные инструменты для автоматизации.

## Структура проекта

```
Tests/frontend/
├── tests/
│   ├── utils.js                    # Вспомогательные функции для тестов
│   ├── guest/                      # Тесты для неавторизованных пользователей
│   │   └── login.test.js
│   └── authorized/                 # Тесты для авторизованных пользователей
│       ├── workspace.test.js       # Пример использования objectName подхода
│       ├── users.test.js
│       ├── sensors.test.js
│       ├── orders.test.js
│       └── ...
├── playwright.config.js            # Конфигурация Playwright
├── global-setup.js                 # Глобальная настройка (авторизация)
├── package.json                    # Зависимости проекта
└── start.sh                        # Скрипт для запуска тестов
```

## Установка и настройка

### Требования
- Node.js (версия 14 или выше)
- npm или yarn

### Установка зависимостей

```bash
cd Tests/frontend
npm install
```

Это установит Playwright и все необходимые зависимости.

## Конфигурация

### playwright.config.js

Основные настройки:

```javascript
module.exports = defineConfig({
  timeout: 0,                              // Отключение общего таймаута
  testDir: './tests',                      // Директория с тестами
  globalSetup: require.resolve('./global-setup.js'),
  use: {
    headless: true,                        // Запуск в безголовом режиме
    viewport: { width: 1400, height: 800 }, // Размер окна браузера
    screenshot: 'only-on-failure',          // Скриншоты только при ошибках
    baseURL: 'http://localhost:7778',      // Базовый URL приложения
  },
  projects: [
    {
      name: 'authorized',                   // Тесты для авторизованных пользователей
      testMatch: /tests\/authorized\/.*\.test\.js/,
      use: {
        storageState: 'storageState.json',  // Используем сохраненную сессию
      },
    },
    {
      name: 'guest',                        // Тесты для гостей
      testIgnore: /tests\/authorized\/.*\.test\.js/,
    }
  ],
});
```

### global-setup.js

Выполняет автоматическую авторизацию перед запуском тестов для авторизованных пользователей. Сохраняет состояние сессии в `storageState.json`.

## Два подхода к созданию тестов

В проекте используются два основных подхода для взаимодействия с элементами UI:

### 1. Клик по координатам

Этот подход использует фиксированные координаты на экране для клика по элементам.

**Преимущества:**
- Простой и быстрый для создания
- Не требует изменений в QML коде

**Недостатки:**
- Хрупкий - ломается при изменении макета
- Сложно поддерживать
- Нечитаемый код

**Пример:**

```javascript
const { test } = require('@playwright/test');
const { reloadPage, clickAt, checkScreenshot } = require('../utils');

test('Login test', async ({ page }) => {
  await reloadPage(page);
  
  await clickAt(page, 700, 386); // Клик по полю "Логин"
  await page.keyboard.type('username');
  
  await clickAt(page, 685, 455); // Клик по полю "Пароль"
  await page.keyboard.type('password');
  
  await clickAt(page, 700, 600); // Клик по кнопке "Войти"
  
  await checkScreenshot(page, 'login_success.png');
});
```

**Функция clickAt:**

```javascript
const clickAt = async (page, x, y) => {
  await page.mouse.click(x, y);
  await waitForPageStability(page);
};
```

### 2. Использование objectName в QML

Это **рекомендуемый подход**. Он использует атрибут `objectName` в QML элементах для их идентификации.

**Преимущества:**
- Устойчивый к изменениям макета
- Легко читается и поддерживается
- Явно указывает намерение теста
- Не зависит от координат

**Недостатки:**
- Требует добавления `objectName` в QML коды

**Пример из workspace.test.js:**

```javascript
const { test } = require('@playwright/test');
const { reloadPage, checkScreenshot, clickOnPage, clickOnCommand, 
        selectComboBox, clickOnButton} = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "Workspace"); // Клик по странице Workspace
});

test('User Actions', async ({ page }) => {
  await clickOnCommand(page, "UserActions"); // Клик по вкладке UserActions
  await checkScreenshot(page, '1-workspace-user-actions.png')
});

test('Graphics Time Filter', async ({ page }) => {
  await clickOnButton(page, ["TimeFilterDelegate"]) // Клик по фильтру дат
  await clickOnButton(page, ["TimeFilterParamView", "Month_Current"]) // Клик по "Этот месяц"
  await checkScreenshot(page, '2-workspace-time-filter.png')
});

test('Customer Filter', async ({ page }) => {
  await selectComboBox(page, "QUISS", ["CustomerFilterDelegate"])
  await checkScreenshot(page, '3-workspace-customer-filter.png')
});
```

#### Как это работает

**В QML коде** добавляется атрибут `objectName`:

```qml
Button {
    objectName: "CreateNewButton"  // Уникальное имя для идентификации
    text: "Create New"
    onClicked: // ...
}

ComboBox {
    objectName: "CustomerFilterDelegate"
    // ...
}
```

**Структура путей:**

Путь представляет собой массив objectName от родительского элемента к дочернему:

```javascript
// Простой путь (один элемент)
clickOnButton(page, ["CreateNewButton"])

// Вложенный путь (несколько элементов)
clickOnButton(page, ["SoftwareProductsInfo", "ViewAllButton"])
// Ищет ViewAllButton внутри SoftwareProductsInfo

// Глубоко вложенный путь
clickOnButton(page, ["FilterPanel", "CustomerFilter", "ClearButton"])
```

## Вспомогательные функции

Файл `tests/utils.js` содержит набор вспомогательных функций:

### Навигация и базовые действия

#### `reloadPage(page, url)`
Перезагружает страницу и ждет стабилизации DOM.

```javascript
await reloadPage(page); // Использует baseURL
await reloadPage(page, 'http://localhost:7778/custom'); // Кастомный URL
```

#### `clickAt(page, x, y)`
Кликает по координатам и ждет стабилизации страницы.

```javascript
await clickAt(page, 700, 400);
```

#### `waitForPageStability(page, options)`
Ждет, пока DOM стабилизируется (нет изменений в течение определенного времени).

```javascript
await waitForPageStability(page, {
  maxTotalTime: 5000,     // Максимальное время ожидания
  domStableTime: 800,     // Время стабильности
});
```

### Работа с objectName элементами

#### `clickOnPage(page, pageId)`
Кликает по кнопке страницы в меню навигации.

```javascript
await clickOnPage(page, "Workspace");    // MenuPanel -> WorkspaceButton
await clickOnPage(page, "Administration"); // MenuPanel -> AdministrationButton
```

**Реализация:**
```javascript
const clickOnPage = async (page, pageId) => {
  await clickOnElement(page, ["MenuPanel", pageId + "Button"])
}
```

#### `clickOnCommand(page, commandId)`
Кликает по команде в панели команд.

```javascript
await clickOnCommand(page, "UserActions"); // CommandsView -> UserActionsButton
await clickOnCommand(page, "Edit");        // CommandsView -> EditButton
await clickOnCommand(page, "Save");        // CommandsView -> SaveButton
```

**Реализация:**
```javascript
const clickOnCommand = async (page, commandId) => {
  await clickOnElement(page, ["CommandsView", commandId + "Button"])
}
```

#### `clickOnButton(page, buttonPath)`
Универсальная функция для клика по кнопке через путь objectName.

```javascript
await clickOnButton(page, ["TimeFilterDelegate"]);
await clickOnButton(page, ["SoftwareProductsInfo", "CreateNewButton"]);
await clickOnButton(page, ["FilterPanel", "SearchTextInput", "ClearButton"]);
```

#### `clickOnElement(page, path)`
Базовая функция для клика по элементу через путь objectName. Находит MouseArea внутри элемента и кликает по его центру.

```javascript
await clickOnElement(page, ["MenuPanel", "WorkspaceButton"]);
```

**Как работает:**
1. Создает CSS селектор из пути: `[objectName="MenuPanel"] [objectName="WorkspaceButton"]`
2. Находит MouseArea внутри элемента: `[objectName="MouseArea"][visible]`
3. Вычисляет центр элемента
4. Кликает по координатам центра

### Работа с формами

#### `fillTextInput(page, text, path)`
Заполняет текстовое поле.

```javascript
await fillTextInput(page, "test@mail.ru", ["FilterPanel", "SearchTextInput"]);
await fillTextInput(page, "username", ["LoginForm", "UsernameInput"]);
```

#### `selectComboBox(page, selectedText, path)`
Выбирает элемент из выпадающего списка (ComboBox).

```javascript
await selectComboBox(page, "QUISS", ["CustomerFilterDelegate"]);
await selectComboBox(page, "None", ["FilterPanel", "SensorStatusFilter"]);
```

**Как работает:**
1. Кликает по ComboBox
2. Открывается PopupMenuDialog
3. Кликает по элементу с указанным текстом в диалоге

### Валидация

#### `checkScreenshot(page, filename, maskParams)`
Сравнивает текущее состояние страницы со сохраненным скриншотом.

```javascript
await checkScreenshot(page, 'workspace-start.png');

// С маской (скрывает динамические области)
await checkScreenshot(page, 'with-mask.png', { 
  x: 580, y: 400, width: 100, height: 40 
});

// Маска по пути objectName
await checkScreenshot(page, 'with-element-mask.png', { 
  path: ["ExpirationControl"],
  padding: 10  // Дополнительные отступы вокруг элемента
});
```

**Параметры маски:**
- `x, y, width, height` - координаты и размер маски
- `path` - путь к элементу для маскирования
- `padding` - отступы вокруг элемента

### Дополнительные функции

#### `wheelScroll(page, deltaY)`
Прокручивает страницу колесом мыши.

```javascript
await wheelScroll(page, 500);  // Прокрутка вниз
await wheelScroll(page, -500); // Прокрутка вверх
```

#### `login(page, username, password)`
Выполняет вход в систему (использует координаты).

```javascript
await login(page, "su", "password123");
```

#### `delay(time)`
Задержка выполнения.

```javascript
await delay(1000); // Ждет 1 секунду
await delay();     // Ждет 5 секунд (по умолчанию)
```

## Создание новых тестов

### Шаг 1: Определите тип теста

- **Guest тесты** - в `tests/guest/` для неавторизованных пользователей
- **Authorized тесты** - в `tests/authorized/` для авторизованных пользователей

### Шаг 2: Создайте файл теста

Создайте файл с именем `feature.test.js`:

```javascript
const { test } = require('@playwright/test');
const { reloadPage, checkScreenshot, clickOnPage, clickOnButton } = require('../utils');

test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "YourPage"); // Навигация к вашей странице
});

test('Test description', async ({ page }) => {
  // Ваш тест здесь
});
```

### Шаг 3: Добавьте objectName в QML (рекомендуется)

Для стабильных тестов добавьте `objectName` к элементам в QML:

```qml
Item {
    objectName: "MyContainer"
    
    Button {
        objectName: "SubmitButton"
        text: "Submit"
        onClicked: // ...
    }
    
    ComboBox {
        objectName: "CategorySelector"
        // ...
    }
}
```

### Шаг 4: Напишите тест

**Используя objectName (рекомендуется):**

```javascript
test('Create new item', async ({ page }) => {
  // Клик по кнопке New
  await clickOnButton(page, ["CommandBar", "NewButton"]);
  
  // Заполнить форму
  await fillTextInput(page, "Item Name", ["ItemForm", "NameInput"]);
  await selectComboBox(page, "Category A", ["ItemForm", "CategorySelector"]);
  
  // Сохранить
  await clickOnButton(page, ["ItemForm", "SubmitButton"]);
  
  // Проверить результат
  await checkScreenshot(page, 'item-created.png');
});
```

**Используя координаты (если objectName недоступен):**

```javascript
test('Coordinate-based test', async ({ page }) => {
  await clickAt(page, 100, 200); // Клик по кнопке
  await page.keyboard.type('test text');
  await clickAt(page, 300, 400); // Клик по другому элементу
  await checkScreenshot(page, 'result.png');
});
```

### Шаг 5: Запустите тест

```bash
# Запустить конкретный тест
npx playwright test tests/authorized/yourtest.test.js

# Запустить все тесты
npm test
```

## Примеры тестов

### Пример 1: Простой тест навигации (objectName подход)

```javascript
test('Navigate to settings', async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "Settings");
  await checkScreenshot(page, 'settings-page.png');
});
```

### Пример 2: Тест с фильтрами

```javascript
test('Apply filters', async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "Sensors");
  
  // Текстовый фильтр
  await fillTextInput(page, "test", ["FilterPanel", "SearchTextInput"]);
  
  // Фильтр по дате
  await clickOnButton(page, ["FilterPanel", "CreationDateFilter"]);
  await clickOnButton(page, ["TimeFilterParamView", "Month_Current"]);
  
  // ComboBox фильтр
  await selectComboBox(page, "Active", ["FilterPanel", "StatusFilter"]);
  
  await checkScreenshot(page, 'filtered-sensors.png');
});
```

### Пример 3: Тест с редактированием

```javascript
test('Edit user', async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "Administration");
  await clickOnButton(page, ["AdministrationView", "Users"]);
  
  // Выбрать пользователя (координаты для выбора из таблицы)
  await clickAt(page, 490, 215);
  
  // Открыть редактор
  await clickOnCommand(page, "Edit");
  
  // Изменить email
  await fillTextInput(page, "newemail@example.com", ["UserEditor", "EmailInput"]);
  
  // Сохранить
  await clickOnCommand(page, "Save");
  
  await checkScreenshot(page, 'user-updated.png');
});
```

### Пример 4: Тест с маской (скрытие динамических элементов)

```javascript
test('Test with masked element', async ({ page }) => {
  await reloadPage(page);
  
  // Маска по координатам
  let mask = { x: 1015, y: 200, width: 150, height: 565 };
  await checkScreenshot(page, 'with-coordinate-mask.png', mask);
  
  // Маска по пути objectName
  await checkScreenshot(page, 'with-path-mask.png', {
    path: ["DateTimeDisplay"],
    padding: 5
  });
});
```

## Запуск тестов

### Локальный запуск

```bash
cd Tests/frontend

# Установить зависимости (один раз)
npm install

# Запустить все тесты
npx playwright test

# Запустить тесты для авторизованных пользователей
npx playwright test --project=authorized

# Запустить тесты для гостей
npx playwright test --project=guest

# Запустить конкретный тест
npx playwright test tests/authorized/workspace.test.js

# Запуск в режиме с UI (headed mode)
npx playwright test --headed

# Запуск с отладкой
npx playwright test --debug
```

### Использование start.sh

Скрипт `start.sh` настраивает окружение и запускает тесты:

```bash
./start.sh
```

### Обновление скриншотов

Если UI изменился и вам нужно обновить эталонные скриншоты:

```bash
npx playwright test --update-snapshots
```

## Лучшие практики

### 1. Используйте objectName вместо координат

❌ **Плохо:**
```javascript
await clickAt(page, 700, 386);
await clickAt(page, 685, 455);
```

✅ **Хорошо:**
```javascript
await fillTextInput(page, "username", ["LoginForm", "UsernameInput"]);
await fillTextInput(page, "password", ["LoginForm", "PasswordInput"]);
await clickOnButton(page, ["LoginForm", "SubmitButton"]);
```

### 2. Используйте beforeEach для общей настройки

```javascript
test.beforeEach(async ({ page }) => {
  await reloadPage(page);
  await clickOnPage(page, "MyPage");
});
```

### 3. Добавляйте осмысленные описания тестов

❌ **Плохо:**
```javascript
test('test1', async ({ page }) => { /* ... */ });
```

✅ **Хорошо:**
```javascript
test('Should filter sensors by creation date', async ({ page }) => { /* ... */ });
```

### 4. Используйте маски для динамических элементов

Элементы с меняющимися данными (время, даты, случайные ID) должны быть замаскированы:

```javascript
await checkScreenshot(page, 'page.png', {
  path: ["DateTimeDisplay"],
  padding: 10
});
```

### 5. Группируйте связанные тесты

```javascript
test.describe('User management', () => {
  test.beforeEach(async ({ page }) => {
    await reloadPage(page);
    await clickOnPage(page, "Administration");
    await clickOnButton(page, ["AdministrationView", "Users"]);
  });

  test('Create user', async ({ page }) => { /* ... */ });
  test('Edit user', async ({ page }) => { /* ... */ });
  test('Delete user', async ({ page }) => { /* ... */ });
});
```

### 6. Используйте вспомогательные функции

Для повторяющихся действий создавайте вспомогательные функции в тесте:

```javascript
async function selectFirstRow(page) {
  await clickAt(page, 600, 245);
}

test('Test 1', async ({ page }) => {
  await selectFirstRow(page);
  // ...
});

test('Test 2', async ({ page }) => {
  await selectFirstRow(page);
  // ...
});
```

### 7. Ждите стабилизации страницы

Все функции из `utils.js` уже ждут стабилизации, но если вы используете напрямую Playwright API:

```javascript
await page.click('#button');
await waitForPageStability(page); // Важно!
```

### 8. Именование objectName

При добавлении objectName в QML используйте понятные и уникальные имена:

```qml
// Хорошие примеры
objectName: "CreateNewButton"
objectName: "UserFilterDelegate"
objectName: "SaveCommand"

// Избегайте общих имен
objectName: "Button"     // Слишком общее
objectName: "Item1"      // Непонятное
```

### 9. Структура путей

Используйте логическую иерархию в путях:

```javascript
// Понятная структура
["FilterPanel", "CustomerFilter", "ClearButton"]
["EditorForm", "PersonalInfo", "EmailInput"]

// Плоская структура (используйте уникальные имена)
["ClearCustomerFilterButton"]
```

### 10. Комментарии в тестах

Добавляйте комментарии для координатных кликов и сложной логики:

```javascript
await clickAt(page, 490, 215); // Select first user from table
await clickOnButton(page, ["UserEditor", "RolesTab"]); // Switch to roles tab
```

## Отладка тестов

### Просмотр тестов в браузере

```bash
npx playwright test --headed --project=authorized
```

### Медленное выполнение

```bash
npx playwright test --headed --slow-mo=1000
```

### Пауза и отладка

```javascript
test('Debug test', async ({ page }) => {
  await page.pause(); // Остановка выполнения для отладки
  // ...
});
```

### Просмотр трейсов

```bash
# Запуск с трейсингом
npx playwright test --trace on

# Просмотр трейса
npx playwright show-trace trace.zip
```

## Устранение неполадок

### Тест падает из-за таймаута

Увеличьте время ожидания в `waitForPageStability`:

```javascript
await waitForPageStability(page, {
  maxTotalTime: 10000,
  domStableTime: 1000,
});
```

### Элемент не найден

1. Проверьте, что objectName правильно указан в QML
2. Проверьте путь к элементу
3. Убедитесь, что элемент visible
4. Используйте `page.pause()` для отладки

### Скриншоты не совпадают

1. Проверьте, нет ли динамических элементов (время, даты)
2. Используйте маски для динамических областей
3. Обновите эталонные скриншоты: `npx playwright test --update-snapshots`

## Заключение

Playwright предоставляет мощный и гибкий способ автоматизации тестирования UI. Следуя этому руководству и используя objectName подход, вы сможете создавать надежные и поддерживаемые тесты.

**Ключевые моменты:**
- ✅ Используйте objectName вместо координат
- ✅ Используйте вспомогательные функции из utils.js
- ✅ Добавляйте осмысленные описания к тестам
- ✅ Маскируйте динамические элементы в скриншотах
- ✅ Следуйте существующим паттернам в проекте

Для получения дополнительной информации смотрите:
- [Официальная документация Playwright](https://playwright.dev/)
- Примеры в `tests/authorized/workspace.test.js`
- Исходный код вспомогательных функций в `tests/utils.js`
