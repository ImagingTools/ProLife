// Organizations (Tenants) page.
//
// TenantCollectionView is not objectName-instrumented internally yet, so coverage is opening the page
// (ViewOrganizations) and a landing screenshot.

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
