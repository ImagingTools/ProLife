# ProLife Release App - GitHub Workflow Guide

## Обзор

ProLife Release App - это GitHub App, интегрированный с GitHub Actions workflows для автоматизации процесса создания релизов ProLife.

## Функциональность

### Автоматическое управление субмодулями
- Выбор версий для каждого субмодуля через параметры workflow
- Автоматический поиск последних версий (опция "latest")
- Обновление субмодулей до указанных версий
- Закрепление версий в коммите

### Автоматизированный процесс релиза
- Обновление версий субмодулей
- Генерация CHANGELOG
- Обновление версии в CMakeLists.txt
- Создание коммита и тега
- Сборка артефактов
- Создание GitHub Release

## Как использовать

### Запуск через GitHub UI

1. Откройте репозиторий ProLife на GitHub
2. Перейдите в **Actions** → **App Dispatch Release**
3. Нажмите **Run workflow**
4. Заполните параметры:

#### Основные параметры
- **Branch to release from** - ветка для релиза (default: `main`)
- **Target release tag** - тег релиза (например, `v2.1.0`)
- **Base tag for changelog** - предыдущий тег (например, `v2.0.0`)
- **Dry run** - тестовый запуск без создания тега/релиза

#### Версии субмодулей
Для каждого субмодуля можно указать:
- Конкретный тег (например, `v2.0.0`)
- Ветку (например, `main`, `develop`)
- `latest` - автоматически выбрать последнюю семантическую версию

**Субмодули:**
- **Acf version**
- **AcfSln version**
- **ImtCore version**
- **Lisa version**
- **Puma version**
- **Agentino version**

5. Нажмите **Run workflow**

### Пример использования

#### Пример 1: Релиз с последними версиями всех субмодулей

```yaml
Branch to release from: main
Target release tag: v2.1.0
Base tag for changelog: v2.0.0
Acf version: latest
AcfSln version: latest
ImtCore version: latest
Lisa version: latest
Puma version: latest
Agentino version: latest
Dry run: true  # Для тестирования
```

#### Пример 2: Релиз с конкретными версиями

```yaml
Branch to release from: main
Target release tag: v2.1.0
Base tag for changelog: v2.0.0
Acf version: v2.3.0
AcfSln version: v1.5.2
ImtCore version: v3.1.0
Lisa version: v1.8.0
Puma version: v2.2.0
Agentino version: v1.4.0
Dry run: false  # Реальный релиз
```

#### Пример 3: Частичное обновление

```yaml
# Обновить только ImtCore и Lisa, остальное оставить как есть
Branch to release from: develop
Target release tag: v2.1.0-rc1
Base tag for changelog: v2.0.0
Acf version: v2.3.0  # Оставить текущую версию
AcfSln version: v1.5.2  # Оставить текущую версию
ImtCore version: latest  # Обновить до последней
Lisa version: latest  # Обновить до последней
Puma version: v2.2.0  # Оставить текущую версию
Agentino version: v1.4.0  # Оставить текущую версию
Dry run: true
```

## Что делает workflow

### App Dispatch Workflow (`app-dispatch.yml`)

1. **Генерирует токен GitHub App**
   - Использует приватный ключ из secrets
   - Создает JWT токен
   - Получает installation access token

2. **Запускает Release workflow**
   - Передает все параметры включая версии субмодулей
   - Использует токен GitHub App для аутентификации

3. **Создает отчет**
   - Показывает все выбранные параметры
   - Ссылку на запущенный workflow

### Release Workflow (`release.yml`)

1. **Checkout кода**
   - Загружает репозиторий с субмодулями
   - Full history для генерации changelog

2. **Обновляет субмодули**
   - Для каждого субмодуля:
     - Если указано `latest` - находит последний semantic version tag
     - Переключает субмодуль на указанную версию
     - Добавляет изменения в staging

3. **Валидирует субмодули**
   - Проверяет что все субмодули на правильных версиях
   - Генерирует отчет о версиях

4. **Генерирует CHANGELOG**
   - Использует conventional-changelog-cli
   - Создает записи на основе коммитов

5. **Обновляет версию**
   - Меняет версию в `Build/CMake/CMakeLists.txt`

6. **Создает коммит**
   - Коммитит все изменения
   - Включает информацию о версиях субмодулей

7. **Создает тег и релиз** (если не dry run)
   - Создает git tag
   - Создает GitHub Release
   - Загружает артефакты

## Настройка GitHub App

### Необходимые secrets

В настройках репозитория должны быть настроены:

1. **APP_ID** - ID вашего GitHub App
2. **APP_INSTALLATION_ID** - Installation ID приложения
3. **APP_PRIVATE_KEY** - Приватный ключ приложения (PEM формат)

### Права доступа GitHub App

Приложение должно иметь права:
- **Contents**: Read & Write (для коммитов и тегов)
- **Workflows**: Read & Write (для запуска workflows)
- **Actions**: Read (для мониторинга)

## Преимущества

### По сравнению с ручным процессом

**Было:**
```bash
# 20+ команд
cd 3rdParty/Acf && git checkout v2.0.0 && cd ../..
cd 3rdParty/AcfSln && git checkout v1.5.0 && cd ../..
# ... повторить для каждого субмодуля
./prepare-release.sh --pin-submodules
# ... генерация changelog
# ... обновление версии
# ... коммит, тег, push
```

**Стало:**
```
1. GitHub Actions → Run workflow
2. Заполнить форму
3. Нажать "Run workflow"
4. Готово!
```

### Ключевые преимущества

✅ **Автоматизация** - весь процесс в одном workflow  
✅ **GUI интерфейс** - форма с параметрами в GitHub UI  
✅ **Версионирование** - контроль версий каждого субмодуля  
✅ **Безопасность** - использует GitHub App с ограниченными правами  
✅ **Аудит** - все действия логируются в GitHub Actions  
✅ **Откат** - легко откатить изменения через Git  
✅ **Dry run** - тестовый запуск перед реальным релизом  

## Мониторинг

### Просмотр прогресса

1. После запуска workflow перейдите в **Actions**
2. Найдите запущенный workflow **Release Preparation**
3. Откройте его чтобы увидеть:
   - Статус каждого шага
   - Логи выполнения
   - Обновления субмодулей
   - Созданные артефакты

### Проверка результата

После завершения workflow:
- **Commits**: Проверьте коммит с обновлениями
- **Tags**: Проверьте созданный тег
- **Releases**: Проверьте созданный GitHub Release
- **Artifacts**: Скачайте артефакты из workflow

## Устранение проблем

### Ошибка: "Failed to generate GitHub App token"

**Причина:** Неправильные secrets  
**Решение:**
1. Проверьте что APP_ID, APP_INSTALLATION_ID и APP_PRIVATE_KEY настроены
2. Убедитесь что приватный ключ в правильном формате (PEM)
3. Проверьте что GitHub App установлен в репозитории

### Ошибка: "Failed to checkout version for submodule"

**Причина:** Указанная версия не существует  
**Решение:**
1. Проверьте что тег существует в субмодуле
2. Используйте `latest` для автоматического выбора
3. Проверьте доступность субмодуля

### Workflow не запускается

**Причина:** Недостаточно прав  
**Решение:**
1. Проверьте права GitHub App
2. Убедитесь что workflows enabled в Settings → Actions

### Dry run успешен, но реальный релиз не создается

**Причина:** Это ожидаемое поведение  
**Решение:**
1. Запустите workflow еще раз
2. Установите **Dry run** в `false`
3. Проверьте что коммиты/теги не создаются повторно

## Лучшие практики

### Перед релизом

1. **Всегда делайте dry run сначала**
   ```
   Dry run: true
   ```

2. **Проверьте результат dry run**
   - Просмотрите логи
   - Проверьте что версии субмодулей правильные
   - Убедитесь что CHANGELOG выглядит корректно

3. **Используйте `latest` осторожно**
   - Для production релизов лучше указывать конкретные версии
   - `latest` хорош для development/testing

### Во время релиза

1. **Мониторьте workflow**
   - Не закрывайте страницу Actions
   - Следите за каждым шагом
   - Проверяйте логи на ошибки

2. **Проверяйте субмодули**
   - Убедитесь что все субмодули обновлены
   - Проверьте отчет SUBMODULE_VERSIONS.md

### После релиза

1. **Проверьте релиз**
   - Скачайте артефакты
   - Проверьте GitHub Release
   - Убедитесь что тег создан

2. **Тестирование**
   - Протестируйте релиз на чистой системе
   - Проверьте все функции

3. **Уведомление**
   - Уведомите команду о новом релизе
   - Обновите документацию если необходимо

## Интеграция с другими системами

### CI/CD Pipeline

Можно запускать workflow из других систем:

```bash
# Используя GitHub CLI
gh workflow run app-dispatch.yml \
  -f release_branch=main \
  -f target_tag=v2.1.0 \
  -f base_tag=v2.0.0 \
  -f dry_run=false
```

### Webhook Integration

Можно настроить webhooks для автоматического запуска при:
- Push тега
- Создании release branch
- Merge в main

## Дополнительная информация

### Связанные файлы

- `.github/workflows/app-dispatch.yml` - Entry point workflow
- `.github/workflows/release.yml` - Main release workflow
- `tools/validate-release.sh` - Validation script
- `prepare-release.sh` - Legacy manual script

### Документация

- [RELEASE_GUIDE_RU.md](RELEASE_GUIDE_RU.md) - Полное руководство по релизам
- [RELEASE_STRATEGY.md](RELEASE_STRATEGY.md) - Стратегия релизов
- [3rdParty/README.md](3rdParty/README.md) - Управление субмодулями

## Поддержка

При возникновении проблем:
1. Проверьте логи workflow
2. Проверьте настройки GitHub App
3. Обратитесь к документации
4. Создайте issue: https://github.com/ImagingTools/ProLife/issues

---

**Версия:** 2.0.0  
**Дата:** 2026-01-21  
**Автор:** ProLife Development Team
