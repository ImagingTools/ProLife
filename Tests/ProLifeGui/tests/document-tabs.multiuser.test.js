// Multi-document tab workspace - GENERIC coverage (MultiDocumentCollectionView.qml/TabDelegate.qml).
//
// Every collection page gets this same tab mechanism for free; Devices is used here purely as a
// convenient, already fully-instrumented vehicle - none of what's under test is Devices-specific
// business logic (that's covered separately by devices.editor.multiuser.test.js). Pinned to its own
// isolated user (tabsEditor - see fixtures/users.js) for the same reason every document-editor spec is:
// the open-documents workspace is keyed server-side by userId only, so two spec files opening tabs as
// the same user concurrently would corrupt each other's workspace.
//
// TabDelegate.qml's objectName is "Tab" + model.index - 0 is always the pinned collection tab, 1+ are
// document tabs in the order they were opened (see BasePage.switchToTab).

const { test, newUserPage } = require('../fixtures/test');
const { DeviceCollectionPage, DeviceEditorPage } = require('../pages');
const { canRunDeviceCommand } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

test.describe('Document tabs', () => {
  test.describe.serial('multiple open documents', () => {
    let page, user, devices;

    test.beforeAll(async ({ browser }, testInfo) => {
      ({ page, user } = await newUserPage(browser, testInfo));
      devices = new DeviceCollectionPage(page);
      await devices.reload();
      await devices.open();
    });

    test.afterAll(async () => {
      if (page) await page.close();
    });

    test.beforeEach(() => {
      test.skip(!canRunDeviceCommand(user, 'New'), 'user cannot create a sensor (AddSensor)');
    });

    test('two documents keep independent state across tab switches', async () => {
      // Tab A: opened first, becomes "Tab1" (Tab0 is the pinned collection tab). A freshly-opened
      // document tab needs a beat to finish its own render before its fields are interactive
      // (confirmed live: filling immediately after newItem() can hit "exists but not yet visible") -
      // every OTHER "new document" suite in this repo takes an "empty new editor" screenshot as its
      // OWN first step for the same reason; do the same here before touching any field.
      await devices.newItem();
      const editorA = new DeviceEditorPage(page);
      await gui.checkScreenshot(page, 'document-tabs-a-empty');
      await editorA.setProject('Tab A value');
      await gui.checkScreenshot(page, 'document-tabs-a-filled');

      // Back to the collection tab to open a SECOND new document - the New command lives on the
      // collection tab's own command bar, not a document tab's.
      await devices.switchToTab(0);
      await devices.newItem();
      const editorB = new DeviceEditorPage(page);
      await gui.checkScreenshot(page, 'document-tabs-b-empty');
      await editorB.setProject('Tab B value');
      await gui.checkScreenshot(page, 'document-tabs-b-filled');

      // Switch back to Tab A - its field must still show what was typed there, proving tab switching
      // doesn't share/overwrite state between concurrently open documents.
      await devices.switchToTab(1);
      await editorA.project.waitForValue('Tab A value');
      await gui.checkScreenshot(page, 'document-tabs-a-still-correct-after-switch');
    });

    test('closing one document tab leaves its sibling open', async () => {
      // Continuing from the previous test - Tab A (index 1) is active. CloseButton is matched
      // unqualified ([objectName="CloseButton"][visible].first()) - every open document tab shows its
      // own X, so this closes whichever one DOM order puts first, not necessarily the active one - see
      // BasePage.closeDocument(). A dirty document raises a "Save all changes?" confirm (discard via
      // No); confirmed live this doesn't always fire here (closeAllDocumentTabs' own comment already
      // notes the pinned collection tab has no CloseButton at all, and dirty-tracking is otherwise
      // opaque from the test side) - handle both outcomes rather than assuming one.
      const editorA = new DeviceEditorPage(page);
      await editorA.closeDocument();
      if ((await gui.countVisible(page, ['Dialog'])) > 0) {
        await gui.clickButton(page, ['NoButton']);
      }

      // One document tab remains (whichever of A/B DOM order didn't just close) - its own field must
      // be unaffected by the other's edits or closure. Don't assume which index it landed at; just
      // confirm the surviving tab still shows one of the two known values (whichever wasn't closed),
      // proving the close didn't corrupt or blank the sibling out.
      await devices.switchToTab(1);
      const remaining = new DeviceEditorPage(page);
      await gui.waitForTextInputValue(page, ['ProjectInput'], (v) => v === 'Tab A value' || v === 'Tab B value');
      await gui.checkScreenshot(page, 'document-tabs-b-survives-sibling-close');

      // Clean up: discard this document too so the shared workspace doesn't leak a dirty tab into
      // whatever runs next under this user.
      await remaining.closeDocument();
      if ((await gui.countVisible(page, ['Dialog'])) > 0) {
        await gui.clickButton(page, ['NoButton']);
      }
    });
  });
});
