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

# 2. Обновление субмодулей / Update submodules
./tools/validate-release.sh auto-update

# 3. Создание ветки / Create branch
./prepare-release.sh --create-release 2.1.0

# 4. Закрепление версий / Pin versions
./prepare-release.sh --pin-submodules

# 5. Генерация отчета / Generate report
./tools/validate-release.sh report

# 6. Финальная валидация / Final validation
./tools/validate-release.sh validate v2.1.0
```

## Инструменты / Tools

| Инструмент / Tool | Назначение / Purpose | Команда / Command |
|-------------------|----------------------|-------------------|
| **quick-release.sh** | Автоматическая подготовка релиза | `./tools/quick-release.sh 2.1.0` |
| **validate-release.sh** | Валидация и автоматизация | `./tools/validate-release.sh validate` |
| **prepare-release.sh** | Ручное управление субмодулями | `./prepare-release.sh --help` |

## Основные операции / Common Operations

### Проверка состояния / Check Status
```bash
./prepare-release.sh --status
./tools/validate-release.sh validate
```

### Обновление одного субмодуля / Update One Submodule
```bash
./prepare-release.sh --list-tags 3rdParty/Acf
./prepare-release.sh --update-submodule 3rdParty/Acf v2.0.0
```

### Обновление всех субмодулей / Update All Submodules
```bash
./tools/validate-release.sh auto-update
# или / or
./prepare-release.sh --update-all
```

### Интерактивное обновление / Interactive Update
```bash
./prepare-release.sh --interactive
```

### Генерация отчета / Generate Report
```bash
./tools/validate-release.sh report VERSIONS.md
./prepare-release.sh --report
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

### Откат субмодуля / Rollback Submodule
```bash
./prepare-release.sh --update-submodule 3rdParty/Acf v1.9.0
git add 3rdParty/Acf
git commit -m "chore(deps): rollback Acf to v1.9.0"
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

### Субмодули не инициализированы / Submodules Not Initialized
```bash
./prepare-release.sh --init
```

### Права доступа / Permission Denied
```bash
chmod +x tools/*.sh prepare-release.sh
```

### Незакоммиченные изменения / Uncommitted Changes
```bash
git status
git add . && git commit -m "..."
# или / or
git stash
```

### Конфликты субмодулей / Submodule Conflicts
```bash
cd 3rdParty/Acf
git fetch --all
git reset --hard origin/main
cd ../..
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
