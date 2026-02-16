# ProLife Development Guidelines

> **Руководство по разработке для системы ProLife**
> 
> Этот документ содержит правила и паттерны для работы с кодовой базой ProLife. 
> Следуйте этим рекомендациям при создании новых компонентов, страниц и функциональности.

---

## 📁 Архитектура проекта

### Основные директории

```
ProLife/
├── Partitura/          # ACC файлы конфигурации (XML)
│   ├── ProLifeQmlVoce.arp/   # Клиентские страницы и UI конфигурация
│   └── ProLifeVoce.arp/      # Серверные компоненты и бизнес-логика
├── Qml/Include/prolifeqml/   # QML компоненты UI
├── Sdl/prolife/1.0/          # GraphQL схемы (SDL файлы)
├── Include/                   # C++ заголовочные файлы
│   ├── prolifegql/           # GraphQL обработчики (C++)
│   ├── prolifedata/          # Утилиты и общие функции
│   ├── prolifedb/            # Работа с БД
│   └── prolifestyle/         # Стили
├── Impl/                     # C++ реализация плагинов
└── Tests/                    # Тесты (включая Playwright e2e)
```

### Слои приложения

```
┌─────────────────────────────────────────┐
│   QML UI Components (Presentation)      │
│   Qml/Include/prolifeqml/*.qml          │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│   ACC Configuration (Wiring)            │
│   Partitura/*.acc files                 │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│   SDL Schemas (Data Contracts)          │
│   Sdl/prolife/1.0/*.sdl                 │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│   C++ Backend (Business Logic)          │
│   Include/prolifegql/*.cpp              │
└─────────────────────────────────────────┘
```

---

## 🎯 Правило №1: Разделение ответственности

**ВАЖНО:** Каждый слой отвечает только за свою часть:

- **SDL** - определяет данные и контракты API
- **QML** - отображает UI и обрабатывает пользовательский ввод
- **ACC** - связывает компоненты и настраивает права доступа
- **C++** - реализует бизнес-логику и работу с данными

---

## 📝 Создание новой страницы в приложении

### Шаг 1: Определите схему данных (SDL)

Создайте файл `Sdl/prolife/1.0/MyEntity.sdl`:

```graphql
schema {
    query: Query
    mutation: Mutation
    namespace: prolife
    version: 1.0
}

import {
    "../../../../ImtCore/Sdl/imtbase/1.0/ImtCollection.sdl"
}

# Элемент списка (облегченная версия для таблицы)
type MyEntityItem {
    id: ID!
    typeId: ID!
    name: String!
    description: String
    status: String
    customerLink: ObjectLink
    added: String
    timeStamp: String
}

# Входные параметры для запроса списка
input MyEntityListInput {
    viewParams: CollectionViewParams
}

# Ответ со списком элементов
type MyEntityListPayload {
    items: [MyEntityItem]
    notification: NotificationItem
}

# Полные данные объекта (для редактора)
type MyEntityData {
    id: ID
    name: String
    description: String
    status: String
    customerId: ID
    # ... другие поля
}

# Входные данные для создания/обновления
input MyEntityDataInput {
    id: ID!
    item: MyEntityData!
}

# GraphQL запросы
type Query {
    MyEntityList(input: MyEntityListInput): MyEntityListPayload!
    MyEntity(input: InputId): MyEntityData!
}

# GraphQL мутации
type Mutation {
    MyEntityUpdate(input: MyEntityDataInput): UpdatedNotificationPayload!
    MyEntityAdd(input: MyEntityDataInput): AddedNotificationPayload!
}

# Описание коллекции для фреймворка
collectionSchema {
    type MyEntity {
        ref: MyEntityItem
        operations {
            list: MyEntityList
        }
        subtype MyEntity {
            ref: MyEntityData
            operations {
                get: MyEntity
                update: MyEntityUpdate
                insert: MyEntityAdd
            }
        }
    }
}
```

**Правила для SDL:**
- Используйте суффиксы: `Item` для списков, `Data` для деталей
- Всегда включайте `id`, `typeId`, `added`, `timeStamp`
- Используйте `ObjectLink` для ссылок на другие сущности
- Определяйте `Input` типы для параметров запросов

---

### Шаг 2: Создайте QML компоненты

#### 2.1 Collection View (Таблица)

Создайте `Qml/Include/prolifeqml/MyEntityCollectionView.qml`:

```qml
import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtguigql 1.0
import prolifeMyEntitySdl 1.0

RemoteCollectionView {
    id: container
    
    // ID коллекции (должен совпадать с ACC)
    collectionId: "MyEntity"
    
    // Делегат команд (кнопки в тулбаре)
    commandsDelegateComp: Component {
        DocumentCollectionViewDelegate {
            collectionView: container
            
            // Типы документов для работы
            documentTypeIds: ["MyEntity"]
            documentViewTypeIds: ["MyEntityEditor"]
            
            // Компоненты представлений
            documentViewsComp: [editorComp]
            
            Component {
                id: editorComp
                MyEntityEditor {}
            }
        }
    }
    
    // Настройка при создании
    Component.onCompleted: {
        // Установка сортировки по умолчанию
        table.setSortingInfo(MyEntityItemTypeMetaInfo.s_name, "ASC");
        
        // Регистрация фильтров для столбцов (опционально)
        registerFieldFilterDelegate("status", statusFilterComp);
        registerFieldFilterDelegate("customerLink", customerFilterComp);
    }
    
    // Компонент фильтра статуса (пример)
    Component {
        id: statusFilterComp
        ComboBoxFilterDelegate {
            model: ["Active", "Inactive", "Pending"]
        }
    }
    
    // Фильтр клиента (пример)
    Component {
        id: customerFilterComp
        CustomerFilterDelegate {}
    }
}
```

**Правила для Collection View:**
- Наследуйте от `RemoteCollectionView` для удаленных данных
- Всегда устанавливайте `collectionId`
- Используйте `DocumentCollectionViewDelegate` для стандартных операций
- Регистрируйте фильтры через `registerFieldFilterDelegate()`
- Устанавливайте сортировку по умолчанию в `Component.onCompleted`

#### 2.2 Editor (Редактор)

Создайте `Qml/Include/prolifeqml/MyEntityEditor.qml`:

```qml
import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import prolifeMyEntitySdl 1.0

DocumentEditorViewBase {
    id: root
    
    // Ссылка на модель данных
    property MyEntityData myEntityData: model ? model : null
    property bool isNew: false
    
    // Обработка изменения модели
    onMyEntityDataChanged: {
        checkPermissions();
    }
    
    // Проверка прав доступа
    function checkPermissions() {
        if (!myEntityData) {
            return;
        }
        
        let canEdit = PermissionsController.checkPermission("EditMyEntity");
        let canChangeName = PermissionsController.checkPermission("ChangeMyEntityName");
        
        if (isNew && canEdit) {
            setReadOnly(false);
        } else {
            nameInput.readOnly = !canChangeName;
            // ... другие поля
        }
    }
    
    // Контроллер команд (для Undo/Redo/Save)
    commandsControllerComp: Component {
        GqlBasedCommandsController {
            typeId: "MyEntity"
            validateComp: validatorComp
        }
    }
    
    // Валидатор
    Component {
        id: validatorComp
        MyEntityValidator {}
    }
    
    // Layout редактора
    contentComp: Component {
        BaseRectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Flickable {
                anchors.fill: parent
                contentHeight: mainLayout.height
                clip: true
                
                ColumnLayout {
                    id: mainLayout
                    width: parent.width
                    spacing: Style.spacing
                    
                    // Заголовок
                    BaseText {
                        text: isNew ? qsTr("New Entity") : qsTr("Edit Entity")
                        font.pixelSize: Style.fontSizeXL
                        font.bold: true
                    }
                    
                    // Поле имени
                    LabeledTextInput {
                        id: nameInput
                        Layout.fillWidth: true
                        labelText: qsTr("Name:")
                        text: myEntityData ? myEntityData.m_name : ""
                        onTextChanged: {
                            if (myEntityData) {
                                myEntityData.m_name = text;
                            }
                        }
                    }
                    
                    // Поле описания
                    LabeledTextArea {
                        id: descriptionInput
                        Layout.fillWidth: true
                        labelText: qsTr("Description:")
                        text: myEntityData ? myEntityData.m_description : ""
                        onTextChanged: {
                            if (myEntityData) {
                                myEntityData.m_description = text;
                            }
                        }
                    }
                    
                    // Выбор статуса
                    LabeledComboBox {
                        id: statusComboBox
                        Layout.fillWidth: true
                        labelText: qsTr("Status:")
                        model: ["Active", "Inactive", "Pending"]
                        currentIndex: {
                            if (!myEntityData) return 0;
                            return model.indexOf(myEntityData.m_status);
                        }
                        onCurrentTextChanged: {
                            if (myEntityData) {
                                myEntityData.m_status = currentText;
                            }
                        }
                    }
                    
                    // Ссылка на клиента (пример)
                    LabeledCollectionDropDown {
                        id: customerCB
                        Layout.fillWidth: true
                        labelText: qsTr("Customer:")
                        collectionId: "Accounts"
                        selectedId: myEntityData ? myEntityData.m_customerId : ""
                        onSelectedIdChanged: {
                            if (myEntityData) {
                                myEntityData.m_customerId = selectedId;
                            }
                        }
                    }
                }
            }
        }
    }
}
```

**Правила для Editor:**
- Наследуйте от `DocumentEditorViewBase`
- Используйте `property <Type>Data` для модели
- Всегда проверяйте права через `PermissionsController.checkPermission()`
- Используйте `GqlBasedCommandsController` для стандартных команд
- Подключайте валидатор через `validateComp`
- Используйте двустороннее связывание с `onTextChanged`/`onCurrentTextChanged`

#### 2.3 Validator (Валидатор)

Создайте `Qml/Include/prolifeqml/MyEntityValidator.qml`:

```qml
import QtQuick 2.15
import imtdocgui 1.0
import prolifeMyEntitySdl 1.0

DocumentValidator {
    id: root
    
    property MyEntityData myEntityData: documentModel
    
    // Функция валидации
    function validate() {
        var errors = [];
        
        // Проверка обязательных полей
        if (!myEntityData.m_name || myEntityData.m_name.trim() === "") {
            errors.push(qsTr("Name is required"));
        }
        
        // Проверка длины
        if (myEntityData.m_name && myEntityData.m_name.length > 100) {
            errors.push(qsTr("Name must be less than 100 characters"));
        }
        
        // Проверка формата
        if (myEntityData.m_status && !["Active", "Inactive", "Pending"].includes(myEntityData.m_status)) {
            errors.push(qsTr("Invalid status value"));
        }
        
        return errors;
    }
}
```

**Правила для Validator:**
- Наследуйте от `DocumentValidator`
- Реализуйте функцию `validate()` возвращающую массив ошибок
- Проверяйте обязательные поля, форматы, длины
- Используйте `qsTr()` для всех сообщений об ошибках

---

### Шаг 3: Настройте ACC файл

Создайте или отредактируйте `Partitura/ProLifeQmlVoce.arp/MyEntityPage.acc`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Acf>
    <AcfHeader>
        <VersionInfos>
            <Version Id="0" Number="5715" Description="ACF"/>
            <Version Id="1" Number="2274" Description="ACF-Solutions"/>
            <Version Id="1023" Number="395264" Description="Qt Framework"/>
        </VersionInfos>
    </AcfHeader>
    <Description>My Entity Page Configuration</Description>
    <Keywords></Keywords>
    <ElementsList>
        
        <!-- Проверка прав -->
        <Element Id="CheckPermissionOr" PackageId="ImtAuthPck" ComponentId="CheckPermissionOr">
            <Data IsEnabled="true" Flags="0">
                <AttributeInfoMap/>
            </Data>
        </Element>
        
        <!-- Заголовки таблицы -->
        <Element Id="CollectionHeadersProvider" PackageId="ImtColPck" ComponentId="CollectionHeadersProvider">
            <Data IsEnabled="true" Flags="0">
                <AttributeInfoMap>
                    <!-- Фильтруемые столбцы -->
                    <AttributeInfo Id="FilterableHeaderIds" Type="Id[]">
                        <Data IsEnabled="true">
                            <Values>
                                <Value>status</Value>
                                <Value>name</Value>
                                <Value>customerLink</Value>
                            </Values>
                        </Data>
                    </AttributeInfo>
                    
                    <!-- Все столбцы -->
                    <AttributeInfo Id="HeaderIds" Type="Id[]">
                        <Data IsEnabled="true">
                            <Values>
                                <Value>status</Value>
                                <Value>name</Value>
                                <Value>description</Value>
                                <Value>customerLink</Value>
                                <Value>added</Value>
                                <Value>timeStamp</Value>
                            </Values>
                        </Data>
                    </AttributeInfo>
                    
                    <!-- Названия столбцов -->
                    <AttributeInfo Id="HeaderNames" Type="Text[]">
                        <Data IsEnabled="true">
                            <Values>
                                <Value>Status</Value>
                                <Value>Name</Value>
                                <Value>Description</Value>
                                <Value>Customer</Value>
                                <Value>Added</Value>
                                <Value>Last Modified</Value>
                            </Values>
                        </Data>
                    </AttributeInfo>
                    
                    <!-- Сортируемые столбцы -->
                    <AttributeInfo Id="SortableHeaderIds" Type="Id[]">
                        <Data IsEnabled="true">
                            <Values>
                                <Value>status</Value>
                                <Value>name</Value>
                                <Value>customerLink</Value>
                                <Value>added</Value>
                                <Value>timeStamp</Value>
                            </Values>
                        </Data>
                    </AttributeInfo>
                </AttributeInfoMap>
            </Data>
        </Element>
        
        <!-- GraphQL Demultiplexer -->
        <Element Id="Demultiplexer" PackageId="ImtServerGqlPck" ComponentId="GraphQLDemultiplexer">
            <Data IsEnabled="true" Flags="0">
                <AttributeInfoMap>
                    <AttributeInfo Id="GqlRequestHandlers" Type="Reference[]">
                        <Data IsEnabled="true">
                            <Values>
                                <Value>MyEntityCollectionController</Value>
                                <Value>MyEntityCommands</Value>
                            </Values>
                        </Data>
                    </AttributeInfo>
                    <AttributeInfo Id="Log" Type="Reference" ExportId="Log">
                        <Data IsEnabled="false"/>
                    </AttributeInfo>
                </AttributeInfoMap>
            </Data>
        </Element>
        
        <!-- Контроллер коллекции (серверный) -->
        <Element Id="MyEntityCollectionController" PackageId="ProLifeGqlPck" ComponentId="MyEntityCollectionController">
            <Data IsEnabled="true" Flags="0">
                <AttributeInfoMap>
                    <!-- Ссылка на репозиторий -->
                    <AttributeInfo Id="MyEntityRepository" Type="Reference" ExportId="MyEntityRepository">
                        <Data IsEnabled="false"/>
                    </AttributeInfo>
                    <!-- Ссылка на другие коллекции (если нужно) -->
                    <AttributeInfo Id="AccountCollection" Type="Reference" ExportId="AccountCollection">
                        <Data IsEnabled="false"/>
                    </AttributeInfo>
                    <AttributeInfo Id="Log" Type="Reference" ExportId="Log">
                        <Data IsEnabled="false"/>
                    </AttributeInfo>
                </AttributeInfoMap>
            </Data>
        </Element>
        
        <!-- Команды документа -->
        <Element Id="MyEntityCommands" PackageId="ImtServerGqlPck" ComponentId="StandardDocumentCommandsController">
            <Data IsEnabled="true" Flags="0">
                <AttributeInfoMap>
                    <AttributeInfo Id="EmbeddedRegistry" Type="AcfEmbedded" ExportId="EmbeddedRegistry">
                        <Data IsEnabled="true">
                            <EmbeddedComponent Id="StandardDocumentCommandsController" PackageId="ImtServerGqlPck">
                                <!-- Права -->
                                <AttributeInfo Id="Permissions" Type="Id[]">
                                    <Data IsEnabled="true">
                                        <Values>
                                            <Value>Close</Value>
                                            <Value>History</Value>
                                            <Value>Redo</Value>
                                            <Value>Save</Value>
                                            <Value>Undo</Value>
                                        </Values>
                                    </Data>
                                </AttributeInfo>
                                <AttributeInfo Id="TypeId" Type="Id">
                                    <Data IsEnabled="true" Value="MyEntity"/>
                                </AttributeInfo>
                                <!-- Экспортируемые интерфейсы -->
                                <AttributeInfo Id="ExportedInterfaces" Type="Id[]">
                                    <Data IsEnabled="true">
                                        <Values>
                                            <Value>IGqlRepresentationController</Value>
                                            <Value>IGqlRequestHandler</Value>
                                            <Value>IRepresentationController</Value>
                                        </Values>
                                    </Data>
                                </AttributeInfo>
                            </EmbeddedComponent>
                        </Data>
                    </AttributeInfo>
                </AttributeInfoMap>
            </Data>
        </Element>
        
        <!-- Определение страницы -->
        <Element Id="MyEntityPage" PackageId="ImtCollectionVoce" ComponentId="MultiDocCollectionPage">
            <Data IsEnabled="true" Flags="0">
                <AttributeInfoMap>
                    <!-- ID коллекции -->
                    <AttributeInfo Id="CollectionId" Type="Id">
                        <Data IsEnabled="true" Value="MyEntity"/>
                    </AttributeInfo>
                    
                    <!-- Путь к QML компоненту -->
                    <AttributeInfo Id="CollectionViewQmlPath" Type="Text">
                        <Data IsEnabled="true" Value="qrc:/qml/ProLife/MyEntityCollectionView.qml"/>
                    </AttributeInfo>
                    
                    <!-- Иконка -->
                    <AttributeInfo Id="IconPath" Type="Text">
                        <Data IsEnabled="true" Value="Icons/MyEntity"/>
                    </AttributeInfo>
                    
                    <!-- Название страницы -->
                    <AttributeInfo Id="Name" Type="Text">
                        <Data IsEnabled="true" Value="My Entities"/>
                    </AttributeInfo>
                    
                    <!-- Права доступа к странице -->
                    <AttributeInfo Id="PagePermissions" Type="Id[]">
                        <Data IsEnabled="true">
                            <Values>
                                <Value>ViewMyEntity</Value>
                                <Value>AddMyEntity</Value>
                                <Value>RemoveMyEntity</Value>
                                <Value>EditMyEntity</Value>
                                <Value>ChangeMyEntityName</Value>
                            </Values>
                        </Data>
                    </AttributeInfo>
                    
                    <!-- Ссылки на компоненты -->
                    <AttributeInfo Id="CollectionHeadersProvider" Type="Reference">
                        <Data IsEnabled="true" Value="CollectionHeadersProvider"/>
                    </AttributeInfo>
                    <AttributeInfo Id="Demultiplexer" Type="Reference">
                        <Data IsEnabled="true" Value="Demultiplexer"/>
                    </AttributeInfo>
                </AttributeInfoMap>
            </Data>
        </Element>
        
    </ElementsList>
</Acf>
```

**Правила для ACC файлов:**

1. **Структура Element:**
   - `Id` - уникальный идентификатор
   - `PackageId` - пакет фреймворка
   - `ComponentId` - тип компонента

2. **Обязательные элементы страницы:**
   - `CheckPermissionOr` - проверка прав
   - `CollectionHeadersProvider` - заголовки таблицы
   - `Demultiplexer` - маршрутизация GraphQL запросов
   - Controller компонент - серверная логика
   - `StandardDocumentCommandsController` - команды документа

3. **AttributeInfo настройки:**
   - `Type="Id"` - идентификаторы
   - `Type="Text"` - текстовые значения
   - `Type="Id[]"` - массивы идентификаторов
   - `Type="Reference"` - ссылки на другие компоненты
   - `ExportId` - для авто-связывания компонентов
   - `IsEnabled="false"` - авто-связывание активно

4. **Порядок HeaderIds должен совпадать с HeaderNames**

---

### Шаг 4: Добавьте страницу в меню

Отредактируйте `Partitura/ProLifeQmlVoce.arp/Pages.acc` или главный конфигурационный файл:

```xml
<!-- В секции PagesList или NavigationMenu -->
<Element Id="NavigationMenu">
    <AttributeInfo Id="Pages" Type="Reference[]">
        <Data IsEnabled="true">
            <Values>
                <!-- ... существующие страницы ... -->
                <Value>MyEntityPage</Value>
            </Values>
        </Data>
    </AttributeInfo>
</Element>
```

---

## 🎨 Правила для QML компонентов

### Импорты

**Обязательные импорты:**
```qml
import QtQuick 2.15          // Базовый Qt
import Acf 1.0               // ACF фреймворк
import imtgui 1.0            // IMT GUI компоненты
import imtcontrols 1.0       // Контролы (кнопки, инпуты)
import imtcolgui 1.0         // Коллекции и таблицы
import imtguigql 1.0         // GraphQL интеграция
import prolifeMyEntitySdl 1.0  // SDL схема вашей сущности
```

### Стили

**Всегда используйте Style.* для UI параметров:**
```qml
BaseText {
    font.pixelSize: Style.fontSizeM    // Не используйте магические числа!
    color: Style.textColor             // Не "#000000"
}

BaseRectangle {
    color: Style.baseColor
    border.color: Style.borderColor
    border.width: Style.borderWidth
    radius: Style.radiusM
}

ColumnLayout {
    spacing: Style.spacing
    Layout.margins: Style.margins
}
```

**Доступные размеры:**
- Шрифты: `fontSizeXS`, `fontSizeS`, `fontSizeM`, `fontSizeL`, `fontSizeXL`
- Радиусы: `radiusS`, `radiusM`, `radiusL`
- Отступы: `spacing`, `margins`

### Права доступа (Permissions)

**Всегда проверяйте права перед изменением UI:**
```qml
function checkPermissions() {
    let canEdit = PermissionsController.checkPermission("EditMyEntity");
    let canDelete = PermissionsController.checkPermission("RemoveMyEntity");
    
    if (!canEdit) {
        setReadOnly(true);
    }
    
    deleteButton.visible = canDelete;
}
```

### GraphQL запросы из QML

**Используйте GqlSdlRequestSender:**
```qml
GqlSdlRequestSender {
    id: requestSender
    requestType: 0  // 0 = Query, 1 = Mutation
    gqlCommandId: ProLifeMyEntitySdlCommandIds.MyEntityList
    
    sdlObjectComp: Component {
        MyEntityListPayload {
            onFinished: {
                if (m_ok) {
                    // Успешный результат
                    console.log("Items count:", m_items.length);
                } else {
                    // Ошибка
                    showError(m_notification.m_message);
                }
            }
        }
    }
}

// Отправка запроса
function loadData() {
    let input = Qt.createQmlObject('
        import prolifeMyEntitySdl 1.0;
        MyEntityListInput {}
    ', this);
    
    requestSender.send(input);
}
```

**Правила для GraphQL в QML:**
- Используйте `GqlSdlRequestSender`, а не старые авто-генерируемые типы
- `requestType: 0` для Query, `requestType: 1` для Mutation
- Всегда проверяйте `m_ok` в `onFinished`
- Обрабатывайте ошибки через `m_notification.m_message`

### Обработка ошибок

**Показывайте ошибки пользователю:**
```qml
// В onFinished обработчике
if (!m_ok) {
    ModalDialogManager.showErrorDialog(
        qsTr("Error"),
        m_notification.m_message
    );
    return;
}

// Или для валидации
BaseText {
    text: errorMessage
    color: Style.errorTextColor
    visible: errorMessage !== ""
}
```

### Кешированные коллекции

**Используйте Cached* компоненты для часто используемых данных:**
```qml
// Инициализация в Component.onCompleted
Component.onCompleted: {
    if (!CachedProductCollection.completed) {
        CachedProductCollection.updateModel();
    }
    
    if (!CachedAccountCollection.completed) {
        CachedAccountCollection.updateModel();
    }
}

// Использование
LabeledCollectionDropDown {
    collectionId: "Accounts"
    // Автоматически использует кеш если доступен
}
```

### Фильтры

**Регистрация фильтров в Collection View:**
```qml
Component.onCompleted: {
    // ComboBox фильтр
    registerFieldFilterDelegate("status", statusFilterComp);
    
    // Кастомный фильтр
    registerFieldFilterDelegate("customerLink", customerFilterComp);
}

Component {
    id: statusFilterComp
    ComboBoxFilterDelegate {
        model: ["All", "Active", "Inactive", "Pending"]
        currentIndex: 0
    }
}

Component {
    id: customerFilterComp
    CustomerFilterDelegate {
        // Специальный компонент для фильтрации по клиентам
    }
}
```

---

## ⚙️ Правила для C++ Backend (Include/prolifegql/)

### Структура Controller компонента

**Минимальный шаблон:**
```cpp
// CMyEntityCollectionControllerComp.h
class CMyEntityCollectionControllerComp : public AcfComponent,
                                           public IGqlRequestHandler
{
public:
    // Обязательные методы
    virtual void OnMyEntityList(const prolifelicenses::MyEntityListInput& input,
                                IOnMyEntityListPayload* answer) = 0;
    
    // Реализация IGqlRequestHandler
    virtual void OnGqlRequest(const QString& operationName,
                             const IGqlSdlPayload* request,
                             IGqlSdlPayload* answer) override;
};

// CMyEntityCollectionControllerComp.cpp
void CMyEntityCollectionControllerComp::OnGqlRequest(
    const QString& operationName,
    const IGqlSdlPayload* request,
    IGqlSdlPayload* answer)
{
    if (operationName == "MyEntityList") {
        auto* input = qobject_cast<const prolifelicenses::MyEntityListInput*>(request);
        auto* payload = qobject_cast<IOnMyEntityListPayload*>(answer);
        if (input && payload) {
            OnMyEntityList(*input, payload);
        }
    }
}

void CMyEntityCollectionControllerComp::OnMyEntityList(
    const prolifelicenses::MyEntityListInput& input,
    IOnMyEntityListPayload* answer)
{
    // Получение данных
    auto repository = GetMyEntityRepository();
    if (!repository) {
        answer->SetNotification(CreateErrorNotification("Repository not available"));
        return;
    }
    
    // Фильтрация и сортировка
    QList<IMyEntityItem*> items;
    auto filter = input.viewParams().filter();
    auto sort = input.viewParams().sort();
    
    // Запрос к БД через repository
    repository->GetItems(filter, sort, items);
    
    // Заполнение ответа
    answer->SetItems(items);
    answer->SetNotification(CreateSuccessNotification());
}
```

### НЕ используйте I_REF между контроллерами

**❌ НЕПРАВИЛЬНО:**
```cpp
// НЕ ДЕЛАЙТЕ ТАК!
class CMyEntityControllerComp {
    I_REF(CSoftwareControllerComp, m_softwareController);
};
```

**✅ ПРАВИЛЬНО - используйте утилиты в prolifedata:**
```cpp
// Include/prolifedata/prolifedata.h
namespace prolifedata {
    // Общая утилита, доступная всем
    void BuildMyEntityTree(IMyEntityCollection* collection,
                          QList<LicenseTreeNode>& tree);
}

// Include/prolifedata/prolifedata.cpp
void prolifedata::BuildMyEntityTree(IMyEntityCollection* collection,
                                   QList<LicenseTreeNode>& tree)
{
    // Реализация общей логики
    // Принимает коллекцию как параметр
}

// Использование в контроллере
void CMyEntityControllerComp::OnGetTree(...)
{
    auto collection = GetMyEntityCollection();
    QList<LicenseTreeNode> tree;
    prolifedata::BuildMyEntityTree(collection, tree);
    answer->SetTree(tree);
}
```

### Встраивание сложных данных

**Для эффективности встраивайте вложенные данные в основную модель:**
```cpp
// ✅ Встроенное дерево в основной объект
type SoftwareProductData {
    id: ID
    serialNumber: String
    licenseTree: LicenseTreeNode  // Встроено!
}

// Заполнение в контроллере
void CSoftwareControllerComp::OnSoftwareProduct(...)
{
    auto product = repository->GetProduct(id);
    
    // Строим дерево сразу
    auto tree = prolifedata::BuildLicenseTree(
        GetSoftwareProductCollection(),
        product->id
    );
    
    product->SetLicenseTree(tree);
    answer->SetProduct(product);
}
```

**❌ Не используйте отдельные запросы для вложенных данных**

### Защита от циклических ссылок

**Всегда используйте защиту при обходе иерархий:**
```cpp
void TraverseParentChain(const QString& startId)
{
    QSet<QString> visitedIds;  // Защита от циклов
    const int MAX_DEPTH = 100;  // Лимит глубины
    int depth = 0;
    
    QString currentId = startId;
    while (!currentId.isEmpty() && depth < MAX_DEPTH) {
        if (visitedIds.contains(currentId)) {
            // Обнаружен цикл!
            LogError("Circular reference detected");
            break;
        }
        visitedIds.insert(currentId);
        
        // Обработка элемента
        auto item = GetItem(currentId);
        ProcessItem(item);
        
        // Переход к родителю
        currentId = item->parentId();
        depth++;
    }
}
```

### Обработка ошибок

**Всегда возвращайте notification:**
```cpp
void CMyEntityControllerComp::OnMyEntityUpdate(...)
{
    // Валидация
    if (input.item().name().isEmpty()) {
        answer->SetNotification(CreateErrorNotification(
            tr("Name is required")
        ));
        return;
    }
    
    // Проверка прав
    if (!CheckPermission("EditMyEntity")) {
        answer->SetNotification(CreateErrorNotification(
            tr("Access denied")
        ));
        return;
    }
    
    // Выполнение операции
    try {
        repository->UpdateItem(input.item());
        answer->SetNotification(CreateSuccessNotification(
            tr("Entity updated successfully")
        ));
    } catch (const std::exception& e) {
        answer->SetNotification(CreateErrorNotification(
            tr("Update failed: %1").arg(e.what())
        ));
    }
}
```

---

## 📊 Бизнес-логика в QML

### Вычисляемые свойства

**Используйте property binding для реактивности:**
```qml
ViewBase {
    property SoftwareProductData data: model
    
    // Вычисляемое свойство
    readonly property bool hasExpiration: data && data.m_expiration !== ""
    readonly property bool isExpired: {
        if (!hasExpiration) return false;
        return new Date(data.m_expiration) < new Date();
    }
    
    // Использование
    BaseText {
        text: isExpired ? qsTr("Expired") : qsTr("Active")
        color: isExpired ? Style.errorTextColor : Style.successTextColor
    }
}
```

### Сложные вычисления - выносите в функции

**Не пишите сложную логику прямо в binding:**
```qml
// ❌ ПЛОХО
height: {
    let h = 0;
    for (let i = 0; i < items.length; i++) {
        if (items[i].visible) {
            h += items[i].height + spacing;
        }
    }
    return h + margins * 2;
}

// ✅ ХОРОШО
function calculateTotalHeight() {
    let h = 0;
    for (let i = 0; i < items.length; i++) {
        if (items[i].visible) {
            h += items[i].height + spacing;
        }
    }
    return h + margins * 2;
}

height: calculateTotalHeight()
```

### Состояние загрузки

**Показывайте индикаторы загрузки:**
```qml
BaseRectangle {
    property bool isLoading: false
    
    ColumnLayout {
        visible: !isLoading
        // Основной контент
    }
    
    BusyIndicator {
        anchors.centerIn: parent
        visible: isLoading
        running: isLoading
    }
}
```

### Диалоги подтверждения

**Всегда спрашивайте подтверждение для деструктивных операций:**
```qml
function deleteEntity() {
    ModalDialogManager.showConfirmDialog(
        qsTr("Confirm Delete"),
        qsTr("Are you sure you want to delete this entity?"),
        function(confirmed) {
            if (confirmed) {
                performDelete();
            }
        }
    );
}

function performDelete() {
    // Отправка запроса удаления
    deleteRequestSender.send(createDeleteInput());
}
```

---

## 🔐 Права доступа (Permissions)

### Иерархия прав

**Права организованы по сущностям:**
```
ViewMyEntity          - просмотр списка
AddMyEntity           - создание новых
RemoveMyEntity        - удаление
EditMyEntity          - общее редактирование
ChangeMyEntityName    - изменение конкретного поля
ViewAllMyEntities     - просмотр всех (не только своих)
```

### Регистрация прав в ACC

**В файле Permissions.acc:**
```xml
<Element Id="MyEntityPermissions" PackageId="ImtAuthPck" ComponentId="PermissionItems">
    <Data IsEnabled="true" Flags="0">
        <AttributeInfoMap>
            <AttributeInfo Id="Permissions" Type="Id[]">
                <Data IsEnabled="true">
                    <Values>
                        <Value>ViewMyEntity</Value>
                        <Value>AddMyEntity</Value>
                        <Value>RemoveMyEntity</Value>
                        <Value>EditMyEntity</Value>
                        <Value>ChangeMyEntityName</Value>
                        <Value>ViewAllMyEntities</Value>
                    </Values>
                </Data>
            </AttributeInfo>
        </AttributeInfoMap>
    </Data>
</Element>
```

### Использование в QML

**Детальная проверка прав:**
```qml
function checkPermissions() {
    // Проверка на уровне страницы
    if (!PermissionsController.checkPermission("ViewMyEntity")) {
        showAccessDenied();
        return;
    }
    
    // Проверка на уровне действий
    addButton.visible = PermissionsController.checkPermission("AddMyEntity");
    deleteButton.visible = PermissionsController.checkPermission("RemoveMyEntity");
    
    // Проверка на уровне полей
    if (isNew) {
        setReadOnly(!PermissionsController.checkPermission("AddMyEntity"));
    } else {
        let canEdit = PermissionsController.checkPermission("EditMyEntity");
        let canChangeName = PermissionsController.checkPermission("ChangeMyEntityName");
        
        nameInput.readOnly = !canChangeName;
        descriptionInput.readOnly = !canEdit;
    }
}
```

### Использование в C++

**Серверная проверка прав:**
```cpp
void CMyEntityControllerComp::OnMyEntityUpdate(...)
{
    auto permissionChecker = GetPermissionChecker();
    if (!permissionChecker || !permissionChecker->CheckPermission("EditMyEntity")) {
        answer->SetNotification(CreateErrorNotification(
            tr("You don't have permission to edit entities")
        ));
        return;
    }
    
    // Выполнение операции
    // ...
}
```

---

## 📋 Чек-лист создания новой страницы

- [ ] **SDL Schema (Sdl/prolife/1.0/MyEntity.sdl)**
  - [ ] Определены типы Item и Data
  - [ ] Созданы Input типы
  - [ ] Определены Query операции (list, get)
  - [ ] Определены Mutation операции (add, update, delete)
  - [ ] Настроен collectionSchema

- [ ] **QML Components (Qml/Include/prolifeqml/)**
  - [ ] Collection View создан
  - [ ] Editor создан
  - [ ] Validator создан
  - [ ] Фильтры настроены (если нужно)
  - [ ] Проверка прав реализована
  - [ ] Обработка ошибок добавлена

- [ ] **ACC Configuration (Partitura/ProLifeQmlVoce.arp/)**
  - [ ] CheckPermissionOr настроен
  - [ ] CollectionHeadersProvider создан
  - [ ] Demultiplexer настроен
  - [ ] Controller компонент добавлен
  - [ ] StandardDocumentCommandsController настроен
  - [ ] Page элемент создан
  - [ ] Права доступа определены

- [ ] **C++ Backend (Include/prolifegql/)**
  - [ ] Controller компонент реализован
  - [ ] Обработчики GraphQL запросов созданы
  - [ ] Repository интеграция добавлена
  - [ ] Валидация и обработка ошибок реализованы

- [ ] **Permissions (Partitura/ProLifeVoce.arp/Permissions.acc)**
  - [ ] Права доступа зарегистрированы

- [ ] **Navigation**
  - [ ] Страница добавлена в меню

- [ ] **Testing**
  - [ ] Создание работает
  - [ ] Редактирование работает
  - [ ] Удаление работает
  - [ ] Фильтрация работает
  - [ ] Сортировка работает
  - [ ] Права доступа проверены

---

## 🎓 Примеры из кодовой базы

### Хорошие примеры для изучения:

1. **Простая страница:** `CustomersPage` (Accounts)
   - Файлы: `AccountCollectionView.qml`, `AccountEditor.qml`, `Accounts.sdl`
   - Базовый CRUD без сложной логики

2. **Средняя сложность:** `OrdersPage`
   - Файлы: `OrderCollectionView.qml`, `OrderEditor.qml`, `Orders.sdl`
   - Фильтры, связи с другими сущностями

3. **Сложная страница:** `SoftwareProductsPage`
   - Файлы: `SoftwareProductCollectionView.qml`, `SoftwareEditor.qml`, `Licenses.sdl`
   - Иерархии, деревья, множественные связи, встроенные данные

4. **Специальные компоненты:**
   - `LicenseTreeCanvas.qml` - Canvas визуализация
   - `RevokeLicenseDialog.qml` - Диалоги с GraphQL
   - `WorkspacePage.qml` - Дашборд с графиками

---

## 🚨 Частые ошибки и как их избежать

### 1. Не забывайте про права доступа

**❌ ОШИБКА:**
```qml
Button {
    text: qsTr("Delete")
    onClicked: deleteEntity()
}
```

**✅ ПРАВИЛЬНО:**
```qml
Button {
    text: qsTr("Delete")
    visible: PermissionsController.checkPermission("RemoveMyEntity")
    onClicked: deleteEntity()
}
```

### 2. Всегда проверяйте m_ok в GraphQL ответах

**❌ ОШИБКА:**
```qml
onFinished: {
    processData(m_items);
}
```

**✅ ПРАВИЛЬНО:**
```qml
onFinished: {
    if (!m_ok) {
        showError(m_notification.m_message);
        return;
    }
    processData(m_items);
}
```

### 3. Не используйте магические числа

**❌ ОШИБКА:**
```qml
font.pixelSize: 14
spacing: 8
color: "#333333"
```

**✅ ПРАВИЛЬНО:**
```qml
font.pixelSize: Style.fontSizeM
spacing: Style.spacing
color: Style.textColor
```

### 4. Защищайтесь от null/undefined

**❌ ОШИБКА:**
```qml
text: myEntityData.m_name
```

**✅ ПРАВИЛЬНО:**
```qml
text: myEntityData ? myEntityData.m_name : ""
```

### 5. Не создавайте зависимости между контроллерами

**❌ ОШИБКА:**
```cpp
I_REF(CMyOtherController, m_otherController);
```

**✅ ПРАВИЛЬНО:**
```cpp
// В prolifedata.h/cpp
namespace prolifedata {
    void SharedFunction(ICollection* collection, ...);
}
```

### 6. Используйте кешированные коллекции

**❌ ОШИБКА (медленно):**
```qml
// Каждый раз запрос к серверу
LabeledCollectionDropDown {
    collectionId: "Accounts"
}
```

**✅ ПРАВИЛЬНО:**
```qml
Component.onCompleted: {
    if (!CachedAccountCollection.completed) {
        CachedAccountCollection.updateModel();
    }
}

LabeledCollectionDropDown {
    collectionId: "Accounts"
    // Автоматически использует кеш
}
```

---

## 📚 Дополнительные ресурсы

### Документация фреймворка

- **ACF Framework:** `/docs/acf/` (если есть)
- **Qt Quick:** https://doc.qt.io/qt-5/qtquick-index.html
- **GraphQL:** https://graphql.org/

### Полезные файлы в проекте

- `BUILDING.md` - инструкции по сборке
- `CONTRIBUTING.md` - правила контрибуции
- `QUICK_REFERENCE.md` - быстрая справка
- `README.md` - общая информация

---

## 🔄 Версионирование документа

**Версия:** 1.0  
**Дата:** 2026-02-16  
**Статус:** Draft - Ожидается обратная связь

---

## 💬 Обратная связь

Этот документ создан для упрощения разработки в проекте ProLife.  
Если вы нашли ошибки, неточности или хотите добавить новые разделы:

1. Оставьте комментарий в Pull Request
2. Создайте Issue в репозитории
3. Обсудите с командой

**Документ будет обновляться на основе ваших комментариев и замечаний.**

---

## ✨ Следующие шаги

После одобрения этого документа:

1. Создать примеры кода в `/examples/`
2. Добавить scaffolding скрипты для автоматической генерации
3. Создать templates для новых компонентов
4. Добавить линтеры и валидаторы для проверки соответствия правилам

---

*Этот документ является живым и должен обновляться по мере развития проекта.*
