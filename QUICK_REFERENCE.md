# ProLife Release Quick Reference

Быстрая справка по инструментам подготовки релиза / Quick reference for release tools

## Команды быстрого старта / Quick Start Commands

### Полная автоматизация / Full Automation
```bash
./tools/quick-release.sh 2.1.0
```

### Пошаговая подготовка / Step-by-Step

```bash
# 1. Валидация / Validation
./tools/validate-release.sh validate

# 2. Создание ветки релиза / Create release branch
git checkout -b release/v2.1.0

# 3. Обновление версии в файлах / Update version in files
# Edit Build/CMake/CMakeLists.txt and CHANGELOG.md

# 4. Генерация отчета / Generate report
./tools/validate-release.sh report

# 5. Финальная валидация / Final validation
./tools/validate-release.sh validate v2.1.0
```

## Инструменты / Tools

| Инструмент / Tool | Назначение / Purpose | Команда / Command |
|-------------------|----------------------|-------------------|
| **quick-release.sh** | Автоматическая подготовка релиза | `./tools/quick-release.sh 2.1.0` |
| **validate-release.sh** | Валидация и автоматизация | `./tools/validate-release.sh validate` |

## Основные операции / Common Operations

### Проверка состояния / Check Status
```bash
./tools/validate-release.sh validate
git status
```

### Обновление зависимостей / Update Dependencies

Dependencies must be updated separately in their own repositories:
```bash
cd /path/to/ImtCore
git pull
# Build and test

cd /path/to/Puma
git pull
# Build and test

# ... repeat for other dependencies
```

### Генерация отчета / Generate Report
```bash
./tools/validate-release.sh report VERSIONS.md
```

## Типичные сценарии / Common Scenarios

### Новый релиз / New Release
```bash
./tools/quick-release.sh 2.1.0
vim CHANGELOG.md
git add . && git commit -m "chore: prepare release v2.1.0"
git push -u origin release/v2.1.0
```

### Hotfix
```bash
git checkout main
git checkout -b hotfix/v2.0.1
# ... fix the issue ...
./tools/validate-release.sh validate v2.0.1
git commit -am "fix: critical issue"
git checkout main && git merge --no-ff hotfix/v2.0.1
git tag -a v2.0.1 -m "Hotfix 2.0.1"
git push origin main --tags
```

## GitHub Actions

### Запуск workflow / Run Workflow
1. Go to: **Actions → Release Preparation**
2. Click: **Run workflow**
3. Fill in:
   - **Branch**: `release/v2.1.0` or `main`
   - **Target tag**: `v2.1.0`
   - **Base tag**: `v2.0.0`
   - **Dry run**: `true` (для теста) / `false` (для релиза)

## Решение проблем / Troubleshooting

### Права доступа / Permission Denied
```bash
chmod +x tools/*.sh
```

### Незакоммиченные изменения / Uncommitted Changes
```bash
git status
git add . && git commit -m "..."
# или / or
git stash
```

### Зависимости не настроены / Dependencies Not Configured
```bash
# Ensure environment variables are set
export IMTCOREDIR=/path/to/ImtCore
export PUMADIR=/path/to/Puma
export LISADIR=/path/to/Lisa
export ACFDIR=/path/to/Acf
export ACFSLNDIR=/path/to/AcfSln
export AGENTINODIR=/path/to/Agentino
```

## Документация / Documentation

| Документ / Document | Описание / Description |
|---------------------|------------------------|
| [RELEASE_GUIDE_RU.md](RELEASE_GUIDE_RU.md) | Подробное руководство на русском |
| [RELEASE_GUIDE.md](RELEASE_GUIDE.md) | Complete guide in English |
| [tools/README.md](tools/README.md) | Tool documentation |
| [AUTOMATION_SUMMARY.md](AUTOMATION_SUMMARY.md) | Обзор автоматизации |
| [RELEASE_STRATEGY.md](RELEASE_STRATEGY.md) | Release strategy |

## Помощь / Help

### Показать справку / Show Help
```bash
./tools/validate-release.sh help
./tools/quick-release.sh
./prepare-release.sh --help
```

### Контакты / Contacts
- GitHub Issues: https://github.com/ImagingTools/ProLife/issues
- Documentation: См. файлы выше / See files above

---

**Quick Reference Version:** 1.0  
**Last Updated:** 2026-01-20
