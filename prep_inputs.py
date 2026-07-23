import re, html, json, sys
sys.path.insert(0,'.')
from tslib import iter_messages, get_source, get_translation, raw_to_key

def parse_full(path):
    content=open(path,encoding='utf-8').read()
    out=[]
    # need context name per message
    for cm in re.finditer(r'<context>(.*?)</context>', content, re.S):
        cbody=cm.group(1)
        name=re.search(r'<name>(.*?)</name>',cbody,re.S).group(1)
        for m in iter_messages(cbody):
            mbody=m.group(2)
            raw=get_source(mbody)
            attr,txt=get_translation(mbody)
            out.append((name,raw,attr,txt))
    return out

fr=parse_full('Impl/ProLifeLoc/Translations/ProLife_fr_FR.ts')
# unique unfinished plain keys with contexts, first-seen order
ctx={}; order=[]
for (n,raw,attr,txt) in fr:
    if attr and 'unfinished' in attr:
        k=raw_to_key(raw)
        if k not in ctx:
            ctx[k]=[]; order.append(k)
        if n not in ctx[k]:
            ctx[k].append(n)
inp={"count":len(order),"strings":[{"en":k,"contexts":ctx[k]} for k in order]}
json.dump(inp, open('translate_input.json','w',encoding='utf-8'), ensure_ascii=False, indent=1)
print("translate_input.json:",len(order),"unique plain keys")

# plain-keyed glossaries per language (finished entries, real translations only)
for lang in ['fr_FR','it_IT','jp_JP','pl_PL','zh_ZH','de_DE']:
    data=parse_full(f'Impl/ProLifeLoc/Translations/ProLife_{lang}.ts')
    g={}
    for (n,raw,attr,txt) in data:
        if attr is not None and 'unfinished' not in attr and raw and txt:
            k=raw_to_key(raw); v=html.unescape(txt)
            if k!=v:  # only real translations
                g.setdefault(k,v)
    json.dump(g, open(f'glossary_plain_{lang}.json','w',encoding='utf-8'), ensure_ascii=False, indent=1)
    print(f"glossary_plain_{lang}.json:",len(g),"entries")
