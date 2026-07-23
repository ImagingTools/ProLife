import re, sys
sys.path.insert(0,'.')
def parse(path):
    content=open(path,encoding='utf-8').read()
    ctxs=[]
    for cm in re.finditer(r'<context>(.*?)</context>', content, re.S):
        cb=cm.group(1); name=re.search(r'<name>(.*?)</name>',cb,re.S).group(1)
        msgs=[]
        for m in re.finditer(r'<message[^>]*>(.*?)</message>', cb, re.S):
            mb=m.group(1)
            s=re.search(r'<source>(.*?)</source>',mb,re.S)
            t=re.search(r'<translation([^>]*)>(.*?)</translation>',mb,re.S)
            attr=t.group(1) if t else ''
            msgs.append((s.group(1) if s else None, attr))
        ctxs.append((name,msgs))
    return ctxs

en=parse('Impl/ProLifeLoc/Translations/ProLife_en_US.ts')
# active = not obsolete/vanished
active_ctx=[]
n_active=0; n_obs=0
for name,msgs in en:
    act=[(s,a) for (s,a) in msgs if 'obsolete' not in a and 'vanished' not in a]
    obs=[(s,a) for (s,a) in msgs if 'obsolete' in a or 'vanished' in a]
    n_active+=len(act); n_obs+=len(obs)
    if act:
        active_ctx.append((name,act))
print(f"en_US: total contexts={len(en)}, active messages={n_active}, obsolete/vanished={n_obs}, contexts w/active={len(active_ctx)}")
# contexts that are entirely obsolete (dropped)
dropped=[name for name,msgs in en if all('obsolete' in a or 'vanished' in a for s,a in msgs)]
print("contexts fully obsolete/vanished (dropped):",dropped)
# contexts with MIXED (some active some obsolete)
mixed=[name for name,msgs in en if any('obsolete' in a or 'vanished' in a for s,a in msgs) and any('obsolete' not in a and 'vanished' not in a for s,a in msgs)]
print("contexts MIXED active+obsolete:",mixed)

de=parse('Impl/ProLifeLoc/Translations/ProLife_de_DE.ts')
print(f"de_DE: contexts={len(de)}, messages={sum(len(m) for _,m in de)}")
# Compare active en context names to de context names
en_names=[n for n,_ in active_ctx]; de_names=[n for n,_ in de]
print("active en context names == de names (as sets)?", set(en_names)==set(de_names))
print("in en-active not de:", set(en_names)-set(de_names))
print("in de not en-active:", set(de_names)-set(en_names))
