// "Modified from another computer" banner - GENERIC coverage (RemoteCollectionView.qml).
//
// Every collection view subscribes to server-pushed change notifications for its own collection; if
// the change came from a DIFFERENT session than the one currently looking at the table, an alert
// banner appears ("This table has been modified from another computer" + an "Update" button) instead
// of silently refreshing out from under the user. This is exactly the kind of same-user,
// multiple-simultaneous-session interaction this suite otherwise goes out of its way to AVOID (see
// fixtures/users.js's isolatedSpec comments) - here it's the deliberate subject of the test, not an
// accident, so it opens two sessions of ONE dedicated user on purpose instead of two different spec
// files colliding by accident.
//
// "Update"'s objectName is auto-derived from its own `text` ("Update" -> "UpdateButton", the same
// Button-derives-from-text convention as OKButton/CancelButton/YesButton/NoButton elsewhere in this
// suite - see GuiTestKit/CONTRIBUTING-testability.md) - no new instrumentation needed.

const { test, newUserPage } = require('../fixtures/test');
const { DeviceCollectionPage, DeviceEditorPage } = require('../pages');
const { canSeePage, canRunDeviceCommand } = require('../matrix/permissions');
const gui = require('imtcore-gui-testkit/lib/gui');

const PAGE = 'Devices';

test.describe('Concurrent session - remote change banner', () => {
  test('editing a device in one session shows the banner in another', async ({ browser }, testInfo) => {
    const sessionA = await newUserPage(browser, testInfo);
    const sessionB = await newUserPage(browser, testInfo);
    try {
      test.skip(!canSeePage(sessionA.user, PAGE), 'user cannot see Hardware');
      test.skip(!canRunDeviceCommand(sessionA.user, 'Edit'), 'user cannot edit a sensor');

      const devicesA = new DeviceCollectionPage(sessionA.page);
      await devicesA.reload();
      await devicesA.open();
      await gui.checkScreenshot(sessionA.page, 'concurrent-banner-before');

      // Session B edits and saves an existing device WITHOUT session A doing anything - the banner in
      // A must come from the server's own change-subscription push, not from any action A takes.
      const devicesB = new DeviceCollectionPage(sessionB.page);
      await devicesB.reload();
      await devicesB.open();
      await devicesB.selectRow(0);
      await devicesB.editItem();
      const editorB = new DeviceEditorPage(sessionB.page);
      await editorB.setProject(`Concurrent edit ${Date.now()}`);
      await editorB.save();

      await gui.expectVisible(
        sessionA.page,
        ['UpdateButton'],
        'the OTHER session should see the "modified from another computer" banner'
      );
      await gui.checkScreenshot(sessionA.page, 'concurrent-banner-shown');

      await gui.clickButton(sessionA.page, ['UpdateButton']);
      await gui.expectHidden(sessionA.page, ['UpdateButton'], 'banner should clear after clicking Update');
    } finally {
      await sessionA.context.close();
      await sessionB.context.close();
    }
  });
});
