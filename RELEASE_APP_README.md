# ProLife Release App

Графическое приложение для управления релизами ProLife. Позволяет:
- Выбирать версии субмодулей через удобный GUI
- Автоматически обновлять субмодули
- Собирать проект
- Создавать установочный EXE файл через InnoSetup

## Требования

### Обязательные
- Python 3.7 или выше
- Git
- CMake (для сборки проекта)

### Для создания установщика (опционально)
- InnoSetup 6 (для Windows): https://jrsoftware.org/isdl.php

## Установка

### Windows

1. Убедитесь, что Python установлен:
```cmd
python --version
```

2. Запустите приложение:
```cmd
ProLifeReleaseApp.bat
```

Или двойным кликом на файл `ProLifeReleaseApp.bat`

### Linux / macOS

1. Сделайте скрипт исполняемым:
```bash
chmod +x ProLifeReleaseApp.sh
```

2. Запустите приложение:
```bash
./ProLifeReleaseApp.sh
```

## Использование

### Вкладка "Submodule Versions"

![Submodule Versions Tab](docs/images/submodules-tab.png)

Здесь вы можете:
1. **Просмотреть** все субмодули из папки `3rdParty/`
2. **Увидеть** текущую версию каждого субмодуля
3. **Выбрать** желаемую версию из выпадающего списка
4. **Обновить** все субмодули до последних тегированных версий

Кнопки:
- **Refresh Versions** - обновить список версий
- **Update All to Latest** - выбрать последние версии для всех субмодулей
- **Apply Selected Versions** - применить выбранные версии

### Вкладка "Build & Release"

![Build & Release Tab](docs/images/build-tab.png)

Процесс создания релиза:

#### 1. Укажите версию релиза
Введите номер версии в формате `1.0.0` (семантическое версионирование)

#### 2. Настройте конфигурацию сборки
- **Build Type**: Release или Debug
- **Build Dir**: Директория для сборки (по умолчанию `build/`)

#### 3. Выполните шаги создания релиза

##### Вариант A: Пошаговый процесс
1. **Pin Submodules** - закрепить субмодули на текущих версиях
2. **Build Project (CMake)** - собрать проект с помощью CMake
3. **Build EXE (InnoSetup)** - создать установочный файл

##### Вариант B: Полный автоматический процесс
Нажмите **"🚀 Complete Release (All Steps)"** для автоматического выполнения всех шагов

### Вкладка "Logs"

![Logs Tab](docs/images/logs-tab.png)

Отображает:
- Все операции приложения
- Вывод команд Git, CMake, InnoSetup
- Ошибки и предупреждения

Кнопки:
- **Clear Logs** - очистить логи
- **Save Logs** - сохранить логи в файл

## Рабочий процесс (Workflow)

### Создание нового релиза

```
1. Открыть приложение
   ↓
2. Вкладка "Submodule Versions"
   - Нажать "Update All to Latest"
   - Или выбрать версии вручную
   - Нажать "Apply Selected Versions"
   ↓
3. Вкладка "Build & Release"
   - Ввести версию релиза (например, 2.1.0)
   - Выбрать Build Type: Release
   - Нажать "🚀 Complete Release"
   ↓
4. Вкладка "Logs"
   - Следить за процессом сборки
   - Проверить на наличие ошибок
   ↓
5. Готово!
   - Установочный файл создан в Impl/ProLifeServer/Install/Output/
```

### Обновление одного субмодуля

```
1. Вкладка "Submodule Versions"
   ↓
2. Найти нужный субмодуль
   ↓
3. Выбрать версию из списка
   ↓
4. Нажать "Apply Selected Versions"
   ↓
5. Проверить изменения через Git
```

## Интеграция с существующими скриптами

ProLife Release App использует существующие скрипты:
- `prepare-release.sh` - для управления субмодулями
- InnoSetup скрипт из `Impl/ProLifeServer/Install/ProLifeServer.iss`

Все операции логируются и можно выполнять те же действия вручную через командную строку.

## Структура проекта

```
ProLife/
├── ProLifeReleaseApp.py        # Главное приложение (Python/Tkinter)
├── ProLifeReleaseApp.bat       # Лаунчер для Windows
├── ProLifeReleaseApp.sh        # Лаунчер для Linux/macOS
├── 3rdParty/                   # Субмодули
│   ├── Acf/
│   ├── AcfSln/
│   ├── ImtCore/
│   ├── Lisa/
│   ├── Puma/
│   └── Agentino/
├── Impl/ProLifeServer/Install/
│   └── ProLifeServer.iss       # InnoSetup скрипт
└── prepare-release.sh          # Скрипт управления релизами
```

## Переменные окружения

### PROLIFE_BUILD_DIR
Используется InnoSetup для определения директории со скомпилированными файлами.
Приложение автоматически устанавливает эту переменную.

## Решение проблем

### Python не найден
**Проблема:** `Python is not installed or not in PATH`

**Решение:**
1. Установите Python с https://www.python.org/
2. При установке отметьте "Add Python to PATH"
3. Или добавьте Python в PATH вручную

### InnoSetup не найден
**Проблема:** `InnoSetup compiler (ISCC.exe) not found`

**Решение:**
1. Установите InnoSetup с https://jrsoftware.org/isdl.php
2. Убедитесь, что он установлен в стандартную директорию
3. Или добавьте путь к ISCC.exe в PATH

### Ошибка при обновлении субмодуля
**Проблема:** `ERROR updating <submodule>`

**Решение:**
1. Проверьте подключение к интернету
2. Убедитесь, что у вас есть доступ к репозиториям
3. Проверьте SSH ключи для GitHub (если используете SSH)
4. Попробуйте выполнить команду вручную:
   ```bash
   cd 3rdParty/<submodule>
   git fetch --all --tags
   git checkout <version>
   ```

### Сборка не удалась
**Проблема:** Ошибки при сборке проекта

**Решение:**
1. Проверьте логи во вкладке "Logs"
2. Убедитесь, что CMake установлен
3. Проверьте зависимости проекта
4. Попробуйте собрать вручную:
   ```bash
   mkdir build && cd build
   cmake ../Build/CMake
   cmake --build . --config Release
   ```

## Преимущества по сравнению со старыми скриптами

### Было (tools/)
- Множество bash скриптов
- Необходимо знать команды
- Ручное выполнение каждого шага
- Сложно для новых пользователей
- Нет визуальной обратной связи

### Стало (ProLife Release App)
- ✅ Единое графическое приложение
- ✅ Интуитивный интерфейс
- ✅ Автоматизация всего процесса
- ✅ Визуальная обратная связь
- ✅ Логирование всех операций
- ✅ Выбор версий из списка (не нужно помнить теги)
- ✅ Один клик для полного релиза
- ✅ Работает на Windows, Linux, macOS

## Дополнительные возможности

### Создание executable файла (опционально)

Можно создать standalone исполняемый файл с помощью PyInstaller:

```bash
pip install pyinstaller
pyinstaller --onefile --windowed --name ProLifeReleaseApp ProLifeReleaseApp.py
```

Результат будет в папке `dist/`

## Поддержка

При возникновении проблем:
1. Проверьте логи во вкладке "Logs"
2. Попробуйте выполнить операцию вручную
3. Проверьте доступность всех инструментов (Python, Git, CMake, InnoSetup)
4. Обратитесь к документации: `RELEASE_GUIDE_RU.md`

## Лицензия

Это приложение является частью проекта ProLife.
© ImagingTools GmbH

---

**Версия:** 1.0.0  
**Дата:** 2026-01-21  
**Автор:** ProLife Development Team
