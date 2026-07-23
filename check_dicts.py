import json, re, sys
inp=json.load(open('translate_input.json',encoding='utf-8'))
keys=[x['en'] for x in inp['strings']]
keyset=set(keys)
ok=True
for lang in ['fr_FR','it_IT','jp_JP','pl_PL','zh_ZH']:
    try:
        d=json.load(open(f'dict_{lang}.json',encoding='utf-8'))
    except Exception as e:
        print(f"{lang}: LOAD ERROR {e}"); ok=False; continue
    dk=set(d.keys())
    missing=keyset-dk
    extra=dk-keyset
    empty=[k for k in keys if k in d and (d[k] is None or str(d[k]).strip()=='')]
    # placeholder check
    phbad=[]
    for k in keys:
        if k in d and d[k] is not None:
            sp=set(re.findall(r'%\d+',k)); tp=set(re.findall(r'%\d+',str(d[k])))
            if sp!=tp: phbad.append((k,d[k],sorted(sp),sorted(tp)))
    # identical-to-English ratio (sanity: too many identical = under-translated)
    ident=[k for k in keys if k in d and str(d[k])==k and len(k)>3 and not re.match(r'^[A-Za-z0-9 _/:.\-]+$',k) is None]
    # count identical excluding obvious keep-as-is (short codes)
    identical=sum(1 for k in keys if k in d and str(d[k])==k)
    print(f"{lang}: keys={len(dk)} missing={len(missing)} extra={len(extra)} empty={len(empty)} ph_mismatch={len(phbad)} identical_to_en={identical}")
    if missing:
        print("   MISSING sample:", list(missing)[:10]); ok=False
    if empty:
        print("   EMPTY sample:", empty[:10]); ok=False
    if phbad:
        print("   PH MISMATCH sample:", phbad[:8]); ok=False
print("ALL DICTS OK" if ok else "ISSUES FOUND")
