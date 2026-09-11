// Organizations (Tenants) page - multi-user coverage.
//
// TenantCollectionView is not objectName-instrumented internally yet, so coverage is a per-user
// landing screenshot plus opening the page for users whose menu offers it - `orgViewer` is the one
// seeded user that makes this page appear, and the landing screenshots are what record that.

const { test } = require('../fixtures/test');
const { OrganizationsPage } = require('../pages');

test.describe('Organizations', () => {
  test.beforeEach(async ({ page }) => {
    await new OrganizationsPage(page).reload();
  });

  test('landing', async ({ page, gui }) => {
    const organizations = new OrganizationsPage(page);
    test.skip(!(await organizations.isAvailable()), 'Organizations is not available to this user');
    await organizations.open();
    await gui.checkScreenshot(page, 'organizations-landing');
  });
});
