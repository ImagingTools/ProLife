// "Modified from another computer" banner - GENERIC coverage (RemoteCollectionView.qml).
//
// Every collection view subscribes to server-pushed change notifications for its own collection; if the
// change came from SOMEONE ELSE, an alert banner appears ("This table has been modified from another
// computer" + an "Update" button) instead of silently refreshing out from under the user.
//
// "Someone else" means a different USER, not a different session: RemoteCollectionChangeListener.qml
// decides it with `isLocalChange = info.ownerId === root.currentUserId`, so a second session of the SAME
// user is a LOCAL change and gets the silent refresh, never the banner. This spec used to open two
// sessions of its own fixture user and wait for a banner that by construction could never appear.
//
// So: the watching session is this spec's own isolated user, and the change is made by `su` in a second
// context that logs in for itself (every fixture user exists in the restored database whether or not it
// got a Playwright project this run - see global-setup.js). That second session saves a device, which is
// why this is @mutating: it belongs in the serial phase, both because it writes and because a su-driven
// document tab must not run beside another spec's su-driven one.
//
// "Update"'s objectName is auto-derived from its own `text` ("Update" -> "UpdateButton", the same
// Button-derives-from-text convention as OKButton/CancelButton/YesButton/NoButton elsewhere in this
// suite - see GuiTestKit/CONTRIBUTING-testability.md) - no new instrumentation needed.

const { test, newUserPage } = require('../fixtures/test');
const { byKey } = require('../fixtures/users');
const { DeviceCollectionPage, DeviceEditorPage } = require('../pages');
const gui = require('imtcore-gui-testkit/lib/gui');

test.describe('Concurrent session - remote change banner', () => {
  test('another user editing a device shows the banner', { tag: '@mutating' }, async ({ browser }, testInfo) => {
    const watcher = await newUserPage(browser, testInfo);
    const peerUser = byKey('su');
    // Explicitly empty, not just "new": a context opened inside a test inherits the project's own
    // `use.storageState`, so a bare newContext() comes up already signed in as the watching user and
    // never shows a login screen at all.
    const peerContext = await browser.newContext({ storageState: { cookies: [], origins: [] } });
    try {
      const devicesA = new DeviceCollectionPage(watcher.page);
      await devicesA.reload();
      test.skip(!(await devicesA.isAvailable()), 'Hardware is not available to this user');
      await devicesA.open();
      await gui.checkScreenshot(watcher.page, 'concurrent-banner-before');

      // The peer edits and saves an existing device while the watcher does nothing - the banner has to
      // come from the server's own change-subscription push, not from any action the watcher takes.
      const peerPage = await peerContext.newPage();
      await gui.reload(peerPage);
      await gui.login(peerPage, peerUser.login, peerUser.password);
      const devicesB = new DeviceCollectionPage(peerPage);
      await devicesB.open();
      await devicesB.selectRow(0);
      await devicesB.editItem();
      const editorB = new DeviceEditorPage(peerPage);
      await editorB.setProject(`Concurrent edit ${Date.now()}`);
      await editorB.save();

      await gui.expectVisible(
        watcher.page,
        ['UpdateButton'],
        'the watching session should see the "modified from another computer" banner'
      );
      await gui.checkScreenshot(watcher.page, 'concurrent-banner-shown');

      await gui.clickButton(watcher.page, ['UpdateButton']);
      await gui.expectHidden(watcher.page, ['UpdateButton'], 'banner should clear after clicking Update');
    } finally {
      await watcher.context.close();
      await peerContext.close();
    }
  });
});
