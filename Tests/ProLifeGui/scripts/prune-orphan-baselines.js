// Reports (and, with --delete, removes) baselines nothing can compare against any more, and flags
// baselines that are suspiciously identical to one another.
//
// Baselines live flat per user: tests/__screenshots__/<project>/<name>-<platform>.png. Screenshot
// names are therefore global across the suite, which is what lets this work from names alone.
//
// Three kinds of dead baseline:
//   1. the name no longer appears in any spec - the check was renamed or removed;
//   2. the name belongs to a spec this project does not run (a user whose permissions exclude it, or
//      an isolatedSpec pinned to somebody else);
//   3. the name belongs to a @mutating test and this project is excluded from the mutating phase.
//
// And one thing it only REPORTS: two baselines of the same user that are byte-identical. That is how
// two real defects looked - a landing shot identical to another page's (navigation never happened) and
// a "saved" shot identical to its "filled" predecessor (the save did nothing) - both committed as
// expected state, where no screenshot comparison could ever notice.
//
//   node scripts/prune-orphan-baselines.js            # report only
//   node scripts/prune-orphan-baselines.js --delete   # remove the dead ones (never the identical ones)

const fs = require('fs');
const path = require('path');
const crypto = require('crypto');
const { execFileSync } = require('child_process');
const { collectionScreenshotNamesFromSource } = require('imtcore-gui-testkit/specs/collectionSpec');

const TESTS_DIR = path.resolve(__dirname, '..', 'tests');
const SCREENSHOTS_DIR = path.join(TESTS_DIR, '__screenshots__');
const DELETE = process.argv.includes('--delete');

function listDirs(dir) {
  if (!fs.existsSync(dir)) return [];
  return fs.readdirSync(dir, { withFileTypes: true }).filter((e) => e.isDirectory()).map((e) => e.name);
}

/** Every `checkScreenshot(page, 'name'` literal, plus whatever a declared collection spec generates. */
function screenshotNamesIn(specSource) {
  const names = new Set();
  const re = /checkScreenshot\s*\(\s*[^,]+,\s*['"]([^'"]+)['"]/g;
  let m;
  while ((m = re.exec(specSource))) names.add(m[1]);
  // Throws rather than returning null if it finds a declaration it cannot read - see its own comment.
  for (const name of collectionScreenshotNamesFromSource(specSource) || []) names.add(name);
  return names;
}

/** The names produced inside a test tagged @mutating. */
function mutatingScreenshotNamesIn(specSource) {
  const names = new Set();
  for (const block of specSource.split(/\n(?=\s*test\s*\()/)) {
    const arrow = block.indexOf('=>');
    const header = arrow === -1 ? block : block.slice(0, arrow);
    if (!header.includes('@mutating')) continue;
    const re = /checkScreenshot\s*\(\s*[^,]+,\s*['"]([^'"]+)['"]/g;
    let m;
    while ((m = re.exec(block))) names.add(m[1]);
  }
  return names;
}

/**
 * Which (project, spec) pairs Playwright schedules - asked of Playwright rather than reimplemented from
 * testIgnore/testMatch/grepInvert. Listed with the full matrix on, so baselines belonging to users
 * outside the default subset are not mistaken for dead. Null when no listing can be produced, and the
 * caller then skips the checks that depend on it rather than calling everything an orphan.
 */
function scheduledSpecsByProject(extraArgs = []) {
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
  const re = /^\s*\[([^\]]+)\]\s+›\s+(\S+?):/gm;
  let m;
  while ((m = re.exec(output))) {
    if (!byProject.has(m[1])) byProject.set(m[1], new Set());
    byProject.get(m[1]).add(path.basename(m[2].trim()));
  }
  return byProject.size ? byProject : null;
}

function main() {
  if (!fs.existsSync(SCREENSHOTS_DIR)) {
    console.log('No __screenshots__ directory found - nothing to check.');
    return;
  }

  // name -> { spec, mutating }. A name used by two specs would collide in the flat layout, so say so.
  const owners = new Map();
  const duplicateNames = [];
  for (const spec of fs.readdirSync(TESTS_DIR).filter((f) => f.endsWith('.js'))) {
    const source = fs.readFileSync(path.join(TESTS_DIR, spec), 'utf8');
    const mutating = mutatingScreenshotNamesIn(source);
    for (const name of screenshotNamesIn(source)) {
      if (owners.has(name)) duplicateNames.push(`${name} (${owners.get(name).spec} and ${spec})`);
      owners.set(name, { spec, mutating: mutating.has(name) });
    }
  }

  const scheduled = scheduledSpecsByProject();
  const mutatingProjects = scheduled ? scheduledSpecsByProject(['--grep', '@mutating']) : null;
  if (!scheduled) {
    console.log('Could not list the test graph - skipping the "this project does not run it" checks.\n');
  }

  const dead = [];
  for (const project of listDirs(SCREENSHOTS_DIR)) {
    const projectDir = path.join(SCREENSHOTS_DIR, project);
    for (const file of fs.readdirSync(projectDir).filter((f) => f.endsWith('.png'))) {
      const name = file.replace(/-[^-]+\.png$/, '');
      const owner = owners.get(name);
      const full = path.join(projectDir, file);
      if (!owner) {
        dead.push({ full, why: 'no spec takes a screenshot by this name' });
        continue;
      }
      if (!scheduled) continue;
      if (!(scheduled.get(project) || new Set()).has(owner.spec)) {
        dead.push({ full, why: `${project} does not run ${owner.spec}` });
        continue;
      }
      if (owner.mutating && !(mutatingProjects.get(project) || new Set()).has(owner.spec)) {
        dead.push({ full, why: `${project} is excluded from the @mutating phase` });
      }
    }
  }

  if (duplicateNames.length) {
    console.log(`Screenshot names used by more than one spec: ${duplicateNames.length}`);
    for (const d of duplicateNames) console.log(`  ${d}`);
    console.log('  Baselines are keyed by name alone, so these two specs share one file.\n');
  }

  console.log(`Baselines nothing can compare against: ${dead.length}`);
  for (const { full, why } of dead) console.log(`  ${path.relative(TESTS_DIR, full)}  - ${why}`);

  reportIdenticalBaselines();

  if (!DELETE) {
    if (dead.length) console.log('\nRun with --delete to remove them.');
    return;
  }
  for (const { full } of dead) fs.rmSync(full, { force: true });
  console.log('\nDeleted.');
}

/**
 * Baselines byte-identical to another baseline of the SAME user. Reported, never deleted - the
 * duplicate is the evidence, and which one of the pair is wrong is a judgement call.
 */
function reportIdenticalBaselines() {
  const collisions = [];
  for (const project of listDirs(SCREENSHOTS_DIR)) {
    const projectDir = path.join(SCREENSHOTS_DIR, project);
    const byHash = new Map();
    for (const file of fs.readdirSync(projectDir).filter((f) => f.endsWith('.png'))) {
      const hash = crypto.createHash('md5').update(fs.readFileSync(path.join(projectDir, file))).digest('hex');
      if (!byHash.has(hash)) byHash.set(hash, []);
      byHash.get(hash).push(file);
    }
    for (const files of byHash.values()) {
      if (files.length > 1) collisions.push({ project, files });
    }
  }

  console.log(`\nBaselines identical to another baseline of the same user: ${collisions.length}`);
  for (const { project, files } of collisions) {
    console.log(`  [${project}] ${files.join('  ==  ')}`);
  }
  if (collisions.length) {
    console.log('  Each group is one image committed under several names - usually a navigation that did');
    console.log('  not happen, or an action that changed nothing. Not pruned: decide which one is wrong.');
  }
}

main();
