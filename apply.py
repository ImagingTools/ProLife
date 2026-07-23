import re, html, json, sys
sys.path.insert(0,'.')
from tslib import xml_escape, raw_to_key

# Usage: python apply.py <ts_file> <dict_json> [--report]
ts_path=sys.argv[1]; dict_path=sys.argv[2]
d=json.load(open(dict_path,encoding='utf-8'))

content=open(ts_path,encoding='utf-8').read()

missing=set(); applied=0; already=0

MSG_RE=re.compile(r'(<message[^>]*>)(.*?)(</message>)', re.S)
SRC_RE=re.compile(r'<source>(.*?)</source>', re.S)
TR_RE=re.compile(r'<translation([^>]*)>(.*?)</translation>', re.S)

def repl_msg(m):
    global applied, already
    head, body, tail = m.group(1), m.group(2), m.group(3)
    tr=TR_RE.search(body)
    if not tr:
        return m.group(0)
    attr=tr.group(1)
    if 'unfinished' not in attr:
        return m.group(0)
    src=SRC_RE.search(body)
    if not src:
        return m.group(0)
    key=raw_to_key(src.group(1))
    if key not in d or d[key] is None or d[key]=='':
        missing.add(key)
        return m.group(0)
    val=xml_escape(d[key])
    # Rebuild translation element, preserving any non-type attributes? 'unfinished' entries here only have type attr.
    # Preserve other attributes (e.g., variants) if present, but drop type="unfinished".
    new_attr=re.sub(r'\s*type="unfinished"','',attr)
    new_tr=f'<translation{new_attr}>{val}</translation>'
    new_body=body[:tr.start()]+new_tr+body[tr.end():]
    applied+=1
    return head+new_body+tail

new_content=MSG_RE.sub(repl_msg, content)

if '--report' in sys.argv:
    print(f"{ts_path}: would apply={applied}, missing_keys={len(missing)}")
    for k in list(missing)[:50]:
        print("  MISSING:", repr(k))
else:
    open(ts_path,'w',encoding='utf-8').write(new_content)
    print(f"{ts_path}: applied={applied}, missing_keys={len(missing)}")
    for k in list(missing)[:50]:
        print("  MISSING:", repr(k))
