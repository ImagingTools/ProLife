// Extracts the DECLARED permission model straight from the product's own configuration, so the test
// suite compares the running system against its specification instead of against a hand-transcribed
// copy of it (matrix/permissions.js is today a manual transcription and will drift).
//
// Sources, all of them the files the product itself is built from:
//   ProLifeFeatures.xml      - every permission id that exists
//   Pages.acc                - page -> PagePermissions (the menu rule is: non-empty intersection)
//   ProLifeVoce.arp/*Permissions.acc  - commandId -> permissions the SERVER enforces
//   ProLifeQmlVoce.arp/*Page.acc      - the GUI commands each page declares
//
// The server filter only applies to commands that appear in the *Permissions.acc registry:
// CCommandsControllerComp skips the check when GetCommandPermissions() comes back empty. A command
// missing from the registry is therefore returned to every user and gated by QML alone - which is
// exactly the distinction the browser tests need in order to know what they still have to cover.
//
//   node scripts/extract-permission-model.js            # write matrix/declared.json, report lint
//   node scripts/extract-permission-model.js --check    # lint only, non-zero exit on findings

const fs = require('fs');
const path = require('path');

const PROLIFE_ROOT = path.resolve(__dirname, '..', '..', '..');
const FEATURES_XML = path.join(PROLIFE_ROOT, 'Impl', 'ProLifeServer', 'ProLifeFeatures.xml');
const QML_VOCE = path.join(PROLIFE_ROOT, 'Partitura', 'ProLifeQmlVoce.arp');
const SERVER_VOCE = path.join(PROLIFE_ROOT, 'Partitura', 'ProLifeVoce.arp');
const OUT = path.resolve(__dirname, '..', 'matrix', 'declared.json');

const CHECK_ONLY = process.argv.includes('--check');

// --- minimal XML walk ----------------------------------------------------------------------------
// The .acc/.xml files are machine-generated and highly regular, but Elements nest (a page contains
// sub-Elements), so depth has to be tracked rather than pattern-matched flat.

function parseAttrs(tagBody) {
  const attrs = {};
  const re = /([A-Za-z_][\w.:-]*)\s*=\s*"([^"]*)"/g;
  let m;
  while ((m = re.exec(tagBody))) attrs[m[1]] = m[2];
  return attrs;
}

/** Walk every tag in document order, reporting name, attrs, depth and whether it self-closes. */
function walk(xml, visit) {
  const re = /<([A-Za-z_][\w.:-]*)((?:[^>"]|"[^"]*")*?)(\/?)>|<\/([A-Za-z_][\w.:-]*)>/g;
  let m;
  let depth = 0;
  while ((m = re.exec(xml))) {
    if (m[4] !== undefined) {
      depth--;
      visit({ name: m[4], close: true, depth });
      continue;
    }
    const selfClosing = m[3] === '/';
    visit({ name: m[1], attrs: parseAttrs(m[2]), depth, selfClosing, index: m.index });
    if (!selfClosing) depth++;
  }
}

function read(file) {
  return fs.readFileSync(file, 'utf8');
}

// --- 1. every permission that exists -------------------------------------------------------------

function declaredPermissions() {
  const permissions = new Map(); // id -> name
  walk(read(FEATURES_XML), (node) => {
    if (node.close || !node.attrs) return;
    const { FeatureId, FeatureName, IsPermission } = node.attrs;
    if (FeatureId && IsPermission === 'true') permissions.set(FeatureId, FeatureName || FeatureId);
  });
  return permissions;
}

// --- generic .acc element reader -----------------------------------------------------------------
//
// Returns top-level-of-their-own-scope Elements as {id, componentId, packageId, attrs, depth}, where
// attrs maps AttributeInfo Id -> string value or array of values. An AttributeInfo belongs to the
// nearest enclosing Element, so the walk keeps an Element stack.

function readAccElements(file) {
  const xml = read(file);
  const elements = [];
  const stack = [];
  let currentAttr = null;
  let inValues = false;

  walk(xml, (node) => {
    if (node.close) {
      if (node.name === 'Element') stack.pop();
      if (node.name === 'AttributeInfo') currentAttr = null;
      if (node.name === 'Values') inValues = false;
      return;
    }
    switch (node.name) {
      case 'Element': {
        const el = {
          id: node.attrs.Id,
          componentId: node.attrs.ComponentId,
          packageId: node.attrs.PackageId,
          depth: node.depth,
          attrs: {},
        };
        elements.push(el);
        if (!node.selfClosing) stack.push(el);
        break;
      }
      case 'AttributeInfo':
        currentAttr = node.attrs.Id;
        break;
      case 'Values':
        inValues = true;
        if (currentAttr && stack.length) stack[stack.length - 1].attrs[currentAttr] = [];
        break;
      case 'Value': {
        // <Value>text</Value> - the text follows the tag we just matched.
        if (!inValues || !currentAttr || !stack.length) break;
        const rest = xml.slice(node.index);
        const text = /^<Value>([^<]*)<\/Value>/.exec(rest);
        if (text) stack[stack.length - 1].attrs[currentAttr].push(text[1]);
        break;
      }
      case 'Data':
        if (currentAttr && stack.length && node.attrs.Value !== undefined) {
          stack[stack.length - 1].attrs[currentAttr] = node.attrs.Value;
        }
        break;
      default:
        break;
    }
  });

  return elements;
}

// --- 2. pages ------------------------------------------------------------------------------------

function declaredPages() {
  const pages = {};
  for (const el of readAccElements(path.join(QML_VOCE, 'Pages.acc'))) {
    const perms = el.attrs.PagePermissions;
    if (!Array.isArray(perms)) continue;
    pages[el.id] = {
      permissions: perms,
      visibleInMenu: el.attrs.IsVisible !== 'false',
      name: el.attrs.Name || el.id,
      component: `${el.packageId}/${el.componentId}`,
    };
  }
  return pages;
}

// --- 3. the server-enforced command registry -----------------------------------------------------

function serverCommandRegistry() {
  const registry = {};
  for (const file of fs.readdirSync(SERVER_VOCE).filter((f) => f.endsWith('Permissions.acc'))) {
    for (const el of readAccElements(path.join(SERVER_VOCE, file))) {
      const commandId = el.attrs.CommandId;
      const perms = el.attrs.CommandPermissions;
      if (!commandId || !Array.isArray(perms)) continue;
      registry[commandId] = { permissions: perms, source: file };
    }
  }
  return registry;
}

// --- 4. the GUI commands each page declares ------------------------------------------------------

function pageCommands() {
  const byPage = {};
  for (const file of fs.readdirSync(QML_VOCE).filter((f) => f.endsWith('Page.acc'))) {
    const page = file.replace(/\.acc$/, '');
    const ids = new Set();
    for (const el of readAccElements(path.join(QML_VOCE, file))) {
      if (el.componentId === 'Command' && el.id) ids.add(el.id);
    }
    if (ids.size) byPage[page] = [...ids].sort();
  }
  return byPage;
}

// --- build + lint --------------------------------------------------------------------------------

function build() {
  const permissions = declaredPermissions();
  const pages = declaredPages();
  const registry = serverCommandRegistry();
  const commands = pageCommands();

  const findings = [];

  for (const [pageId, page] of Object.entries(pages)) {
    for (const perm of page.permissions) {
      if (!permissions.has(perm)) {
        findings.push(`Pages.acc: page "${pageId}" requires permission "${perm}", which ProLifeFeatures.xml does not define`);
      }
    }
  }

  for (const [commandId, entry] of Object.entries(registry)) {
    for (const perm of entry.permissions) {
      if (!permissions.has(perm)) {
        findings.push(`${entry.source}: command "${commandId}" requires permission "${perm}", which ProLifeFeatures.xml does not define`);
      }
    }
  }

  // The test suite's own fixture roles. A role that grants a permission the product never defines
  // exercises a path no real role can reach: production roles are assembled from ProLifeFeatures.xml,
  // while the fixtures write raw permission strings straight through the API.
  const { USERS } = require('../fixtures/users');
  for (const user of USERS) {
    for (const perm of user.permissions) {
      if (perm !== '*' && !permissions.has(perm)) {
        findings.push(`fixtures/users.js: role "${user.key}" grants "${perm}", which ProLifeFeatures.xml does not define`);
      }
    }
  }

  // Which GUI commands the server actually filters, and which reach every user regardless.
  const gating = {};
  for (const [page, ids] of Object.entries(commands)) {
    gating[page] = {};
    for (const id of ids) {
      gating[page][id] = registry[id]
        ? { enforcedBy: 'server', permissions: registry[id].permissions }
        : { enforcedBy: 'client-only' };
    }
  }

  return {
    model: {
      generatedFrom: ['ProLifeFeatures.xml', 'Pages.acc', 'ProLifeVoce.arp/*Permissions.acc', 'ProLifeQmlVoce.arp/*Page.acc'],
      permissions: Object.fromEntries([...permissions].sort()),
      pages,
      serverCommandRegistry: registry,
      pageCommands: gating,
    },
    findings,
  };
}

function main() {
  const { model, findings } = build();

  const clientOnly = [];
  for (const [page, cmds] of Object.entries(model.pageCommands)) {
    for (const [id, info] of Object.entries(cmds)) {
      if (info.enforcedBy === 'client-only') clientOnly.push(`${page}/${id}`);
    }
  }

  console.log(`permissions defined:        ${Object.keys(model.permissions).length}`);
  console.log(`pages with PagePermissions: ${Object.keys(model.pages).length}`);
  console.log(`server-enforced commands:   ${Object.keys(model.serverCommandRegistry).length}`);
  console.log(`GUI commands, client-only:  ${clientOnly.length}`);
  for (const c of clientOnly) console.log(`    ${c}`);

  if (findings.length) {
    console.log(`\n${findings.length} finding(s):`);
    for (const f of findings) console.log(`  - ${f}`);
  } else {
    console.log('\nno inconsistencies between the configuration files');
  }

  if (!CHECK_ONLY) {
    fs.writeFileSync(OUT, `${JSON.stringify(model, null, 2)}\n`);
    console.log(`\nwritten: ${path.relative(path.resolve(__dirname, '..'), OUT)}`);
  }

  process.exit(findings.length && CHECK_ONLY ? 1 : 0);
}

main();
