import re, html, json, sys
sys.path.insert(0,'.')
from tslib import raw_to_key

def parse_map(path):
    content=open(path,encoding='utf-8').read()
    m={}
    for cm in re.finditer(r'<context>(.*?)</context>', content, re.S):
        cb=cm.group(1)
        for mm in re.finditer(r'<message[^>]*>(.*?)</message>', cb, re.S):
            mb=mm.group(1)
            s=re.search(r'<source>(.*?)</source>',mb,re.S)
            t=re.search(r'<translation([^>]*)>(.*?)</translation>',mb,re.S)
            if not s or not t: continue
            attr=t.group(1)
            if 'obsolete' in attr or 'vanished' in attr: continue
            k=raw_to_key(s.group(1)); v=html.unescape(t.group(2))
            m.setdefault(k,v)
    return m

de=parse_map('Impl/ProLifeLoc/Translations/ProLife_de_DE.ts')
ru=parse_map('Impl/ProLifeLoc/Translations/ProLife_ru_RU.ts')
sv=json.load(open('sv_input.json',encoding='utf-8'))
ref={}
miss_de=0; miss_ru=0
for x in sv['strings']:
    k=x['en']
    ref[k]={"de":de.get(k),"ru":ru.get(k),"ctx":x['contexts']}
    if de.get(k) is None: miss_de+=1
    if ru.get(k) is None: miss_ru+=1
json.dump(ref, open('sv_reference.json','w',encoding='utf-8'), ensure_ascii=False, indent=1)
print("reference entries:",len(ref),"missing de:",miss_de,"missing ru:",miss_ru)
# Show samples where de kept English (keep-as-is) 
keep=[k for k in ref if ref[k]['de']==k]
print("kept-as-is in de (count):",len(keep))
print("sample kept:",keep[:25])
