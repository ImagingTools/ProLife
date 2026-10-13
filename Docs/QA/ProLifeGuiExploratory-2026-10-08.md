# ProLife GUI: исследовательское тестирование, 2026-10-08

Стенд: `http://localhost:7778` (ProLifeServer, Debug_Qt6_VC17_x64). Браузер: Chromium (Playwright), окно 1600×900.
Аккаунты: `su` (суперпользователь) и `Roman`. Две сессии работали одновременно.
Исходное состояние Roman: роль **Test**, группа **QUISS** (у группы ролей нет). После тестов исходное состояние восстановлено.
Все тестовые данные создавались с префиксом `QA_` и удалены через GUI (см. раздел 6).
Скриншоты лежат в `screens/`.

## 1. Сводка

| Раздел | Дефекты | UX-замечания |
|---|---|---|
| Права и роли (Administration, профили Roman) | 5 (2 Critical) | 6 |
| Hardware (Devices) | 5 (1 Critical) | 6 |
| Software | 2 | 3 |
| Orders | 2 | 3 |
| Customers | 2 | 2 |
| Organizations | 1 | 3 |
| Support (Tickets) | 3 | 3 |
| Workspace (Dashboard / User Actions / Analytics) | 3 | 3 |
| Совместная работа двух сессий | 2 (1 Critical) | 1 |
| Общее (поиск, настройки, профиль, логин, консоль) | 4 | 6 |
| **Второй проход** (лицензии, ревизии, организации, пользователи и группы, пароль и тема, узкие экраны, Sign up) — раздел 7 | 17 (1 Critical) | 7 |

Пояснение к пометкам:
- «Повтор ×2» — дефект воспроизведён минимум два раза.
- «×1» — воспроизведён один раз, повтор не выполнялся.

---

## 2. Дефекты

### Critical

**D-01. После снятия роли её права остаются у пользователя, и сервер их выполняет.** Повтор ×2.
- Страница: Administration → Users → Roman; Profile → Roles & Permissions.
- Аккаунт: su меняет роли; Roman проверяет.
- Шаги:
  1. Под su снять с Roman роль *Test* и назначить *QA_Roman ReadOnly* (только права View*). Сохранить.
  2. Roman: Logout, затем Login.
  3. Открыть Profile → Roles & Permissions.
  4. Hardware → New Hardware → заполнить поля → Save.
- Ожидание: эффективные права равны правам ReadOnly; создание Hardware отклоняется.
- Факт:
  - В профиле указана роль только ReadOnly, но в списке прав есть Add Sensor, Bind/Unbind Sensor, Create License File, Transfer Licenses, Change Order/Description/Mac Address. Это права роли *Test*.
  - Создание Hardware проходит: QA-SN-RO-1 в старой сессии, QA-SN-RO-2 после чистого перелогина.
  - В редакторе пользователя (General / Assigned Roles / Group Membership / History) эти права нигде не видны, и снять их из GUI нельзя.
- Скриншоты: [62](screens/62-roman-ro-new-hw.png), [66](screens/66-roman-ro-fresh-new-hw-save.png), [191](screens/191-roman-profile-roles.png), [56](screens/56-su-roman-roles-saved.png).
- Гипотеза: у пользователя (или у группы QUISS) хранятся прямые права, либо кэш эффективных прав не сбрасывается при изменении ролей. Нужна проверка на сервере.

**D-02. Hardware: права на уровне полей не проверяются ни в UI, ни на сервере.** Повтор ×2.
- Страница: Hardware → редактор.
- Аккаунт: Roman (роль Test: Change Serial Number, Change Device Type и Change Hardware Configuration **не** выданы).
- Шаги: открыть Hardware → изменить Serial Number → Save.
- Ожидание: поле недоступно для редактирования, либо сервер отклоняет сохранение.
- Факт:
  - Поля Serial Number, Device Type и HW Configuration редактируются.
  - Изменение Serial Number сохраняется: QA-SN-0001 → QA-SN-0002-EDIT.
  - Второй раз: ревизия #5 записи QA-SN-RO-2 от Roman, «'Serial Number' changed».
- Для сравнения: редакторы Customers, Software и Orders под ReadOnly корректно становятся только для чтения ([69](screens/69-roman-ro-account-editor.png)).
- Скриншоты: [24](screens/24-roman-hw-editor.png), [25](screens/25-roman-hw-editor-serial-typed.png), [101](screens/101-su-hw-revision-dialog.png).
- QML: `ProLife/Qml/Include/prolifeqml/DeviceEditor.qml`.

**D-03. Потеря обновлений: нет контроля конкурентного редактирования.** Повтор ×2.
- Страница: Hardware → редактор одной и той же записи.
- Аккаунт: su и Roman одновременно.
- Шаги:
  1. su и Roman открывают одну запись.
  2. Roman меняет Description и сохраняет.
  3. su (его вкладка не обновилась) меняет Serial и сохраняет.
- Ожидание: предупреждение о конфликте или подгрузка свежей версии.
- Факт:
  - Save у su молча откатывает Description Roman.
  - Повтор в обратную сторону: сохранение из устаревшей вкладки Roman откатывает Serial, который сохранил su.
  - История ревизий подтверждает: #3 → #4 → #5.
  - Открытая вкладка второго пользователя не получает ни обновления, ни баннера.
- Скриншоты: [86](screens/86-su-hw-after-roman-save.png), [87](screens/87-su-hw-save-over.png), [89](screens/89-su-final-after-stale-save.png), [101](screens/101-su-hw-revision-dialog.png).

### Major

**D-04. Создание Software в обход права Add License через Orders → Add Product → Create new.** ×1.
- Аккаунт: Roman, роль *QA_Roman Partial* (Orders полностью, Licenses только просмотр).
- Факт: в заказ добавлен новый Software QA_SW_ROMAN, и запись появилась в разделе Software ([172](screens/172-roman-partial-add-new-sw.png), [174](screens/174-su-software-qa-search.png)).
- Ожидание: режим «Create new» недоступен без AddLicense / AddSensor.

**D-05. Date picker: ввод даты с клавиатуры игнорируется.** Повтор ×2.
- Страница: Software → Expiration (Unlimited выключен).
- Факт:
  - Набранная дата (невалидная 31.02.2020 или валидная 15.03.2028) не помечает документ изменённым.
  - После Save в поле остаётся набранный текст, а на сервере старое значение: при повторном открытии 08.10.2027.
  - Выбор через календарь работает (15.10.2027 сохранилось).
  - Невалидная дата никак не подсвечивается.
- Скриншоты: [115](screens/115-su-sw-date-invalid.png), [117](screens/117-su-sw-reopen-expiration.png), [118](screens/118-su-sw-valid-typed-date-reopen.png).
- QML: `ProLife/Qml/Include/prolifeqml/SoftwareEditor.qml` (ExpirationDatePicker).

**D-06. Невалидный MAC-адрес сохраняется.** ×1.
- Hardware → New: маска расставляет двоеточия, но пропускает любые символы. Красная рамка ничего не блокирует.
- Сохранено значение `zz:no:t-:a-:ma:c`.
- Скриншоты: [35](screens/35-roman-new-hw-state.png), [36](screens/36-roman-new-hw-save-invalid-mac.png).
- QML: `DeviceEditor.qml` (MacAddressInput).

**D-07. Orders: Save при невалидном Delivery-ID молча снимает признак изменений.** ×1.
- После Save пропадает «*», кнопка Save становится неактивной, сообщения об ошибке нет. Документ при этом **не** сохранён.
- Скриншот: [123](screens/123-su-order-save-invalid-delivery.png).
- QML: `ProLife/Qml/Include/prolifeqml/OrderEditor.qml`.

**D-08. Support: тикет сохраняется без заголовка, а Type меняется после сохранения.** ×1.
- Создан тикет #7 без Title.
- В форме было выбрано Type = «Support Request», после сохранения стало «Access Request».
- Вкладка после переименования осталась «<no name>».
- Скриншоты: [72](screens/72-roman-ticket-new.png), [73](screens/73-roman-ticket-save-empty.png), [76](screens/76-roman-ticket-title-enter.png).
- QML: `ImtCore/Qml/imtdeskgui/TicketEditor.qml`.

**D-09. Dashboard: счётчики противоречат данным.** Повтор ×2.
- Фильтр «This Year (2026)»: карточка Hardware показывает 0.
- При этом в 2026 году создано 5 записей: Analytics → «RTV Hardware (5)», и круговая диаграмма Hardware Status на том же дашборде их учитывает.
- Скриншоты: [01](screens/01-su-landing.png), [91](screens/91-su-dashboard-timefilter.png), [94](screens/94-su-analytics.png).

**D-10. User Actions не записывает сегодняшние действия, итог «застрял» на 20.** Повтор ×2 (до и после перезагрузки).
- За 08.10 нет ни одной записи, хотя создавались роли, Hardware, Software, Orders, Customers и тикеты.
- Итог «(1-20/20)» не меняется при размере страницы 50.
- Скриншот: [93](screens/93-su-useractions.png).

**D-11. Попап «+ Add Role» в редакторе пользователя выходит за край окна, видны только 2 роли из 20.** Повтор ×2.
- Скриншоты: [52](screens/52-su-user-add-role-picker.png), [53](screens/53-su-user-add-role-search.png).
- QML: `ImtCore/Qml/imtcolgui/ItemSelectElementView.qml` или `ImtCore/Qml/imtauthgui/UserView.qml`.

**D-12. Фильтр Customers показывается пользователю без ViewAccounts, внутри — сырая ошибка сервера.** ×1.
- Hardware → фильтр Customers: в выпадающем списке красный текст «Invalid permissions for the user 'Roman Kucherenko'».
- Ссылки в колонке Customer кликабельны, но ничего не происходят; в консоли `No NavigableItem found for: Accounts`.
- Скриншоты: [21](screens/21-roman-customers-filter.png), [22](screens/22-roman-click-customer-link.png).
- QML: `ProLife/Qml/Include/prolifeqml/DeviceCollectionView.qml` (CustomersFilter).

**D-13. Customers: email не проверяется.** ×1.
- Клиент сохранён с email `not-an-email`.
- Скриншот: [142](screens/142-su-customer-invalid-email.png).
- QML: `ProLife/Qml/Include/prolifeqml/AccountEditor.qml`.

### Minor

| ID | Где | Факт | Повтор | Скриншот |
|---|---|---|---|---|
| D-14 | Role → Permissions | После сброса фильтра родитель показывает полную галочку, хотя отмечена часть дочерних прав. Клик по такому родителю **снимает** все права раздела: так у роли Partial пропали все права Orders. | ×2 | [45](screens/45-su-perm-after-clear-filter.png), [48](screens/48-su-role-partial-perms.png) |
| D-15 | Hardware | Create License File и Transfer Licenses активны при Licenses (0), нажатие даёт диалог ошибки. Bind активен на несохранённой новой записи и выдаёт только тост «Please save the document first». | ×1 | [106](screens/106-su-hw-CreateLicenseFile.png), [31](screens/31-roman-new-hw-bind-unsaved.png) |
| D-16 | Revisions | На одно создание приходится две ревизии «Created the document». В истории пользователя Roman много ревизий без пользователя и без описания. | ×2 | [101](screens/101-su-hw-revision-dialog.png), [15](screens/15-su-user-editor-History.png) |
| D-17 | Order → Products | Сразу после добавления Expiration = 15.10.2027, после повторного открытия = 2027-10-15, а из Article пропадает «(12.10197)». | ×1 | [133](screens/133-su-order-saved-with-product.png), [171](screens/171-roman-partial-order-products.png) |
| D-18 | New Role | Ошибка «Role-ID can't be empty!» относится к полю только для чтения. Не подсвечено поле, которое нужно заполнить (Role Name). | ×1 | [41](screens/41-su-new-role-save-empty.png) |
| D-19 | Отказ сервера | При отказе в правах показываются два тоста сразу: «Internal error» и «Invalid permissions…». | ×1 | [184](screens/184-roman-yes-save-revoked.png) |
| D-20 | Customer → Groups | У Roman с правом Change Account Groups таблица групп молча пустая: GroupsList требует административное право ViewGroups. | ×1 | [181](screens/181-roman-full-account-groups.png) |
| D-21 | Organizations | Пагинация показывает «(1-0/0)» при пустом списке. | ×1 | [70](screens/70-roman-ro-Tenants.png) |
| D-22 | User Actions | Заголовок колонки «Zeit» (немецкий) в английском UI. Grep по QML/ACC источник не нашёл; возможно, это серверные HeaderNames или перевод. | ×2 | [93](screens/93-su-useractions.png) |
| D-23 | Загрузка | 404 на шрифты `ProLife/Views/Fonts/Ubuntu-Light.ttf` и `Ubuntu-Medium.ttf` при каждом старте. | ×3 | — |
| D-24 | Консоль | `Connections: signal 'hoveredChanged' not found` (Support), `Unable to create param from json. Param: ServerLog` (Preferences), warning `GetTableViewParams` с пустым message на каждой коллекции. | ×2 | — |
| D-25 | Customer edit | Один Save оставил документ изменённым (известное поведение «Save в два клика»). | ×1 | — |

---

## 3. Матрица прав (Roman)

Профили:
- **Test** — исходная роль.
- **RO** — QA_Roman ReadOnly: все View*, кроме Administration.
- **Partial** — RO плюс Order Management полностью.
- **Full** — всё, кроме Administration.

Проверка всегда после чистого логина.

| Действие | Test | RO | Partial | Full |
|---|---|---|---|---|
| Меню | Hardware, Search, Support | все разделы, кроме Admin | все разделы, кроме Admin | все разделы, кроме Admin |
| Hardware: New / Bind / Create License File / Transfer | видны | **видны, New работает (D-01)** | видны | видны |
| Hardware: Remove | скрыт | скрыт | скрыт | виден |
| Hardware: редактирование полей без права | **можно, сохраняется (D-02)** | **можно, сохраняется** | — | — |
| Software: New/Remove; редактор | — | скрыты; только чтение ✔ | скрыты; только чтение ✔ | доступны |
| Software через Order → Add Product → Create new | — | — | **создаётся (D-04)** | — |
| Orders: New/Edit/Remove | — | скрыты; только чтение ✔ | доступны, сохранение ✔ | доступны |
| Customers: Edit / Restore | — | Edit виден, но редактор только для чтения ✔; Restore виден | то же | доступны |
| Customer → Groups | — | — | — | пусто без пояснения (D-20) |
| Фильтр Customers на Hardware | ошибка в списке (D-12) | работает | работает | работает |
| Organizations | — | 0 записей, видны Switch/Leave | то же | New доступен, 0 записей |
| Support: New/Edit/Remove | доступны | доступны (своих прав у тикетов нет) | доступны | доступны |
| Workspace | скрыт | Dashboard/User Actions/Analytics | то же | то же |
| Прямой переход на закрытую страницу | ссылка Customer → ничего, ошибка в консоли | — | — | — |
| Смена роли при открытой сессии | — | UI не обновляется до перезагрузки; устаревшая кнопка New открывает форму без Save. Сервер отклоняет OrderAdd ✔ | — | — |

---

## 4. Предложения по UX

Все предложения реализуемы на контролах проекта и Style-токенах, без Quick Controls.

| # | Проблема | Предложение | Эффект | Сложность |
|---|---|---|---|---|
| U-01 | Четыре разных раскладки редакторов: Hardware/Software/Orders (метка слева, поле справа), Ticket (метки над полями, без навигации), Organization (хлебные крошки вместо вкладок), Profile. Разный порядок кнопок: Undo/Redo/Save в Ticket, Save/Undo/Redo в остальных. | Единый шаблон редактора и единый порядок команд. | Предсказуемость | M |
| U-02 | Терминология «плавает»: Hardware / Sensor / Device («Removing the sensor»), Customers / Account Name, Organizations / Tenant Name, Support / Tickets, Admin / Administration, «Design Schema». | Глоссарий и единые подписи в меню, заголовках, диалогах и правах. | Понятность | S |
| U-03 | Ячейки-ссылки (Customer, Delivery-ID, Hardware-ID) обрезаются без многоточия; заголовки «H…», «Deli…», статусы «Cancel…». | Многоточие и tooltip; минимальная ширина колонок; Fit to width по умолчанию. | Читаемость | S |
| U-04 | Статус показывается по-разному: иконка+текст в Hardware, только иконка в Software, только текст в Orders, чип в Support. | Один компонент статуса (иконка+текст). | Консистентность | S |
| U-05 | Действия, которые заведомо не сработают, активны: Create License File / Transfer без лицензий, Bind на несохранённом документе. | Выключать кнопку и показывать tooltip с причиной. | Меньше «ошибок-сюрпризов» | S |
| U-06 | Красные сообщения валидации видны сразу на пустой форме; MAC подсвечивается рамкой без текста; Save не блокируется. | Показывать ошибку после blur или первого Save; текст под полем; блокировать Save и подсвечивать первое невалидное поле. | Меньше мусорных записей | M |
| U-07 | Подтверждения «Deleting a selected element», «Save all changes ?» с кнопками Yes/No. | Называть объект («Удалить Hardware QA-SN-…?»), кнопки Save / Discard / Cancel. | Меньше ошибочных удалений | S |
| U-08 | После изменения прав открытая сессия не обновляется. | Push-событие об изменении прав или баннер «Права изменены — обновите страницу». | Нет «призрачных» кнопок | M |
| U-09 | Открытый документ не знает, что его сохранил другой пользователь. | Баннер «Документ изменён пользователем X» с действиями Обновить / Сравнить; проверка версии при Save (вместе с D-03). | Нет потери данных | L |
| U-10 | Дерево прав: 110 плоских строк с путями через «/»; одинаковые названия (Change Order, Change Description) в боковой панели роли без указания раздела. | Настоящая иерархия (Edit Sensor → Change Sensor → …); в панели группировка по разделам. | Меньше ошибок при настройке ролей | M |
| U-11 | Боковая панель «Please select an item for showing additional informations» постоянно занимает 200 px. | Сворачивать, пока ничего не выбрано; исправить грамматику. | +200 px таблице | S |
| U-12 | Search до ввода показывает «No results»; результаты Hardware неразличимы (у всех «RTV Hardware»); тикеты и организации не ищутся. | Пустое состояние «Введите запрос»; вторая строка с Serial / MAC / клиентом; подсветка совпадения; добавить Tickets и Organizations. | Поиск становится полезным | M |
| U-13 | Чип «··· 2» выглядит как счётчик активных фильтров. | Подпись «+2 фильтра» или «Ещё». | Понятность | S |
| U-14 | Пустые состояния предлагают действие тем, кому оно недоступно («Items added to this collection will show up here»). | Текст пустого состояния в зависимости от прав. | Корректные ожидания | S |
| U-15 | Quantity > 1 в Add Product → Create new при единственном поле Software-ID. | Либо N полей ID, либо явная подсказка про автонумерацию. | Понятность | S |
| U-16 | Пресеты дат без «All time» и без произвольного диапазона; фильтры Dashboard и Analytics независимы и подписаны по-разному. | Добавить All time и Custom; общий фильтр периода для Workspace. | Аналитика удобнее | M |
| U-17 | Диалог Revisions: длинные описания изменений обрезаются. | Перенос строк или раскрытие по клику. | Читаемость аудита | S |
| U-18 | Вкладка документа не обновляет заголовок после переименования. В профиле и в тикете смешаны autosave («Saved») и явный Save. | Единая модель сохранения; заголовок вкладки из модели. | Консистентность | M |

---

## 5. Что не удалось проверить

Пункты, которые в первом проходе были пропущены (лицензии, ревизии, организации, пользователи и группы, пароль и тема, узкие экраны), проверены после разрешения; результаты в разделе 7.

Осталось непроверенным:
- **Split / Revoke.** Команды есть только на сервере (`SoftwareProductsPage.acc`: SplitLicense, RevokeLicense, ChildLicensesList, ParentChainList). В QML их никто не вызывает, кнопок в GUI нет; в `LicenseTreeCanvas.qml` отрисовка revoke-рёбер закомментирована. Проверить через GUI невозможно.
- **Revision Backup / Export:** в GUI соответствующих кнопок не найдено.
- **Organizations:** Grants, Connect Organization, Relationships — открыты, но сценарии не прогонялись.
- **Restore удалённого клиента**, Access Tokens, смена языка.
- **Клавиатурная навигация** — только частично. Esc закрывает диалоги ✔. Клик в текстовое поле выделяет весь текст, и ввод заменяет значение (наблюдалось один раз).
- **Режимы редактирования графика** License Creation Activity (Edit / Add / Delete points): кнопки есть в DOM, видимый вход не найден.

## 6. Уборка

Удалено через GUI:
- Hardware: QA-SN-0002-EDIT, QA-SN-RO-1, QA-SN-RO-2.
- Software: QA_SW_1, QA_SW_ROMAN.
- Order: 99999.
- Customer: QA_Customer_1.
- Organization: QA_Org_1.
- Ticket: #7 QA_Ticket_Roman_1.
- Роли: QA_Roman ReadOnly / Partial / Full.

Roman возвращена роль Test (группа QUISS не менялась).

Что осталось:
- Комментарий Roman и смена Priority у тикета #7 удалены вместе с тикетом.
- Изменения раскладки таблиц su (сортировка Hardware по Serial Number) хранятся на сервере для пользователя su.

Уборка после второго прохода (раздел 7):
- Удалены: Hardware QA-REV-1; пользователи qa_user1 («QA Dup»), qa_user2, qa_selfreg2; группа QA_Group_1; организация QA_Org_2 (вместе с ролью QA_OrgRole_Sensors).
- Лицензия Import_19582_2 отвязана и снова привязана к 70:b3:d5:aa:b3:51: итоговое состояние совпадает с исходным, но в истории ревизий записи остались.
- Тема su возвращена на Light, контекст su — No organization.
- Roman вышел из QA_Org_2, его роль Test и группа QUISS не менялись.
- В БД после проверки Set / Delete revision остались ревизии на QA-записях (удалены вместе с ними).

## 7. Дополнительная проверка (второй проход)

Разрешённые сценарии: лицензии, ревизии, организации, пользователи и группы, пароль и тема, узкие экраны, плюс самостоятельная регистрация.

### 7.1 Новые дефекты

| ID | Серьёзность | Где | Факт | Повтор | Скриншоты |
|---|---|---|---|---|---|
| D-26 | **Critical** | Users / Groups | Роль группы копируется пользователю как **прямая** роль. После выхода из группы роль и её права остаются (qa_user1: вышел из QA_Group_1, в Assigned Roles осталась «Sensor Production User», права действуют после перелогина). Вероятная первопричина D-01. | ×2 (qa_user1, qa_user2) | [414](screens/414-qa-profile-perms-after-group-removed.png), [420](screens/420-su-qauser2-roles.png) |
| D-27 | Major | Hardware → Transfer Licenses | Перенос невозможен: после выбора устройства кнопка Transfer остаётся неактивной. Пикер для RTV-устройства предлагает только RTV.3d Hardware. При открытии сервер возвращает `Object collection iterator creation failed` на DevicesList; в консоли `View already registered`. | ×2 | [315](screens/315-su-transfer-licenses.png), [319](screens/319-su-transfer2-selected.png) |
| D-28 | Major | Пикер Transfer | Внутри пикера — полная командная панель Hardware (New / Edit / **Remove** / Bind / Create License File / Transfer). Remove активен для выбранной строки. New открывает документ в основном окне за модальным диалогом. Однажды диалог залип: две коллекции наложились (дубли заголовков и пагинации), не закрывался ни Close, ни ✕, ни Esc — помогла только перезагрузка. | ×2 / залипание ×1 [не воспроизводится стабильно] | [316](screens/316-su-transfer-result.png), [332](screens/332-su-after-close2.png), [335](screens/335-su-transfer-new-inside.png) |
| D-29 | Major | Revisions | Set this revision и Delete revision выполняются **без подтверждения**; удаление ревизии необратимо. Для Current-ревизии кнопки корректно неактивны. | ×2 | [344](screens/344-su-rev-after-set.png), [348](screens/348-su-rev-delete-current.png) |
| D-30 | Major | Revisions | Правки ещё не переоткрытой новой записи пишутся как «Created the document» (3 ревизии подряд); дифф изменений теряется. | ×2 (QA-SN-RO-2, QA-REV-1) | [341](screens/341-su-rev1-selected.png) |
| D-31 | Major | Admin → New User | Пользователь сохраняется с паролем `1` и без email, хотя форма показывает «at least 8 characters» и «Please enter the email». Вход с паролем `1` работает. Admin → Change password тот же короткий пароль блокирует (непоследовательно). | ×1 | [403](screens/403-su-new-user-shortpwd.png), [433](screens/433-su-admin-pwd-short.png) |
| D-32 | Major | Sign up | При слабом пароле или невалидном email диалог молча закрывается; сервер: `Internal error. Unable to create response for command-ID: 'RegisterUser'`. Пользователь не создан и не получает сообщения. С валидными данными регистрация работает. | ×1 | [461](screens/461-guest-signup-filled.png), [462](screens/462-guest-signup-result.png) |
| D-33 | Major | Users / Groups picker | Только что созданная группа не находится поиском в пикере (до и после перезагрузки), хотя есть в полном списке. Существующие группы ищутся. | ×2 | [416a](screens/416a-su-group-picker.png), [419](screens/419-su-group-picker-search.png) |
| D-34 | Major | Тёмная тема | После перезагрузки в Dark иконки статусов в таблицах тёмные на тёмном фоне и почти не видны (до перезагрузки были светлыми). Иконка пользователя в шапке — белый кружок. Read-only поля визуально не отличаются от редактируемых. | ×1 | [442](screens/442-su-dark-Devices.png), [444](screens/444-su-dark-after-reload.png), [443](screens/443-su-dark-editor.png) |
| D-35 | Major | Узкие экраны | 1024 и 768 px: горизонтальная прокрутка страницы (scrollWidth 1046 и 790), колонки сжимаются до 2–3 символов, у таблицы нет своего горизонтального скролла. 390 px: дашборд остаётся в 3 колонки, User Actions наезжает на фильтр Customers, глобальный поиск исчезает, клик по иконке Hardware в свёрнутом меню не переключает страницу. | ×1 на каждую ширину | [450-1024](screens/450-su-1024-hardware.png), [450-768](screens/450-su-768-hardware.png), [451-390](screens/451-su-390-workspace.png) |
| D-36 | Minor | Самостоятельная регистрация | Зарегистрированный пользователь сразу активен (созданный админом — выключен по умолчанию, без подсказки). Он видит Hardware и New Hardware, но таблица пустая без колонок: GetCollectionHeaders отказывает в правах. | ×1 | [404](screens/404-qa-user1-login.png), [465](screens/465-qa-selfreg2-landing.png) |
| D-37 | Minor | Hardware → Bind | После Save в диалоге Bind вкладка Licenses открытого редактора не обновляется. Сохранение отвязки спрашивает «Please enter the project» без объяснения, зачем. При каждом открытии в консоли `Unable to update elements 'collectionId' is empty!`. Сами Bind и Unbind работают. | ×2 | [302](screens/302-su-hw-editor-licenses.png), [305](screens/305-su-after-unbind-save.png), [309](screens/309-su-bind-confirmed.png) |
| D-38 | Minor | Create License File | Выбор Encrypted / Unencrypted — меню без заголовка в центре экрана, а не у кнопки. Оба варианта скачиваются под одинаковым именем `<mac>_License.lic`. Для не-Finished устройства сообщение понятное ✔. | ×2 | [311](screens/311-su-create-license-file.png), [312](screens/312-su-create-license-file-finished.png) |
| D-39 | Minor | Тосты | Не исчезают сами: висят минутами, копятся и закрывают кнопки диалогов (Close в Revisions). Опечатка «successfully delete». | ×3 | [348](screens/348-su-rev-delete-current.png), [356](screens/356-su-create-invitation.png) |
| D-40 | Minor | Organizations | Подробности ниже, в 7.2. | — | [350](screens/350-su-org2-saved.png), [364](screens/364-roman-switched-org.png), [373](screens/373-su-org-role-perms.png) |
| D-41 | Risk | Organization → Members → Edit | Владелец организации открывает **глобальную** учётку участника: Username, Email, Account enabled, Change password. Проверено только под su; для обычного владельца организации нужно отдельно убедиться, что это запрещено. | ×1 | [387](screens/387-su-member-edit.png) |
| D-42 | Minor | Users | Поиск в списке пользователей не ищет по логину (Username): по `qa_user1` пусто, по Name и email находит. Пикер приглашений в организацию выходит за правый край (как D-11). | ×2 | [356](screens/356-su-create-invitation.png) |

Подробности к D-40 (Organizations):
- Edit в списке активен, но отказывает тостом «first switch to that organization».
- Приглашение уходит сразу по клику на чекбокс в пикере, Leave выполняется без подтверждения.
- Покинуть текущую организацию можно только после переключения на другую.
- Удалённая организация остаётся в меню пользователя до перезагрузки.
- Выбор организации не запоминается между входами.
- В роли организации список прав пуст, пока у организации не включены функции, и не обновляется без переоткрытия роли.
- После перехода в организацию без ролей пользователь попадает на пустой Search без пояснения.
- Для «No organization» показан текст «You are currently working in this organization».

### 7.2 Что работает корректно

- **Bind / Unbind** на реальных лицензиях: изменения сохраняются, в списке Hardware и Software видны сразу; бейдж «New» у добавляемой лицензии.
- **Create License File** для Finished-устройства скачивает `.lic`; для не-Finished — понятное сообщение.
- **Set this revision** откатывает данные (Description = QA_rev_v1).
- **Организации:**
  - изоляция данных: в QA_Org_2 списки Hardware, Orders и Customers пусты;
  - приглашение приходит в реальном времени (точка у Profile, Accept / Reject), статус участника у владельца обновляется live;
  - внутри организации сервер не дал создать Hardware без роли организации (в редакторе нет Save).
- **Права через группу** выдаются (qa_user1 получил Hardware после вступления в группу).
- **Смена пароля в профиле:** короткий пароль и несовпадение блокируют кнопку, неверный текущий пароль даёт понятную ошибку, после смены старый пароль не работает, новый работает.
- **Dark-тема** сохраняется после перезагрузки (кроме D-34).
- **Страница входа на 390 px** выглядит прилично.

### 7.3 Дополнения к UX-предложениям

| # | Проблема | Предложение | Сложность |
|---|---|---|---|
| U-19 | Пикеры (Transfer, Add Product) переиспользуют полные коллекции вместе с командной панелью. | Режим `pickerMode`: только поиск, фильтры и выбор; без команд и боковых редакторов. | M |
| U-20 | Необратимые операции без подтверждения: Delete / Set revision, Leave, приглашение одним кликом. | Подтверждение с названием объекта; для приглашений — явная кнопка «Пригласить N пользователей». | S |
| U-21 | Трёхуровневая навигация (меню → вкладки организации → вкладки роли или участника) на одном экране. | Открывать роль и участника отдельной вкладкой документа или диалогом; хлебные крошки оставить одни. | M |
| U-22 | Права в организации настраиваются в 3 шага без подсказок (функции организации → роль → участник). | Мастер или подсказка в пустом состоянии: «Сначала включите функции во вкладке Permissions». | S |
| U-23 | Тосты без автоскрытия. | Автоскрытие через 5–8 с для success и info; ошибки оставлять до закрытия; ограничить стек до 3. | S |
| U-24 | Нет адаптивной раскладки. | Горизонтальный скролл у таблицы вместо страницы; дашборд в 1 колонку при ширине < 900 px; командная панель полностью уходит в «…». | L |
| U-25 | Пользователь, созданный админом, по умолчанию выключен без подсказки. | Включать по умолчанию или явно подсвечивать «Account disabled — user can't log in» перед сохранением. | S |
