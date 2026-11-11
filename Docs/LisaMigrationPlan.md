# План: перенос функционала Lisa в ProLife и каталог Hardware

## Кратко

ProLife уже частично зависит от Lisa: `LisaGQLEngine`, `LisaRepositories`, отдельная БД `lisa`, Lisa URL/WebSocket в настройках.
Каталог Features/Products/Licenses физически реализован в ImtCore (`imtlic`, `imtlicdb`, `imtlicgql`, `imtlicgui`);
Lisa вносит только композицию (Partitura), права, локализацию, SDK и конвертер. Поэтому перенос — это перевязка
этих компонентов на ProLife, миграция данных в БД ProLife и удаление всех ссылок на Lisa.

Для железа вводится отдельный каталог по образцу фич:

| Программный продукт | Аппаратный продукт |
|---|---|
| Feature (дерево фич, requirements) | **HardwareComponent** — модуль/комплектующее, дерево подмодулей, аппаратные атрибуты |
| Product (категория Software) + список фич | Product (категория Hardware) + **HardwareProductSpec** (BOM с количествами и вариантами) |
| License definition (набор фич) | **Конфигурация** = license definition аппаратного продукта (набор компонентов) |
| Product instance / лицензия | **Device** + фактически установленные компоненты с серийными номерами |

Конфигурация как license definition сохраняет совместимость с текущей логикой Devices:
`configurationName` / `configurationArticle` берутся из `MIT_LICENSE_NAME` / `MIT_LICENSE_ID`
(`CDeviceCollectionControllerComp`, `CDeviceCollectionDocumentServiceComp`).

## Принятые решения

- Данные Lisa переносятся в БД ProLife миграцией; в рантайме Lisa больше не нужна.
- Объём: страницы и API Features/Products/Licenses, LisaSdk, LisaConverter / `CDatabaseConverterComp`, Playwright-тесты LisaGui.
- Не переносится: LisaSettingsPlugin, legacy Accounts/Installations/FeaturePackages, инсталляторы `Install/`.
- `HardwareComponent` — отдельная сущность со своей SDL, таблицей и редактором.
- Атрибуты компонента: part number, ревизия, производитель, поставщик, MPN, количество в составе (BOM),
  опциональность и взаимоисключающие варианты, requirements / incompatibilities, технические параметры (key/value),
  жизненный цикл (Active / NRND / EOL), признак serialized, совместимые версии firmware.
- Устройство (Device) хранит фактический состав с серийными номерами модулей.
- Новая страница **Hardware** (аппаратные продукты + компоненты); `DevicesPage` переименовывается в **Devices**.

## Шаги

### Фаза 1. Каталог Features/Products/Licenses внутри ProLife (блокирует фазы 2, 3, 5)

1. Перенести из `Lisa/Partitura/LisaVoce.arp` в [ProLifeVoce.arp](../Partitura/ProLifeVoce.arp): `Repositories`,
   `FeatureSqlRepository`, `ProductSqlRepository`, `LicenseSqlRepository`, `RootDataController`,
   `FeaturesPermissions`, `ProductPermissions`. Таблицы назначить на БД ProLife.
2. Перенести GraphQL-связки из `Lisa/Partitura/LisaGraphQlVoce.arp/LisaGQLEngine.acc` и `LisaHandlers`
   (list/get/add/update для Feature, Product с import/export и dependencies, License). В
   [ProLifeServerBase.acc](../Partitura/ProLifeQmlVoce.arp/ProLifeServerBase.acc) заменить все ссылки
   `LisaGQLEngine/*` и `LisaRepositories/*` на локальные компоненты.
3. Удалить настройки подключения к Lisa:
   - `LisaDatabaseAccessSettings`, `LisaConnectionInterface` — [ProLifeServerSettings.acc](../Partitura/ProLifeVoce.arp/ProLifeServerSettings.acc);
   - `LisaUrlParam`, `LisaWebSocketParam` — [ServerPageData.acc](../Partitura/ProLifeQmlVoce.arp/ServerPageData.acc);
   - `LisaUrlRepresentation`, `LisaWsUrlRepresentation` — [ServerPageRepresentation.acc](../Partitura/ProLifeQmlVoce.arp/ServerPageRepresentation.acc);
   - `LisaUrlConnection` — [ProLifeApplicationSettings.acc](../Partitura/ProLifeQmlVoce.arp/ProLifeApplicationSettings.acc);
   - захардкоженный `http://localhost:7776/Lisa/graphql` — [ProductsPage.acc](../Partitura/ProLifeQmlVoce.arp/ProductsPage.acc);
   - экспорт `LisaDatabaseConnector`, `LisaLoginStatusSubscriberController`;
   - [ProLifeHandlersTest.acc](../Partitura/ProLifeVoce.arp/ProLifeHandlersTest.acc).
4. Добавить группы прав `FeatureManagement`, `ProductManagement`, `LicenseDefinitionManagement`
   (из `Lisa/Impl/LisaServer/LisaFeatures.h`) в [ProLifeFeatures.h](../Impl/ProLifeServer/ProLifeFeatures.h)
   и [ProLifeFeatures.xml](../Impl/ProLifeServer/ProLifeFeatures.xml).
5. Зарегистрировать страницы Features, Products, Licenses в [Pages.acc](../Partitura/ProLifeQmlVoce.arp/Pages.acc)
   (шаблон — `Lisa/Partitura/LisaQmlVoce.arp/Pages.acc`; вью `FeatureCollectionView.qml`,
   `ProductCollectionView.qml`, `LicenseCollectionView.qml` и их редакторы из ImtCore). Products показывает
   только категорию Software; аппаратные продукты — на странице Hardware.
6. В [ProLifeMain.qml](../Qml/Include/prolifeqml/ProLifeMain.qml) обновлять кэши фич/продуктов/лицензий при
   login/logout, как в `Lisa/Include/lisaqml/Qml/LisaMain.qml`.
7. Перенести переводы Lisa в `ProLifeLoc`.

### Фаза 2. Перенос данных (зависит от фазы 1)

8. Новая миграция (следующая после 19): document-таблицы `Features`, `Products`, `Licenses` через
   `ObjectCollectionMigrationController` (как миграция 14 в `Lisa/Partitura/LisaQmlVoce.arp/LisaServerBase.acc`).
9. Расширить [CDatabaseConverterComp](../Include/prolifedb/CDatabaseConverterComp.h) одноразовым импортом из
   БД `lisa` (логика из `Lisa/Include/lisadb/CDatabaseConverterComp.cpp`). Импорт идемпотентный, ID документов
   сохраняются — на них ссылаются Orders и Devices. Подключение к БД `lisa` остаётся только у конвертера.
10. CLI-режим `ProLifeConverter`, заменяющий LisaConverter.

### Фаза 3. SDK (параллельно с фазой 2)

11. Перенести `Lisa/Impl/LisaSdk` в `ProLife/Impl/ProLifeSdk` (namespace `ProLifeSdk`, `CLicenseController`:
    `SetLicenseFilePath`, `SetPublicKey`, `ImportLicense`, `GetFeatureList`). Для совместимости — алиас-заголовок `LisaSdk.h`.

### Фаза 4. Аппаратный каталог (зависит от шагов 1–4)

12. **SDL компонентов** — `Sdl/prolife/1.0/HardwareComponents.sdl` по образцу `ImtCore/Sdl/imtlic/1.0/Features.sdl`:
    - `HardwareComponentData`: `componentId`, `name`, `description`, `componentType` (Module / Part / Assembly / Consumable),
      `partNumber`, `revision`, `manufacturer`, `supplier`, `mpn`, `lifecycleStatus` (Active / NRND / EOL), `serialized`,
      `requirements [ID]`, `incompatibilities [ID]`, `parameters [HardwareParameter{key, name, value, unit}]`,
      `firmwareCompatibility [FirmwareRange{firmwareId, minVersion, maxVersion}]`;
    - `HardwareComponentNode{component, quantity, optional, alternativeGroup}` — рекурсивные подмодули;
      количество, опциональность и группа альтернатив — свойства узла состава, а не карточки компонента;
    - `HardwareComponentItem`, list/get/add/update, `collectionSchema`.
13. **SDL спецификации** — `Sdl/prolife/1.0/HardwareProducts.sdl`: `HardwareProductSpec` 1:1 с продуктом категории
    Hardware по `productId`; `bom [BomEntry{componentId, quantity, optional, alternativeGroup, selectedSubComponentIds}]`
    и сводная валидация. Конфигурации — license definitions продукта, их `featureIds` = `componentIds`.
    Обе SDL подключить в [Sdl/prolife/CMake/CMakeLists.txt](../Sdl/prolife/CMake/CMakeLists.txt).
14. **Domain** (`prolifedata`): `IHardwareComponentInfo` / `CHardwareComponentInfo` (дерево и сериализация по образцу
    `imtlic::CFeatureInfo`), `IHardwareProductSpec` / `CHardwareProductSpec`, валидатор `CHardwareConfigurationValidator`:
    requirements, incompatibilities, не более одного варианта в группе альтернатив, отсутствие циклов, наличие
    обязательных позиций BOM, предупреждения для EOL/NRND.
15. **БД** (`prolifedb`): `CHardwareComponentDatabaseDelegateComp`, `CHardwareProductSpecDatabaseDelegateComp`
    (JSONB, по образцу `CDeviceDatabaseDelegateComp`); миграция таблиц `HardwareComponents`, `HardwareProductSpecs`;
    repository `.acc` по образцу [DeviceSqlRepository.acc](../Partitura/ProLifeVoce.arp/DeviceSqlRepository.acc).
16. **GraphQL** (`prolifegql`): `CHardwareComponentCollectionControllerComp`, `CHardwareProductSpecControllerComp`
    с проверкой валидатором при сохранении конфигурации; change-generators по образцу
    `CHardwareBindingChangeGeneratorComp`; алиасы в [ProLifeGqlPck.h](../Impl/ProLifeGqlPck/ProLifeGqlPck.h).
17. **Права**: группа `HardwareManagement` — ViewPage, View, Edit/Add/Remove/Change
    (+ Components, Specification, Configurations, Lifecycle) с requirements вида `/HardwareManagement/View`.

### Фаза 5. UI страницы Hardware (зависит от фазы 4)

18. `HardwarePage` с переключателем «Компоненты» / «Аппаратные продукты»:
    `HardwareComponentCollectionView.qml` (колонки part number, revision, manufacturer, lifecycle, serialized) и
    `HardwareProductCollectionView.qml` (Products с категорией Hardware). Регистрация в Pages.acc и
    `HardwarePage.acc` по образцу [DevicesPage.acc](../Partitura/ProLifeQmlVoce.arp/DevicesPage.acc).
19. `HardwareComponentEditor.qml` на основе логики `ImtCore/Qml/imtlicgui/FeatureEditor.qml`
    (`rebuildFeatureTree`, requirement-пути) + вкладки «Параметры» (key/value), «Firmware»,
    «Поставка» (manufacturer, supplier, MPN), бейдж жизненного цикла, выбор incompatibilities.
20. `HardwareProductView.qml` (имя `HardwareProductEditor.qml` уже занято этапом заказа): поля продукта, BOM-дерево
    с количествами, optional и группами альтернатив; вкладка «Конфигурации» — license definitions продукта с выбором
    компонентов (аналог `LicenseEditor.qml`) и подсветкой нарушений валидатора.
21. Переименовать `DevicesPage` в «Devices». Навигацию `Devices/<hardware-filter>` в
    [WorkspacePage.qml](../Qml/Include/prolifeqml/WorkspacePage.qml) не менять.

### Фаза 6. Фактическая конфигурация устройства (зависит от фазы 4)

22. Расширить `IDeviceInfo` / `CDeviceInfo` и `DeviceData` в [Sensors.sdl](../Sdl/prolife/1.0/Sensors.sdl) полем
    `installedComponents [InstalledComponent{componentId, serialNumber, revision, firmwareVersion, quantity}]`.
23. Для существующих устройств `installedComponents` заполняется из их конфигурации (license) — в миграции или при
    первом открытии. Для компонентов с `serialized` серийный номер обязателен при статусе производства «собрано».
24. В [DeviceEditor.qml](../Qml/Include/prolifeqml/DeviceEditor.qml) — секция «Состав» под существующим правом
    `/SensorManagement/Edit/Change/HardwareConfiguration`; отклонения от конфигурации показываются предупреждением.

### Фаза 7. Тесты и отключение Lisa (финальная)

25. Перенести Playwright-спеки из `Lisa/Tests/LisaGui` (features, products, licenses; collection и editor) и page
    objects в [Tests/ProLifeGui](../Tests/ProLifeGui). Добавить спеки для hardware-компонентов, hardware-продуктов
    и состава устройства.
26. C++-тесты в [prolifetest](../Include/prolifetest): валидатор, сериализация `CHardwareComponentInfo`, импорт из Lisa.
27. Postman/CI: убрать запуск `LisaServerTest.exe` и восстановление тестовых данных Lisa
    ([ProLifeApiPostman](../Tests/ProLifeApiPostman/README.md)); добавить коллекции для новых GraphQL-операций.
28. Удалить пути Lisa из [ProLife.awc](../Config/ProLife.awc), CMake-зависимостей, [BUILDING.md](../BUILDING.md)
    и release workflows.

Рекомендуется отдельный PR на каждую группу фаз: 1+2, 3, 4+5, 6, 7.

## Проверка

1. Сборка CMake (`Build/CMake`) и генерация SDL без ошибок.
2. `grep -ri lisa ProLife/` — допустимы только конвертер и алиас SDK.
3. Конвертер на копии БД `lisa`: число документов Features/Products/Licenses совпадает, ID сохранены,
   повторный запуск ничего не дублирует.
4. Перенесённые Playwright-тесты Lisa проходят на `ProLifeServerTest`; новые hardware-спеки и Postman-коллекции
   проходят без запущенной Lisa.
5. Unit-тесты валидатора: конфликт альтернатив, incompatibility, отсутствующий requirement, EOL-предупреждение.
6. Вручную: компонент → продукт с BOM → конфигурация → устройство; состав и серийники сохраняются, Orders и
   Workspace работают как раньше.

## Открытые вопросы

1. Конфигурации как license definitions (рекомендуется, совместимо с Devices и Orders) или отдельная сущность
   `HardwareConfiguration` — чище, но требует переноса `MIT_LICENSE_*` у устройств.
2. Производители и поставщики: свободный текст (рекомендуется на первом этапе) или отдельная коллекция.
3. Если `ProductCollectionView.qml` нельзя отфильтровать по категории через параметры страницы, нужна минимальная
   правка в ImtCore.
