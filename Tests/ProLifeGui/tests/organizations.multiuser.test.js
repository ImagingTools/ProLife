// Organizations (Tenants) page - multi-user coverage.
//
// TenantCollectionView is not objectName-instrumented internally yet, so coverage is a per-user
// landing screenshot plus opening the page for authorized users. `orgViewer` (ViewOrganizations only)
// is the user that makes this page appear; every other seeded user must NOT see it, which the
// "menu reflects permissions" test in workspace.multiuser.test.js asserts structurally now that
// Tenants is part of the permission matrix.

const { test } = require('../fixtures/test');
const { OrganizationsPage } = require('../pages');
const { canSeePage } = require('../matrix/permissions');

const PAGE = 'Tenants';

test.describe('Organizations', () => {
  test.beforeEach(async ({ page }) => {
    await new OrganizationsPage(page).reload();
  });

  test('landing', async ({ page, gui, user }) => {
    if (canSeePage(user, PAGE)) await new OrganizationsPage(page).open();
    await gui.checkScreenshot(page, 'organizations-landing');
  });

  test('opens for authorized users', async ({ page, gui, user }) => {
    test.skip(!canSeePage(user, PAGE), 'user cannot see Organizations');
    await new OrganizationsPage(page).open(); // open() hard-fails if the TenantsButton is missing
    await gui.checkScreenshot(page, 'organizations-view');
  });
});
