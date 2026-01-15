@echo on
for /f %%i in ('..\..\..\..\ImtCore\3rdParty\Python\3.8\python.exe ..\..\..\..\ImtCore\Build\GetSvnVersion.py -D ..\..\..\') do set APP_VERSION=%%i

set PROLIFE_BUILD_DIR=..\..\..\Bin\Release_Qt6_VC17_x64
iscc ProLifeServer.iss

