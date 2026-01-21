# Quick Start: ProLife Release App

Быстрое руководство для создания релиза ProLife с помощью нового GUI приложения.

## Шаг 1: Установка

### Windows
```cmd
setup-release-app.bat
```

### Linux/macOS
```bash
chmod +x setup-release-app.sh
./setup-release-app.sh
```

Скрипт проверит и установит необходимые зависимости:
- Python 3.7+
- Git
- CMake
- Tkinter (GUI библиотека)

## Шаг 2: Запуск приложения

### Windows
Двойной клик на `ProLifeReleaseApp.bat` или:
```cmd
ProLifeReleaseApp.bat
```

### Linux/macOS
```bash
./ProLifeReleaseApp.sh
```

## Шаг 3: Использование

### Вкладка 1: "Submodule Versions"
1. Нажмите **"Refresh Versions"** для загрузки списка субмодулей
2. Для каждого субмодуля видны:
   - Текущая версия
   - Список доступных версий (теги)
3. Выберите нужные версии из выпадающих списков
4. Нажмите **"Update All to Latest"** для автовыбора последних версий (опционально)
5. Нажмите **"Apply Selected Versions"** для применения

### Вкладка 2: "Build & Release"
1. Введите версию релиза (например, `1.0.0`)
2. Выберите тип сборки: `Release` или `Debug`
3. Проверьте путь к директории сборки
4. **Один клик для всего**: Нажмите **"🚀 Complete Release (All Steps)"**

Или выполните поэтапно:
- Шаг 1: **"Pin Submodules"** - закрепить версии
- Шаг 2: **"Build Project (CMake)"** - собрать проект
- Шаг 3: **"Build EXE (InnoSetup)"** - создать установщик

### Вкладка 3: "Logs"
Здесь отображаются все операции и их результаты в реальном времени.

## Пример: Создание релиза 2.1.0

```
1. Запустить ProLifeReleaseApp.bat

2. Вкладка "Submodule Versions":
   - Нажать "Update All to Latest"
   - Нажать "Apply Selected Versions"
   - Подождать завершения обновления

3. Вкладка "Build & Release":
   - Ввести версию: 2.1.0
   - Выбрать: Release
   - Нажать "🚀 Complete Release (All Steps)"

4. Вкладка "Logs":
   - Следить за процессом
   - Проверить успешность операций

5. Готово!
   Установочный файл: Impl/ProLifeServer/Install/Output/ProLifeServerInstall.exe
```

## Типичные проблемы

### "Python is not installed"
Установите Python с https://www.python.org/
При установке отметьте "Add Python to PATH"

### "Tkinter is not available"
**Linux:** `sudo apt-get install python3-tk`
**Windows:** Переустановите Python с опцией "tcl/tk and IDLE"

### "InnoSetup compiler not found"
Установите InnoSetup: https://jrsoftware.org/isdl.php
Используется только для создания установщика (шаг 3)

### Сборка не удалась
Проверьте логи во вкладке "Logs"
Убедитесь что CMake установлен и проект собирается вручную

## Дополнительно

### Создать только установщик (без пересборки)
Если проект уже собран:
1. Убедитесь что `build/` содержит скомпилированные файлы
2. Нажмите только **"Build EXE (InnoSetup)"**

### Обновить только один субмодуль
1. Вкладка "Submodule Versions"
2. Выберите версию для нужного субмодуля
3. Нажмите "Apply Selected Versions"
4. Проверьте изменения: `git status`

### Использовать командную строку
Старые скрипты всё ещё работают:
```bash
./prepare-release.sh --status
./tools/validate-release.sh validate
./tools/quick-release.sh 2.1.0
```

## Что дальше?

После создания релиза:
1. Проверьте установщик: `Impl/ProLifeServer/Install/Output/`
2. Протестируйте установку на чистой системе
3. Закоммитьте изменения: `git commit -am "Release v2.1.0"`
4. Создайте тег: `git tag -a v2.1.0 -m "Release 2.1.0"`
5. Отправьте в репозиторий: `git push && git push --tags`

## Полная документация

- [RELEASE_APP_README.md](RELEASE_APP_README.md) - Подробное руководство
- [RELEASE_GUIDE_RU.md](RELEASE_GUIDE_RU.md) - Руководство по релизам
- [README.md](README.md) - Основная документация

## Поддержка

Вопросы и проблемы: https://github.com/ImagingTools/ProLife/issues

---

**Совет:** Сделайте ярлык для `ProLifeReleaseApp.bat` на рабочем столе для быстрого доступа!
