import re, json, sys
sys.path.insert(0,'.')
from tslib import xml_escape, raw_to_key

d=json.load(open('dict_sv_SE.json',encoding='utf-8'))
content=open('Impl/ProLifeLoc/Translations/ProLife_en_US.ts',encoding='utf-8').read()

# 1) language attribute
assert '<TS version="2.1" language="en_US">' in content
content=content.replace('<TS version="2.1" language="en_US">','<TS version="2.1" language="sv_SE">')

missing=set(); dropped=0; replaced=0
MSG=re.compile(r'([ \t]*)<message>(.*?)</message>\n', re.S)
def f(m):
    global dropped, replaced
    indent, body = m.group(1), m.group(2)
    if 'type="obsolete"' in body or 'type="vanished"' in body:
        dropped+=1
        return ''
    srcm=re.search(r'<source>(.*?)</source>', body, re.S)
    key=raw_to_key(srcm.group(1))
    if key not in d:
        missing.add(key); 
        return m.group(0)
    val=xml_escape(d[key])
    def repl_tr(t): return '<translation>'+val+'</translation>'
    body2=re.sub(r'<translation>.*?</translation>', repl_tr, body, count=1, flags=re.S)
    replaced+=1
    return indent+'<message>'+body2+'</message>\n'
content=MSG.sub(f, content)

# 2) remove now-empty contexts (only <name>)
EMPTY=re.compile(r'[ \t]*<context>\s*<name>[^<]*</name>\s*</context>\n', re.S)
before=content
content=EMPTY.sub('', content)
empctx = before.count('<context>') - content.count('<context>')

open('Impl/ProLifeLoc/Translations/ProLife_sv_SE.ts','w',encoding='utf-8').write(content)
print(f"replaced={replaced} dropped_obsolete={dropped} empty_contexts_removed={empctx} missing={len(missing)}")
for k in list(missing)[:20]: print("  MISSING:",repr(k))
