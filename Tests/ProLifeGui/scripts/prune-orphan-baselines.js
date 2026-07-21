// Finds (and, with --delete, removes) orphaned screenshot baselines under tests/__screenshots__.
//
// Two kinds of orphan, both harmless-but-clutter rather than test-breaking:
//   1. SPEC-level: tests/__screenshots__/<project>/<specRelPath>/ where <specRelPath> no longer
//      exists under tests/ at all - the spec file was renamed, split, or deleted (e.g. this session's
//      administration.multiuser.test.js -> administration.editor.multiuser.test.js split, or the
//      earlier duplicate-test-deletion pass), but its baselines were never cleaned up.
//   2. SCREENSHOT-level: an individual <name>-<platform>.png inside an EXISTING spec's baseline
//      directory whose <name> no longer appears in any `checkScreenshot(page, '<name>'` call in that
//      spec's current source - the spec still exists, but that particular check was renamed or
//      removed from it.
//
// Usage:
//   node scripts/prune-orphan-baselines.js            # report only, deletes nothing
//   node scripts/prune-orphan-baselines.js --delete    # actually remove the orphans found

const fs = require('fs');
const path = require('path');

const TESTS_DIR = path.resolve(__dirname, '..', 'tests');
const SCREENSHOTS_DIR = path.join(TESTS_DIR, '__screenshots__');
const DELETE = process.argv.includes('--delete');

function listDirs(dir) {
  if (!fs.existsSync(dir)) return [];
  return fs.readdirSync(dir, { withFileTypes: true }).filter((e) => e.isDirectory()).map((e) => e.name);
}

// Recursively find every baseline leaf directory under a project folder, returning its path relative
// to the project folder (e.g. "devices.editor.multiuser.test.js" - specs live flat in tests/, but
// this stays recursive in case that ever changes) and the list of .png files directly inside it.
function findSpecDirs(projectDir, relPath = '') {
  const abs = path.join(projectDir, relPath);
  const entries = fs.readdirSync(abs, { withFileTypes: true });
  const pngFiles = entries.filter((e) => e.isFile() && e.name.endsWith('.png')).map((e) => e.name);
  const results = [];
  if (pngFiles.length > 0) {
    results.push({ relPath, pngFiles });
  }
  for (const e of entries) {
    if (e.isDirectory()) {
      results.push(...findSpecDirs(projectDir, path.join(relPath, e.name)));
    }
  }
  return results;
}

// Extract every `checkScreenshot(<page-expr>, 'name'` / `"name"` literal from a spec file's source -
// good enough for this codebase's convention of always passing a string literal, not a template/var.
function extractScreenshotNames(specSource) {
  const names = new Set();
  const re = /checkScreenshot\s*\(\s*[^,]+,\s*['"]([^'"]+)['"]/g;
  let m;
  while ((m = re.exec(specSource))) {
    names.add(m[1]);
  }
  return names;
}

function main() {
  if (!fs.existsSync(SCREENSHOTS_DIR)) {
    console.log('No __screenshots__ directory found - nothing to check.');
    return;
  }

  const orphanSpecDirs = [];
  const orphanScreenshotFiles = [];

  for (const project of listDirs(SCREENSHOTS_DIR)) {
    const projectDir = path.join(SCREENSHOTS_DIR, project);
    for (const { relPath, pngFiles } of findSpecDirs(projectDir)) {
      // relPath is the spec's path under tests/ (e.g. "devices.editor.multiuser.test.js").
      const specPath = path.join(TESTS_DIR, relPath);
      const dirPath = path.join(projectDir, relPath);

      if (!fs.existsSync(specPath)) {
        orphanSpecDirs.push(dirPath);
        continue;
      }

      const specSource = fs.readFileSync(specPath, 'utf8');
      const validNames = extractScreenshotNames(specSource);

      for (const pngFile of pngFiles) {
        // Strip the trailing "-<platform>.png" (e.g. "-linux.png"/"-win32.png") to get the name passed
        // to checkScreenshot.
        const baseName = pngFile.replace(/-[^-]+\.png$/, '');
        if (!validNames.has(baseName)) {
          orphanScreenshotFiles.push(path.join(dirPath, pngFile));
        }
      }
    }
  }

  console.log(`Orphaned spec baseline directories (spec file no longer exists): ${orphanSpecDirs.length}`);
  for (const d of orphanSpecDirs) console.log(`  ${path.relative(TESTS_DIR, d)}`);

  console.log(`\nOrphaned individual screenshots (spec exists, this check name doesn't): ${orphanScreenshotFiles.length}`);
  for (const f of orphanScreenshotFiles) console.log(`  ${path.relative(TESTS_DIR, f)}`);

  if (!DELETE) {
    if (orphanSpecDirs.length + orphanScreenshotFiles.length > 0) {
      console.log('\nRun with --delete to remove the above.');
    }
    return;
  }

  for (const d of orphanSpecDirs) fs.rmSync(d, { recursive: true, force: true });
  for (const f of orphanScreenshotFiles) fs.rmSync(f, { force: true });
  console.log('\nDeleted.');
}

main();
