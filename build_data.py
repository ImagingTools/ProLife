import re, json, html

def parse_ts(path):
    with open(path, encoding='utf-8') as f:
        content = f.read()
    results = []
    for cm in re.finditer(r'<context>(.*?)</context>', content, re.S):
        cbody = cm.group(1)
        name = re.search(r'<name>(.*?)</name>', cbody, re.S).group(1)
        for mm in re.finditer(r'<message([^>]*)>(.*?)</message>', cbody, re.S):
            mattr = mm.group(1); mbody = mm.group(2)
            src = re.search(r'<source>(.*?)</source>', mbody, re.S)
            tr = re.search(r'<translation([^>]*)>(.*?)</translation>', mbody, re.S)
            src_t = src.group(1) if src else None
            tr_attr = tr.group(1) if tr else ''
            tr_text = tr.group(2) if tr else ''
            results.append((name, src_t, tr_attr, tr_text, mattr))
    return results

# Context map + unique order from fr
fr = parse_ts('Impl/ProLifeLoc/Translations/ProLife_fr_FR.ts')
ctxmap = {}
order = []
for (n,s,a,t,ma) in fr:
    if 'unfinished' in a:
        if s not in ctxmap:
            ctxmap[s] = []
            order.append(s)
        if n not in ctxmap[s]:
            ctxmap[s].append(n)

with open('unfinished_context_map.json','w',encoding='utf-8') as f:
    json.dump({s:ctxmap[s] for s in order}, f, ensure_ascii=False, indent=1)
print("unique strings:", len(order))

# Per-language glossary of FINISHED entries (source->translation), skip identity & resource-like
for lang in ['fr_FR','it_IT','jp_JP','pl_PL','zh_ZH','de_DE']:
    data = parse_ts(f'Impl/ProLifeLoc/Translations/ProLife_{lang}.ts')
    gloss = {}
    for (n,s,a,t,ma) in data:
        if 'unfinished' not in a and t and s:
            # keep only where translation differs from source (real translation) OR keep all for reference
            gloss.setdefault(s, t)
    with open(f'glossary_{lang}.json','w',encoding='utf-8') as f:
        json.dump(gloss, f, ensure_ascii=False, indent=0)
    diff = sum(1 for s,t in gloss.items() if s!=t)
    print(f"{lang}: glossary entries={len(gloss)} (differing={diff})")
