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

// Which (project, spec) pairs Playwright actually schedules - asked of Playwright itself rather than
// reimplemented from the config's testIgnore/testMatch/grepInvert rules. Listed with the full matrix
// enabled, so baselines belonging to users outside the default fast subset are not mistaken for dead.
// Returns null if no listing can be produced, and the caller then skips this check rather than
// reporting every baseline in the repo as an orphan.
function scheduledSpecsByProject(extraArgs = []) {
  const { execFileSync } = require('child_process');
  let output = '';
  try {
    output = execFileSync('npx', ['playwright', 'test', '--list', ...extraArgs], {
      cwd: path.resolve(__dirname, '..'),
      env: { ...process.env, PROLIFE_GUI_ALL_USERS: '1' },
      encoding: 'utf8',
      shell: process.platform === 'win32',
      stdio: ['ignore', 'pipe', 'ignore'],
    });
  } catch (err) {
    output = (err && err.stdout) || '';
  }

  const byProject = new Map();
  const re = /^\s*\[([^\]]+)\]\s+›\s+([^:]+):/gm;
  let m;
  while ((m = re.exec(output))) {
    if (!byProject.has(m[1])) byProject.set(m[1], new Set());
    byProject.get(m[1]).add(m[2].trim().replace(/\\/g, '/'));
  }
  return byProject.size ? byProject : null;
}

// Screenshot names taken inside a test tagged @mutating. A project excluded from the mutating phase
// (buildProjects' mutatingUserKeys -> a project-level grepInvert) still has that spec scheduled, so the
// directory-level check above cannot see that these particular baselines are now unreachable.
function mutatingScreenshotNames(specSource) {
  const names = new Set();
  // Split at each test( boundary and keep the blocks whose header carries the tag.
  const blocks = specSource.split(/\n(?=\s*test\s*\()/);
  for (const block of blocks) {
    const arrow = block.indexOf('=>');
    const header = arrow === -1 ? block : block.slice(0, arrow);
    if (!header.includes('@mutating')) continue;
    const re = /checkScreenshot\s*\(\s*[^,]+,\s*['"]([^'"]+)['"]/g;
    let m;
    while ((m = re.exec(block))) names.add(m[1]);
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
  const unscheduledDirs = [];

  const scheduled = scheduledSpecsByProject();
  const mutatingProjects = scheduled ? scheduledSpecsByProject(['--grep', '@mutating']) : null;
  if (!scheduled) {
    console.log('Could not list the test graph - skipping the "project no longer runs this spec" check.\n');
  }

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

      // The spec still exists, but this project no longer runs it - e.g. a user whose permissions put
      // the spec outside its project (playwright.config.js's SPEC_PAGES). These baselines can never be
      // compared against anything again.
      if (scheduled) {
        const specsHere = scheduled.get(project);
        if (!specsHere || !specsHere.has(relPath.replace(/\\/g, '/'))) {
          unscheduledDirs.push(dirPath);
          continue;
        }
      }

      const specSource = fs.readFileSync(specPath, 'utf8');
      const validNames = extractScreenshotNames(specSource);
      // Drop the names only a @mutating test produces when this project is excluded from that phase.
      if (mutatingProjects && !(mutatingProjects.get(project) || new Set()).has(relPath.replace(/\\/g, '/'))) {
        for (const name of mutatingScreenshotNames(specSource)) validNames.delete(name);
      }

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

  console.log(`\nBaseline directories this project no longer runs: ${unscheduledDirs.length}`);
  for (const d of unscheduledDirs) console.log(`  ${path.relative(TESTS_DIR, d)}`);

  console.log(`\nOrphaned individual screenshots (spec exists, this check name doesn't): ${orphanScreenshotFiles.length}`);
  for (const f of orphanScreenshotFiles) console.log(`  ${path.relative(TESTS_DIR, f)}`);

  const total = orphanSpecDirs.length + unscheduledDirs.length + orphanScreenshotFiles.length;
  if (!DELETE) {
    if (total > 0) console.log('\nRun with --delete to remove the above.');
    return;
  }

  for (const d of [...orphanSpecDirs, ...unscheduledDirs]) fs.rmSync(d, { recursive: true, force: true });
  for (const f of orphanScreenshotFiles) fs.rmSync(f, { force: true });
  console.log('\nDeleted.');
}

main();
