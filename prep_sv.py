import re, html, json, sys
sys.path.insert(0,'.')
from tslib import raw_to_key

content=open('Impl/ProLifeLoc/Translations/ProLife_en_US.ts',encoding='utf-8').read()
ctxmap={}; order=[]
n_active=0
for cm in re.finditer(r'<context>(.*?)</context>', content, re.S):
    cb=cm.group(1); name=re.search(r'<name>(.*?)</name>',cb,re.S).group(1)
    for m in re.finditer(r'<message[^>]*>(.*?)</message>', cb, re.S):
        mb=m.group(1)
        s=re.search(r'<source>(.*?)</source>',mb,re.S)
        t=re.search(r'<translation([^>]*)>(.*?)</translation>',mb,re.S)
        attr=t.group(1) if t else ''
        if 'obsolete' in attr or 'vanished' in attr: continue
        n_active+=1
        k=raw_to_key(s.group(1))
        if k not in ctxmap:
            ctxmap[k]=[]; order.append(k)
        if name not in ctxmap[k]:
            ctxmap[k].append(name)
print("active messages:",n_active,"unique sources:",len(order))
inp={"count":len(order),"strings":[{"en":k,"contexts":ctxmap[k]} for k in order]}
json.dump(inp, open('sv_input.json','w',encoding='utf-8'), ensure_ascii=False, indent=1)

# Overlap with the 624 legacy set (for reference)
leg=set(x['en'] for x in json.load(open('translate_input.json',encoding='utf-8'))['strings'])
ov=sum(1 for k in order if k in leg)
print("overlap with legacy 624:",ov,"unique-to-sv:",len(order)-ov)
