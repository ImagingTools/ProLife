// Hardware (Devices) - "Create license file", superuser path.
//
// The Encrypt/Unencrypt popup appears only for the superuser (loggedUserIsSuperuser() in
// onCreateLicenseFile()); every other user's CreateLicenseFile either succeeds silently or shows the
// validation error covered in devices.collection.test.js. Every spec file runs as its own user (see
// fixtures/users.js), so this flow lives in a file of its own, pinned to `su`. Both choices submit a
// real request, so both are exercised, and both are @mutating.

const { test, newUserPage } = require('../fixtures/test');
const { DeviceCollectionPage } = require('../pages');
const gui = require('imtcore-gui-testkit/lib/gui');

test.describe.serial('Hardware / create license file - encrypt choice (superuser)', () => {
  let page, devices;

  test.beforeAll(async ({ browser }, testInfo) => {
    ({ page } = await newUserPage(browser, testInfo));
    devices = new DeviceCollectionPage(page);
    await devices.reload();
    await devices.open();
  });

  test.afterAll(async () => {
    if (page) await page.context().close();
  });

  test('choose Encrypted', { tag: '@mutating' }, async () => {
    const selected = await devices.selectCompleteLicensedSensor();
    test.skip(!selected, 'no finished licensed sensor has both serial number and MAC address');
    await devices.createLicenseFile();
    await gui.checkScreenshot(page, 'devices-create-license-file-encrypt-popup', await devices.masks());
    await devices.chooseEncrypted();
    await gui.checkScreenshot(page, 'devices-create-license-file-encrypted', await devices.masks());
  });

  test('choose Unencrypted', { tag: '@mutating' }, async () => {
    const selected = await devices.selectCompleteLicensedSensor();
    test.skip(!selected, 'no finished licensed sensor has both serial number and MAC address');
    await devices.createLicenseFile();
    await devices.chooseNotEncrypted();
    await gui.checkScreenshot(page, 'devices-create-license-file-not-encrypted', await devices.masks());
  });
});
