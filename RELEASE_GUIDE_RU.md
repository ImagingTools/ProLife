# Руководство по подготовке релиза ProLife

Это руководство описывает процесс подготовки и создания релиза для проекта ProLife с максимальной автоматизацией.

## Обзор

ProLife использует автоматизированные скрипты и GitHub Actions для упрощения процесса релиза. Процесс включает:

- Управление версиями субмодулей
- Автоматическую генерацию changelog
- Обновление версий в файлах проекта
- Создание релизов в GitHub
- Валидацию перед релизом

## Быстрый старт

### Проверка текущего состояния

```bash
# Проверить статус субмодулей
./prepare-release.sh --status

# Полная валидация
./tools/validate-release.sh validate
```

## Подготовка к релизу

### Шаг 1: Инициализация субмодулей (если необходимо)

Если субмодули не инициализированы:

```bash
./prepare-release.sh --init
```

### Шаг 2: Обновление субмодулей до нужных версий

#### Автоматическое обновление (Рекомендуется)

Автоматически обновить все субмодули до последних тегированных версий:

```bash
./tools/validate-release.sh auto-update
```

Этот скрипт:
- Найдет последние семантические версии (v1.2.3) для каждого субмодуля
- Обновит субмодули до этих версий
- Подготовит изменения для коммита

#### Ручное обновление

Для обновления конкретного субмодуля:

```bash
# Посмотреть доступные теги
./prepare-release.sh --list-tags 3rdParty/Acf

# Обновить до конкретной версии
./prepare-release.sh --update-submodule 3rdParty/Acf v2.0.0
./prepare-release.sh --update-submodule 3rdParty/ImtCore v1.5.0
```

#### Интерактивное обновление (Linux/macOS)

```bash
./prepare-release.sh --interactive
```

Скрипт предложит выбрать версию для каждого субмодуля.

### Шаг 3: Тестирование интеграции

После обновления субмодулей **обязательно** протестируйте проект:

```bash
# Сборка проекта
mkdir -p build && cd build
cmake ../Build/CMake
cmake --build . --config Release

# Запуск тестов
ctest --config Release

cd ..
```

### Шаг 4: Валидация перед релизом

Запустите полную валидацию:

```bash
./tools/validate-release.sh validate v2.1.0
```

Скрипт проверит:
- ✅ Все субмодули инициализированы
- ✅ Нет незакоммиченных изменений
- ✅ Субмодули на правильных версиях
- ✅ CHANGELOG.md обновлен
- ✅ Версия в CMakeLists.txt соответствует целевой

### Шаг 5: Создание ветки релиза

```bash
./prepare-release.sh --create-release 2.1.0
```

Это создаст ветку `release/v2.1.0` и переключится на неё.

### Шаг 6: Обновление CHANGELOG.md

Вручную отредактируйте `CHANGELOG.md`, добавив описание изменений:

```markdown
## [2.1.0] - 2026-01-20

### Добавлено
- Новая функция X
- Поддержка формата Y

### Изменено
- Улучшена производительность Z

### Исправлено
- Исправлена ошибка A
- Исправлена утечка памяти B

### Зависимости
- Обновлен Acf до v2.0.0
- Обновлен ImtCore до v1.5.0
```

### Шаг 7: Генерация отчета о версиях субмодулей

```bash
./tools/validate-release.sh report RELEASE_SUBMODULES.md
```

Это создаст файл с информацией о всех версиях субмодулей для документирования релиза.

### Шаг 8: Закрепление версий субмодулей

```bash
./prepare-release.sh --pin-submodules
```

Это зафиксирует текущие коммиты всех субмодулей.

### Шаг 9: Финальная валидация

```bash
./tools/validate-release.sh validate v2.1.0
```

Убедитесь, что все проверки пройдены.

### Шаг 10: Коммит изменений

```bash
git status  # Проверить изменения
git add .
git commit -m "chore: prepare release v2.1.0

- Updated submodules to stable versions
- Updated CHANGELOG.md
- Pinned all dependencies
"
```

### Шаг 11: Отправка ветки релиза

```bash
git push -u origin release/v2.1.0
```

## Создание релиза через GitHub Actions

### Вариант 1: Автоматический релиз (рекомендуется)

1. Перейдите в GitHub Actions вашего репозитория
2. Выберите workflow "Release Preparation"
3. Нажмите "Run workflow"
4. Заполните параметры:
   - **Branch to release from**: `main` или `release/v2.1.0`
   - **Target release tag**: `v2.1.0`
   - **Base tag for changelog**: `v2.0.0` (предыдущий релиз)
   - **Dry run**: `true` для тестового запуска, `false` для реального релиза

5. Workflow автоматически:
   - Сгенерирует changelog
   - Обновит версию в CMakeLists.txt
   - Создаст коммит
   - Создаст тег (если не dry run)
   - Соберет артефакты
   - Создаст GitHub Release (если не dry run)

### Вариант 2: Использование GitHub App Token

Если у вас настроен GitHub App:

1. Перейдите в GitHub Actions
2. Выберите workflow "App Dispatch Release"
3. Запустите с теми же параметрами

Это использует GitHub App token для дополнительных прав доступа.

### Вариант 3: Локальный релиз

Если вы хотите создать релиз вручную:

```bash
# Убедитесь что на ветке main
git checkout main
git pull

# Слияние ветки релиза
git merge --no-ff release/v2.1.0

# Создание тега
git tag -a v2.1.0 -m "Release version 2.1.0"

# Отправка в репозиторий
git push origin main --tags

# Слияние обратно в develop (если есть)
git checkout develop
git merge --no-ff release/v2.1.0
git push origin develop
```

## Автоматизированные инструменты

### validate-release.sh

Основной инструмент валидации и автоматизации:

```bash
# Полная валидация
./tools/validate-release.sh validate v2.1.0

# Автообновление субмодулей
./tools/validate-release.sh auto-update

# Закрепление с валидацией
./tools/validate-release.sh pin

# Генерация отчета
./tools/validate-release.sh report

# Показать итоги
./tools/validate-release.sh summary v2.1.0
```

### prepare-release.sh

Инструмент управления субмодулями:

```bash
# Статус
./prepare-release.sh --status

# Инициализация
./prepare-release.sh --init

# Обновление всех
./prepare-release.sh --update-all

# Обновление конкретного
./prepare-release.sh --update-submodule 3rdParty/Acf v2.0.0

# Список тегов
./prepare-release.sh --list-tags 3rdParty/Acf

# Интерактивный режим
./prepare-release.sh --interactive

# Закрепление
./prepare-release.sh --pin-submodules

# Создание ветки
./prepare-release.sh --create-release 2.1.0

# Отчет
./prepare-release.sh --report
```

## Полный автоматизированный workflow

Для максимально быстрой подготовки релиза:

```bash
#!/bin/bash
# Скрипт быстрой подготовки релиза

VERSION="2.1.0"

echo "🚀 Начинаем подготовку релиза v$VERSION"

# 1. Проверка состояния
echo "1️⃣ Проверка состояния..."
./tools/validate-release.sh validate

# 2. Создание ветки релиза
echo "2️⃣ Создание ветки релиза..."
./prepare-release.sh --create-release $VERSION

# 3. Автообновление субмодулей
echo "3️⃣ Обновление субмодулей..."
./tools/validate-release.sh auto-update

# 4. Генерация отчета о версиях
echo "4️⃣ Генерация отчета..."
./tools/validate-release.sh report RELEASE_SUBMODULES_v$VERSION.md

# 5. Закрепление субмодулей
echo "5️⃣ Закрепление субмодулей..."
./prepare-release.sh --pin-submodules

# 6. Валидация
echo "6️⃣ Финальная валидация..."
./tools/validate-release.sh validate v$VERSION

echo ""
echo "✅ Автоматическая подготовка завершена!"
echo ""
echo "Следующие шаги вручную:"
echo "  1. Обновите CHANGELOG.md"
echo "  2. Обновите версию в Build/CMake/CMakeLists.txt"
echo "  3. Запустите сборку и тесты"
echo "  4. Закоммитьте изменения: git commit -m 'chore: prepare release v$VERSION'"
echo "  5. Отправьте: git push -u origin release/v$VERSION"
echo "  6. Запустите GitHub Actions workflow для создания релиза"
```

Сохраните этот скрипт как `quick-release.sh` и используйте:

```bash
chmod +x quick-release.sh
./quick-release.sh
```

## Управление версиями субмодулей

### Стратегия версионирования

Для стабильных релизов **всегда используйте тегированные версии** субмодулей:

✅ **Правильно:**
- `v2.0.0` - семантическая версия
- `v1.5.3` - патч релиз

❌ **Избегайте для продакшн релизов:**
- `main` - последний коммит (нестабильно)
- `abc123def` - конкретный коммит (сложно отследить)

### Когда обновлять субмодули

1. **Патч релизы субмодулей** - обновляйте немедленно (исправления багов)
2. **Минорные релизы** - обновляйте перед каждым релизом (новые функции)
3. **Мажорные релизы** - планируйте отдельно (breaking changes)

### Проверка совместимости

После обновления субмодулей:

```bash
# Полная пересборка
rm -rf build
mkdir build && cd build
cmake ../Build/CMake
cmake --build . --config Release

# Запуск всех тестов
ctest --config Release --verbose

# Интеграционные тесты
# ... запустите ваши интеграционные тесты ...
```

## Типичные сценарии

### Hotfix релиз

Для критических исправлений в production:

```bash
# Создаём ветку от main
git checkout main
git checkout -b hotfix/v2.0.1

# Исправляем проблему
# ... делаем изменения ...

# Коммитим
git commit -am "fix: critical bug in X"

# Обновляем версию
sed -i 's/project(ProLifeAll VERSION [0-9.]*)/project(ProLifeAll VERSION 2.0.1)/' Build/CMake/CMakeLists.txt

# Валидация
./tools/validate-release.sh validate v2.0.1

# Мержим в main
git checkout main
git merge --no-ff hotfix/v2.0.1
git tag -a v2.0.1 -m "Hotfix release 2.0.1"
git push origin main --tags

# Мержим в develop
git checkout develop
git merge --no-ff hotfix/v2.0.1
git push origin develop

# Удаляем ветку hotfix
git branch -d hotfix/v2.0.1
```

### Откат субмодуля

Если новая версия субмодуля вызывает проблемы:

```bash
# Посмотреть доступные версии
./prepare-release.sh --list-tags 3rdParty/Acf

# Откатиться на предыдущую версию
./prepare-release.sh --update-submodule 3rdParty/Acf v1.9.0

# Тестирование
# ... проверить работоспособность ...

# Коммит
git add 3rdParty/Acf
git commit -m "chore(deps): rollback Acf to v1.9.0 due to regression"
```

### Обновление одного субмодуля

Для обновления только одной зависимости:

```bash
# Проверить доступные версии
./prepare-release.sh --list-tags 3rdParty/ImtCore

# Обновить
./prepare-release.sh --update-submodule 3rdParty/ImtCore v1.6.0

# Тестирование
mkdir -p build && cd build
cmake ../Build/CMake
cmake --build . --config Release
ctest --config Release
cd ..

# Коммит
git add 3rdParty/ImtCore
git commit -m "chore(deps): update ImtCore to v1.6.0

- Includes performance improvements
- Fixes memory leak
"
```

## Troubleshooting

### Ошибка: субмодуль не инициализирован

```bash
./prepare-release.sh --init
```

### Ошибка: конфликт при обновлении субмодуля

```bash
cd 3rdParty/Acf
git fetch --all
git reset --hard origin/main
cd ../..
./prepare-release.sh --pin-submodules
```

### Ошибка: detached HEAD в субмодуле

```bash
cd 3rdParty/Acf
git checkout main  # или нужную ветку
cd ../..
git add 3rdParty/Acf
```

### Ошибка валидации: незакоммиченные изменения

```bash
# Посмотреть изменения
git status

# Закоммитить или отменить
git add .
git commit -m "chore: preparation changes"
# или
git checkout .
```

### Проблемы с правами доступа к GitHub Actions

Если workflow не может создать релиз:

1. Проверьте права токена в Settings → Actions → General
2. Убедитесь, что выбрано "Read and write permissions"
3. Для приватных репозиториев может потребоваться GitHub App

## Интеграция с CI/CD

### Автоматическая проверка субмодулей в CI

Добавьте в ваш CI/CD pipeline:

```yaml
# .github/workflows/ci.yml
- name: Validate submodules
  run: |
    ./tools/validate-release.sh validate
    
- name: Check submodule versions
  run: |
    ./prepare-release.sh --status
```

### Автоматическое создание релиза при push тега

```yaml
# .github/workflows/auto-release.yml
name: Auto Release

on:
  push:
    tags:
      - 'v*'

jobs:
  release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive
          
      - name: Validate release
        run: ./tools/validate-release.sh validate ${GITHUB_REF#refs/tags/}
        
      - name: Build
        run: |
          mkdir build && cd build
          cmake ../Build/CMake
          cmake --build . --config Release
          
      - name: Create Release
        uses: softprops/action-gh-release@v1
        with:
          files: build/artifacts/*
```

## Best Practices

### Версионирование

1. **Следуйте семантическому версионированию**: MAJOR.MINOR.PATCH
2. **Используйте префикс 'v'**: v2.1.0
3. **Документируйте breaking changes** в CHANGELOG.md

### Субмодули

1. **Всегда используйте теги** для production релизов
2. **Тестируйте после обновления** каждого субмодуля
3. **Документируйте версии** в CHANGELOG.md
4. **Создавайте отчеты** о версиях для каждого релиза

### Тестирование

1. **Полная пересборка** после обновления субмодулей
2. **Запуск всех тестов** перед релизом
3. **Интеграционное тестирование** с новыми версиями
4. **Тестирование на целевых платформах**

### Коммуникация

1. **Предупреждайте команду** о планируемых релизах
2. **Документируйте изменения** в CHANGELOG.md
3. **Создавайте release notes** в GitHub
4. **Обновляйте документацию** пользователя

## Дополнительные ресурсы

- [RELEASE_STRATEGY.md](RELEASE_STRATEGY.md) - Полная стратегия релизов
- [RELEASE_GUIDE.md](RELEASE_GUIDE.md) - Руководство на английском
- [BUILDING.md](BUILDING.md) - Инструкции по сборке
- [Semantic Versioning](https://semver.org/lang/ru/) - Спецификация версионирования
- [Keep a Changelog](https://keepachangelog.com/ru/) - Формат changelog

## Контакты

При возникновении проблем:

1. Проверьте эту документацию
2. Изучите вывод ошибок
3. Запустите валидацию: `./tools/validate-release.sh validate`
4. Проверьте статус: `./prepare-release.sh --status`
5. Обратитесь к команде разработки

---

**Последнее обновление:** 2026-01-20  
**Поддерживается:** Командой разработки ProLife
