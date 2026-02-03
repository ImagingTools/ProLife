# Перенос тестовых утилит в ImtCore - Сводка

## Что было сделано

Общие утилиты для тестирования с Playwright были извлечены из `Tests/frontend` в структурированный npm модуль, готовый к переносу в репозиторий ImtCore.

## Созданные файлы

### Основной пакет утилит
- **`ImtCore-TestUtils/playwright-utils/`** - Директория с переиспользуемым пакетом
  - `package.json` - Конфигурация npm пакета (@imtcore/playwright-utils v1.0.0)
  - `index.js` - Главная точка входа модуля
  - `utils.js` - Все утилиты для тестирования (извлечено из Tests/frontend/tests/utils.js)
  - `global-setup.js` - Фабричная функция createGlobalSetup для настройки тестов
  - `README.md` - Полная документация на русском языке
  - `README_EN.md` - Краткая документация на английском языке

### Документация
- **`ImtCore-TestUtils/README.md`** - Общая информация о структуре и назначении
- **`ImtCore-TestUtils/MIGRATION_GUIDE.md`** - Подробное руководство по миграции

### Обновленные файлы ProLife
- **`Tests/frontend/global-setup.js`** - Обновлен для использования нового пакета
- **`Tests/frontend/tests/utils.js`** - Добавлено предупреждение о deprecation

## Структура пакета @imtcore/playwright-utils

```
playwright-utils/
├── package.json          # npm package configuration
├── index.js             # Main entry point
├── utils.js             # All testing utilities
├── global-setup.js      # Factory for global setup
├── README.md            # Full documentation (Russian)
└── README_EN.md         # Brief documentation (English)
```

## Экспортируемые функции

### Взаимодействие со страницей
- `clickAt(page, x, y)` - Клик по координатам
- `clickOnElement(page, path)` - Клик по элементу через objectName
- `clickOnPage(page, pageId)` - Клик по странице меню
- `clickOnCommand(page, commandId)` - Клик по команде
- `clickOnButton(page, buttonPath)` - Клик по кнопке

### Работа с формами
- `fillTextInput(page, text, path)` - Заполнение текстового поля
- `selectComboBox(page, selectedText, path)` - Выбор из выпадающего списка

### Навигация
- `reloadPage(page, url)` - Перезагрузка страницы
- `waitForPageStability(page, options)` - Ожидание стабильности DOM
- `delay(time)` - Задержка

### Скриншоты
- `checkScreenshot(page, filename, maskParams)` - Создание скриншота с маской

### Авторизация
- `login(page, username, password)` - Авторизация пользователя

### Прокрутка
- `wheelScroll(page, deltaY)` - Прокрутка колесом мыши

### Global Setup
- `createGlobalSetup(options)` - Фабрика для создания функции глобальной настройки

## Следующие шаги

### 1. Перенос в ImtCore (вручную)
```bash
# В репозитории ImtCore создайте структуру:
mkdir -p Tests/playwright-utils
# Скопируйте все файлы из ImtCore-TestUtils/playwright-utils/
cp -r ProLife/ImtCore-TestUtils/playwright-utils/* ImtCore/Tests/playwright-utils/
```

### 2. Публикация пакета (в ImtCore)
После переноса в ImtCore, опубликуйте пакет одним из способов:

**Вариант A: Локальная разработка**
```json
{
  "devDependencies": {
    "@imtcore/playwright-utils": "file:../../../ImtCore/Tests/playwright-utils"
  }
}
```

**Вариант B: Git dependency**
```json
{
  "devDependencies": {
    "@imtcore/playwright-utils": "git+https://github.com/ImagingTools/ImtCore.git#Tests/playwright-utils"
  }
}
```

**Вариант C: npm registry** (приватный)
```bash
cd ImtCore/Tests/playwright-utils
npm publish
```

### 3. Обновление ProLife
После публикации пакета в ImtCore:

1. Обновите `Tests/frontend/package.json`:
```json
{
  "devDependencies": {
    "@playwright/test": "^1.52.0",
    "@imtcore/playwright-utils": "^1.0.0"
  }
}
```

2. Измените импорт в `Tests/frontend/global-setup.js`:
```javascript
// Было:
const { createGlobalSetup } = require('../../ImtCore-TestUtils/playwright-utils');

// Станет:
const { createGlobalSetup } = require('@imtcore/playwright-utils');
```

3. Обновите импорты во всех тестовых файлах:
```javascript
// Было:
const { reloadPage, clickAt } = require('../utils');

// Станет:
const { reloadPage, clickAt } = require('@imtcore/playwright-utils');
```

4. Удалите старые файлы:
```bash
rm Tests/frontend/tests/utils.js
rm -rf ImtCore-TestUtils
```

### 4. Тестирование
```bash
cd Tests/frontend
npm install
npx playwright test
```

## Преимущества новой структуры

1. **Переиспользование**: Утилиты доступны для всех проектов ImagingTools
2. **Версионирование**: Четкое управление версиями через npm
3. **Документация**: Подробная документация API
4. **Конфигурируемость**: Гибкая настройка через createGlobalSetup
5. **Поддержка**: Централизованное обслуживание и обновления

## Изменения в ProLife

### Минимальные изменения
- `Tests/frontend/global-setup.js` - использует новую структуру
- `Tests/frontend/tests/utils.js` - добавлено предупреждение о deprecation

### После полной миграции
- Все тестовые файлы будут использовать `@imtcore/playwright-utils`
- `Tests/frontend/tests/utils.js` будет удален
- `ImtCore-TestUtils/` директория будет удалена

## Обратная совместимость

На данный момент:
- ✅ Все существующие тесты продолжают работать
- ✅ Старый `utils.js` остается доступным
- ✅ `global-setup.js` работает с новой структурой через относительный путь
- ⏳ После переноса в ImtCore потребуется обновление импортов

## Документация

Полная документация доступна в следующих файлах:
- `ImtCore-TestUtils/playwright-utils/README.md` - API и примеры использования
- `ImtCore-TestUtils/MIGRATION_GUIDE.md` - Пошаговое руководство по миграции
- `ImtCore-TestUtils/README.md` - Обзор структуры

## Контакты

При возникновении вопросов обращайтесь к команде разработки ImagingTools.
