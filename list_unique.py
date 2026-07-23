import re, json

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
# unique unfinished sources, preserving first-seen order
seen = {}
for (n,s,a,t) in data:
    if 'unfinished' in a and s not in seen:
        seen[s] = n
uniq = list(seen.keys())
print("TOTAL UNIQUE UNFINISHED:", len(uniq))
# length distribution
import statistics
lens = [len(s) for s in uniq]
short = sum(1 for l in lens if l<=15)
med = sum(1 for l in lens if 15<l<=40)
lng = sum(1 for l in lens if l>40)
print(f"short(<=15 chars): {short}, medium(16-40): {med}, long(>40): {lng}")
with open('unique_unfinished.json','w',encoding='utf-8') as f:
    json.dump(uniq, f, ensure_ascii=False, indent=0)
print("Wrote unique_unfinished.json")
