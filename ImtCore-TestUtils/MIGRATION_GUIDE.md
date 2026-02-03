# Руководство по миграции тестовых утилит в ImtCore

Этот документ описывает процесс переноса общих утилит для тестирования из ProLife в ImtCore и последующую миграцию ProLife на использование нового пакета.

## Обзор

Общий код для тестирования (utils.js, global-setup.js) из `Tests/frontend` был реорганизован в переиспользуемый npm модуль, который должен быть размещен в репозитории ImtCore для использования в различных проектах ImagingTools.

## Этап 1: Перенос кода в ImtCore

### 1.1 Создание структуры в ImtCore

В репозитории ImtCore создайте следующую структуру:

```
ImtCore/
└── Tests/
    └── playwright-utils/
        ├── package.json
        ├── index.js
        ├── utils.js
        ├── global-setup.js
        └── README.md
```

### 1.2 Копирование файлов

Скопируйте все файлы из `ProLife/ImtCore-TestUtils/playwright-utils/` в созданную структуру ImtCore.

```bash
# В репозитории ProLife
cd /path/to/ProLife
cp -r ImtCore-TestUtils/playwright-utils/* /path/to/ImtCore/Tests/playwright-utils/
```

### 1.3 Коммит в ImtCore

```bash
cd /path/to/ImtCore
git add Tests/playwright-utils
git commit -m "feat(tests): add common Playwright testing utilities

- Add reusable test utilities for Playwright tests
- Add configurable global setup factory
- Add utilities for page interaction, forms, screenshots
- Add comprehensive API documentation"
git push origin feature/playwright-utils
```

Создайте Pull Request и дождитесь его одобрения и слияния.

## Этап 2: Публикация пакета (опционально)

### Вариант A: Публикация в npm registry (приватный или публичный)

```bash
cd /path/to/ImtCore/Tests/playwright-utils
npm publish
```

### Вариант B: Использование через Git dependency

Если вы не хотите публиковать в npm, можно использовать git dependency:

```json
{
  "devDependencies": {
    "@imtcore/playwright-utils": "git+https://github.com/ImagingTools/ImtCore.git#subfolder:Tests/playwright-utils"
  }
}
```

Или через локальный путь во время разработки:

```json
{
  "devDependencies": {
    "@imtcore/playwright-utils": "file:../../../ImtCore/Tests/playwright-utils"
  }
}
```

## Этап 3: Миграция ProLife

### 3.1 Обновление package.json

Обновите `Tests/frontend/package.json`:

```json
{
  "devDependencies": {
    "@playwright/test": "^1.52.0",
    "@imtcore/playwright-utils": "^1.0.0"
  }
}
```

Затем установите зависимости:

```bash
cd Tests/frontend
npm install
```

### 3.2 Обновление global-setup.js

Файл `Tests/frontend/global-setup.js` уже обновлен для использования нового пакета. После публикации пакета в ImtCore, измените импорт:

```javascript
// Было (временно):
const { createGlobalSetup } = require('../../ImtCore-TestUtils/playwright-utils');

// Станет:
const { createGlobalSetup } = require('@imtcore/playwright-utils');
```

### 3.3 Обновление тестовых файлов

Обновите импорты во всех тестовых файлах:

**До:**
```javascript
const { reloadPage, clickAt, checkScreenshot, login } = require('../utils');
```

**После:**
```javascript
const { reloadPage, clickAt, checkScreenshot, login } = require('@imtcore/playwright-utils');
```

Файлы для обновления:
- `Tests/frontend/tests/guest/login.test.js`
- `Tests/frontend/tests/authorized/sensors.test.js`
- `Tests/frontend/tests/authorized/users.test.js`
- `Tests/frontend/tests/authorized/groups.test.js`
- `Tests/frontend/tests/authorized/workspace.test.js`
- `Tests/frontend/tests/authorized/licenses.test.js`
- `Tests/frontend/tests/authorized/startPage.test.js`
- `Tests/frontend/tests/authorized/customers.test.js`
- `Tests/frontend/tests/authorized/orders.test.js`
- `Tests/frontend/tests/authorized/roles.test.js`

### 3.4 Удаление старого кода

После успешной миграции и тестирования:

```bash
cd /path/to/ProLife
# Удалить старый utils.js
rm Tests/frontend/tests/utils.js

# Удалить временную директорию с примером
rm -rf ImtCore-TestUtils
```

### 3.5 Коммит изменений

```bash
git add .
git commit -m "feat(tests): migrate to @imtcore/playwright-utils

- Update test imports to use @imtcore/playwright-utils package
- Remove local utils.js in favor of shared package from ImtCore
- Update global-setup.js to use createGlobalSetup from package"
```

## Этап 4: Тестирование

### 4.1 Локальное тестирование

Перед удалением старого кода убедитесь, что все тесты работают:

```bash
cd Tests/frontend
npm test
```

### 4.2 Проверка всех тестовых сценариев

Запустите все тестовые файлы и убедитесь, что они работают корректно с новыми импортами.

## Дальнейшее использование

### В других проектах

Теперь любой проект ImagingTools может использовать эти утилиты:

```bash
npm install @imtcore/playwright-utils --save-dev
```

```javascript
const { reloadPage, login, checkScreenshot } = require('@imtcore/playwright-utils');

test('My test', async ({ page }) => {
  await reloadPage(page);
  await login(page, 'user', 'pass');
  await checkScreenshot(page, 'my-page.png');
});
```

### Обновление утилит

Когда нужно обновить утилиты:

1. Внесите изменения в `ImtCore/Tests/playwright-utils`
2. Увеличьте версию в `package.json`
3. Опубликуйте новую версию (если используется npm registry)
4. Обновите версию в проектах:
   ```bash
   npm update @imtcore/playwright-utils
   ```

## Рекомендации

1. **Версионирование**: Используйте семантическое версионирование для пакета
2. **Документация**: Поддерживайте актуальную документацию в README.md
3. **Обратная совместимость**: При изменении API старайтесь сохранять обратную совместимость
4. **Тестирование**: Добавьте тесты для самих утилит в ImtCore
5. **Changelog**: Ведите CHANGELOG.md для отслеживания изменений

## Контакты

При возникновении вопросов или проблем обращайтесь к команде разработки ImagingTools.
