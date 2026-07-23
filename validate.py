import re, html, json, sys
import xml.dom.minidom as minidom

# Usage: python validate.py <ts_file> [<ts_file> ...]
def check(path):
    content=open(path,encoding='utf-8').read()
    errs=[]
    # 1) XML well-formed
    try:
        minidom.parseString(content.encode('utf-8'))
    except Exception as e:
        errs.append(f"XML PARSE ERROR: {e}")
        return errs
    # 2) no unfinished markers
    nun=content.count('type="unfinished"')
    if nun: errs.append(f"{nun} unfinished markers remain")
    # 3) scan messages for empty active translations & placeholder mismatch
    empty=0; ph=0; ph_examples=[]
    for cm in re.finditer(r'<context>(.*?)</context>', content, re.S):
        cbody=cm.group(1)
        name=re.search(r'<name>(.*?)</name>',cbody,re.S).group(1)
        for m in re.finditer(r'<message[^>]*>(.*?)</message>', cbody, re.S):
            mb=m.group(1)
            s=re.search(r'<source>(.*?)</source>',mb,re.S)
            t=re.search(r'<translation([^>]*)>(.*?)</translation>',mb,re.S)
            if not t: continue
            attr,txt=t.group(1),t.group(2)
            if 'type="obsolete"' in attr or 'type="vanished"' in attr:
                continue
            if txt.strip()=='':
                empty+=1
                if empty<=10: errs.append(f"EMPTY translation in [{name}] source={s.group(1)[:40] if s else '?'!r}")
            # placeholder check
            if s:
                src=s.group(1)
                src_ph=set(re.findall(r'%\d+', src))
                tr_ph=set(re.findall(r'%\d+', txt))
                if src_ph!=tr_ph:
                    ph+=1
                    if len(ph_examples)<15:
                        ph_examples.append((name,src,txt,src_ph,tr_ph))
    if empty: errs.append(f"TOTAL EMPTY active translations: {empty}")
    if ph:
        errs.append(f"PLACEHOLDER mismatches: {ph}")
        for name,src,txt,sp,tp in ph_examples:
            errs.append(f"   [{name}] src_ph={sorted(sp)} tr_ph={sorted(tp)} src={src[:50]!r} tr={txt[:50]!r}")
    return errs

allok=True
for p in sys.argv[1:]:
    e=check(p)
    if e:
        allok=False
        print(f"=== {p}: {len(e)} issue(s) ===")
        for x in e: print("  -",x)
    else:
        print(f"=== {p}: OK (well-formed, no unfinished, no empty, placeholders preserved) ===")
sys.exit(0 if allok else 1)
