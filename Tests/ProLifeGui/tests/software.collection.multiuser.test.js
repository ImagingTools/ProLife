// Software (SoftwareProducts) COLLECTION view - full functional coverage, multi-user.
//
// Runs once per user-project; screenshots auto-separate per user (see playwright.config.js). Users
// who cannot see the Software page skip the interaction body but still record a landing screenshot,
// so the permission difference is captured. Command/field gating comes from matrix/permissions.js.

const { test } = require('../fixtures/test');
const { SoftwareCollectionPage } = require('../pages');
const { canSeePage, canRunSoftwareCommand } = require('../matrix/permissions');

const PAGE = 'SoftwareProducts';

test.describe('Software / collection', () => {
  test.beforeEach(async ({ page }) => {
    await new SoftwareCollectionPage(page).reload();
  });

  // Landing screenshot for every user (documents what each permission level sees).
  test('landing', async ({ page, gui, user }) => {
    if (canSeePage(user, PAGE)) await new SoftwareCollectionPage(page).open();
    await gui.checkScreenshot(page, 'software-landing');
  });

  // Structural gate: which commands each user's command bar exposes.
  test('command bar reflects permissions', async ({ page, user }) => {
    test.skip(!canSeePage(user, PAGE), 'user cannot see Software');
    const software = new SoftwareCollectionPage(page);
    await software.open();
    for (const cmd of ['New', 'Edit', 'Remove', 'Revision', 'Split', 'Revoke']) {
      if (canRunSoftwareCommand(user, cmd)) {
        await software.commands.expectHasCommand(cmd);
      } else {
        await software.commands.expectNoCommand(cmd);
      }
    }
  });

  // Everything below only runs for users who can open the page.
  test.describe('interactions', () => {
    test.beforeEach(async ({ page, user }) => {
      test.skip(!canSeePage(user, PAGE), 'user cannot see Software');
      await new SoftwareCollectionPage(page).open();
    });

    // --- filters (each registered filter + built-ins) ------------------------------------------
    test('filter - text search', async ({ page, gui }) => {
      await new SoftwareCollectionPage(page).search('test');
      await gui.checkScreenshot(page, 'software-filter-text');
    });

    test('filter - license status', async ({ page, gui }) => {
      await new SoftwareCollectionPage(page).selectFilterOption('licenseStatus', 'Show only paired licenses');
      await gui.checkScreenshot(page, 'software-filter-license-status-paired');
    });

    test('filter - usage (internal use)', async ({ page, gui }) => {
      await new SoftwareCollectionPage(page).selectFilterOption('usage', 'For Internal Purposes');
      await gui.checkScreenshot(page, 'software-filter-usage-internal');
    });

    test('filter - customers', async ({ page, gui, user }) => {
      test.skip(!user.can('ViewAccounts'), 'customers filter needs ViewAccounts');
      await new SoftwareCollectionPage(page).selectFilterOption('customers', 'QUISS');
      await gui.checkScreenshot(page, 'software-filter-customer-quiss');
    });

    test('filter - creation date preset', async ({ page, gui }) => {
      await new SoftwareCollectionPage(page).setCreationDate('Year_Last');
      await gui.checkScreenshot(page, 'software-filter-creation-date');
    });

    test('filter - license creation date preset', async ({ page, gui }) => {
      await new SoftwareCollectionPage(page).setLicenseCreationDate('Month_Current');
      await gui.checkScreenshot(page, 'software-filter-license-creation-date');
    });

    test('filter - clear all', async ({ page, gui }) => {
      const software = new SoftwareCollectionPage(page);
      await software.search('test');
      await software.selectFilterOption('licenseStatus', 'Show only paired licenses');
      await software.clearAllFilters();
      await gui.checkScreenshot(page, 'software-filter-cleared');
    });

    // --- sorting (columns are addressable by header id) ----------------------------------------
    test('sort by name column', async ({ page, gui }) => {
      const software = new SoftwareCollectionPage(page);
      await software.table.sortBy('name');
      await gui.checkScreenshot(page, 'software-sort-name-1');
      await software.table.sortBy('name');
      await gui.checkScreenshot(page, 'software-sort-name-2');
    });

    test('sort by serial number column', async ({ page, gui }) => {
      await new SoftwareCollectionPage(page).table.sortBy('serialNumber');
      await gui.checkScreenshot(page, 'software-sort-serial');
    });

    // --- pagination ----------------------------------------------------------------------------
    test('pagination - page size and navigation', async ({ page, gui }) => {
      const software = new SoftwareCollectionPage(page);
      await software.pagination.setPageSize(50);
      await gui.checkScreenshot(page, 'software-pagination-50');
      await software.pagination.goToPage(2);
      await gui.checkScreenshot(page, 'software-pagination-page-2');
    });

    // --- row selection + context-sensitive commands --------------------------------------------
    test('select first row', async ({ page, gui }) => {
      await new SoftwareCollectionPage(page).selectRow(0);
      await gui.checkScreenshot(page, 'software-row-selected');
    });

    test('revision dialog', async ({ page, gui, user }) => {
      test.skip(!canRunSoftwareCommand(user, 'Revision'), 'no Revision permission');
      const software = new SoftwareCollectionPage(page);
      await software.selectRow(0);
      await software.revision();
      await gui.checkScreenshot(page, 'software-revision-dialog');
    });

    test('remove confirmation dialog', async ({ page, gui, user }) => {
      test.skip(!canRunSoftwareCommand(user, 'Remove'), 'no Remove permission');
      const software = new SoftwareCollectionPage(page);
      await software.selectRow(0);
      await software.removeItem();
      await gui.checkScreenshot(page, 'software-remove-dialog');
    });

    test('split dialog', async ({ page, gui, user }) => {
      test.skip(!canRunSoftwareCommand(user, 'Split'), 'no Split permission');
      const software = new SoftwareCollectionPage(page);
      await software.selectRow(0);
      await software.split();
      await gui.checkScreenshot(page, 'software-split-dialog');
    });

    test('revoke dialog', async ({ page, gui, user }) => {
      test.skip(!canRunSoftwareCommand(user, 'Revoke'), 'no Revoke permission');
      const software = new SoftwareCollectionPage(page);
      await software.selectRow(0);
      await software.revoke();
      await gui.checkScreenshot(page, 'software-revoke-dialog');
    });
  });
});
