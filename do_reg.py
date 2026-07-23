import re
def rd(p): return open(p,'rb').read()
def wr(p,b): open(p,'wb').write(b)

# 1) ProLifeClient.acc (CRLF) : add sv_SE after ru_RU value
p='Impl/ProLifeClient/ProLifeClient.acc'
d=rd(p); m=re.search(rb'([ \t]*)<Value>ru_RU</Value>\r\n', d)
assert m, 'ru_RU not found in ProLifeClient.acc'
ins=m.group(1)+b'<Value>sv_SE</Value>\r\n'
assert b'<Value>sv_SE</Value>' not in d
d=d[:m.end()]+ins+d[m.end():]; wr(p,d); print('ProLifeClient.acc: +sv_SE')

# 2) .manifest (CRLF JSON) : append sv_SE to languages array
p='Include/prolifestyle/Resources/Translations/.manifest'
d=rd(p); assert b'"sv_SE"' not in d
d2=d.replace(b'"de_DE"]', b'"de_DE", "sv_SE"]')
assert d2!=d, 'manifest pattern not found'; wr(p,d2); print('.manifest: +sv_SE')

# 3) ReleaseTranslations.bat (CRLF): append lrelease line
p='Impl/ProLifeLoc/Translations/ReleaseTranslations.bat'
d=rd(p); assert b'ProLife_sv_SE.ts' not in d
if not d.endswith(b'\r\n'): d+=b'\r\n'
d+=b'%QTDIR%\\bin\\lrelease.exe ProLife_sv_SE.ts\r\n'; wr(p,d); print('ReleaseTranslations.bat: +sv_SE')

# 4) UpdateAllTranlsations.bat (CRLF): append call line
p='Impl/ProLifeLoc/Translations/UpdateAllTranlsations.bat'
d=rd(p); assert b'Update_sv_SE.bat' not in d
if not d.endswith(b'\r\n'): d+=b'\r\n'
d+=b'call Update_sv_SE.bat\r\n'; wr(p,d); print('UpdateAllTranlsations.bat: +call')

# 5) Update_sv_SE.bat (CRLF): create new, ru_RU pattern
p='Impl/ProLifeLoc/Translations/Update_sv_SE.bat'
line=b'%QTDIR%\\bin\\lupdate.exe ..\\..\\..\\Include ..\\..\\..\\AuxInclude ..\\..\\..\\Qml -target-language sv_SE -ts ProLife_sv_SE.ts -recursive -locations relative -no-obsolete\r\n'
wr(p,line); print('Update_sv_SE.bat: created')

# 6) prolifeqmlJsWeb.qrc (CRLF): add ProLife_sv_SE.ts after en_US ProLife line
p='Include/prolifestyle/Resources/html/prolifeqmlJsWeb.qrc'
d=rd(p); assert b'ProLife_sv_SE.ts' not in d
m=re.search(rb'([ \t]*)<file alias="ProLife_en_US\.ts">Translations/ProLife_en_US\.ts</file>\r\n', d)
assert m, 'ProLife_en_US.ts line not found'
ins=m.group(1)+b'<file alias="ProLife_sv_SE.ts">Translations/ProLife_sv_SE.ts</file>\r\n'
d=d[:m.end()]+ins+d[m.end():]; wr(p,d); print('prolifeqmlJsWeb.qrc: +sv_SE')
