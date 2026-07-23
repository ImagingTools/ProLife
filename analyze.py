import re

def parse_ts(path):
    with open(path, encoding='utf-8') as f:
        content = f.read()
    results = []
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

data = parse_ts('Impl/ProLifeLoc/Translations/ProLife_fr_FR.ts')
finished = [(n,s,t) for (n,s,a,t) in data if 'unfinished' not in a]
print("=== fr_FR: sample of 30 FINISHED entries (real translations?) ===")
for n,s,t in finished[:30]:
    print(f"  [{n}] {s!r} -> {t!r}")
