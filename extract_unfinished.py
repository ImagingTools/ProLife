import re, sys

def parse_ts(path):
    with open(path, encoding='utf-8') as f:
        content = f.read()
    # find all messages with their source and translation
    results = []
    # split by context
    for cm in re.finditer(r'<context>(.*?)</context>', content, re.S):
        cbody = cm.group(1)
        name = re.search(r'<name>(.*?)</name>', cbody, re.S).group(1)
        for mm in re.finditer(r'<message[^>]*>(.*?)</message>', cbody, re.S):
            mbody = mm.group(1)
            src = re.search(r'<source>(.*?)</source>', mbody, re.S)
            tr = re.search(r'<translation([^>]*)>(.*?)</translation>', mbody, re.S)
            src_t = src.group(1) if src else None
            tr_attr = tr.group(1) if tr else ''
            tr_text = tr.group(2) if tr else ''
            results.append((name, src_t, tr_attr, tr_text))
    return results

for lang in ['fr_FR','it_IT','jp_JP','pl_PL','zh_ZH']:
    data = parse_ts(f'Impl/ProLifeLoc/Translations/ProLife_{lang}.ts')
    unf = [(n,s) for (n,s,a,t) in data if 'unfinished' in a]
    print(f'{lang}: total={len(data)} unfinished={len(unf)}')

# Compare unfinished sets across langs
sets = {}
for lang in ['fr_FR','it_IT','jp_JP','pl_PL','zh_ZH']:
    data = parse_ts(f'Impl/ProLifeLoc/Translations/ProLife_{lang}.ts')
    sets[lang] = set((n,s) for (n,s,a,t) in data if 'unfinished' in a)
base = sets['fr_FR']
for lang in ['it_IT','jp_JP','pl_PL','zh_ZH']:
    print(f'{lang} unfinished == fr_FR unfinished: {sets[lang]==base}')

# Unique sources among unfinished
uniq_src = set(s for (n,s) in base)
print('Unique unfinished source strings (fr):', len(uniq_src))
