import re, sys
sys.path.insert(0,'.')
from tslib import xml_escape, raw_to_key

WEB={
 'Name':'Namn','Description':'Beskrivning','Added':'Tillagd','Last Modified':'Senast ändrad',
 'Account Name':'Kontonamn','Account Type':'Kontotyp','Account Description':'Kontobeskrivning',
 'Customer':'Kund','Email':'E-post','Instance-ID':'Instans-ID','Feature Name':'Funktionsnamn',
 'Feature-ID':'Funktions-ID','License Name':'Licensnamn','License-ID':'Licens-ID','Packages':'Paket',
 'Products':'Produkter','Accounts':'Konton','Installations':'Installationer','New':'Ny','Remove':'Ta bort',
 'Edit':'Redigera','Save':'Spara','Duplicate':'Duplicera','Close':'Stäng','Rename':'Byt namn',
 'Set Description':'Ange beskrivning','Apply':'Verkställ','Mode':'Läge','Design scheme':'Designschema',
 'Design':'Design','Language':'Språk','DB Settings':'Databasinställningar','General':'Allmänt',
 'Lisa Settings':'Lisa-inställningar','Network Settings':'Nätverksinställningar','Dark':'Mörkt','Light':'Ljust',
 'Russian':'Ryska','English':'Engelska','Polish':'Polska','German':'Tyska','Server Url':'Server-URL',
 'DB Name':'Databasnamn','Host':'Värd','Password':'Lösenord','Port':'Port','User name':'Användarnamn',
}
src='Include/prolifestyle/Resources/Translations/ProLife_en_US.ts'
content=open(src,encoding='utf-8').read()
assert '<TS language="en_US" version="2.0">' in content
content=content.replace('<TS language="en_US" version="2.0">','<TS language="sv_SE" version="2.0">')

missing=set(); cnt=0
PAT=re.compile(r'(<source>(.*?)</source>\s*<translation>)(.*?)(</translation>)', re.S)
def f(m):
    global cnt
    key=raw_to_key(m.group(2))
    if key not in WEB:
        missing.add(key); return m.group(0)
    cnt+=1
    return m.group(1)+xml_escape(WEB[key])+m.group(4)
content=PAT.sub(f,content)
open('Include/prolifestyle/Resources/Translations/ProLife_sv_SE.ts','w',encoding='utf-8').write(content)
print('translated messages:',cnt,'missing:',len(missing),list(missing)[:10])
