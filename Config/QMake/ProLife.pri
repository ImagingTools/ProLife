include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

INCLUDEPATH += $(PROLIFEDIR)/Include
INCLUDEPATH += $(PROLIFEDIR)/Impl
INCLUDEPATH += $(PROLIFEDIR)/$$AUXINCLUDEDIR

include($(ACFDIR)/Config/QMake/ApplicationConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)
include($(IMTCOREDIR)/Config/QMake/OpenSSL.pri)
include($(IACFDIR)/Config/QMake/zlib.pri)

HEADERS =
QT += xml network sql quick qml websockets

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

DEFINES += WEB_COMPILE

LIBS += -L$(ACFDIR)/Lib/$$COMPILER_DIR -lAcfLoc
LIBS += -L$(ACFSLNDIR)/Lib/$$COMPILER_DIR -liauth -liqtgui -liservice -lAcfSlnLoc
LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR -limtbase -limtmail -limtgui -limtauth -limtauthgui -limtlicdb -limtlic -limtlicgui -lImtCoreLoc -limtwidgets -limtzip -limtrest -limtcrypt -limt3dgui -limtrepo -limtstyle -limtqml -limtdb -limtfile -limtstylecontrolsqml
LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR -limtlicgql -limtguigql -limtgql -limtauthgql -limtauthdb -limtcom -limtapp -limtclientgql -limtservice -limtlog -limtguiqml -limtcolguiqml -limtdocguiqml -limtcontrolsqml -limtauthguiqml -limtguigqlqml -limtlicguiqml -limtcol -limtbasesdl -limtcolorsdl -limtlicsdl
LIBS += -L$(PROLIFEDIR)/Lib/$$COMPILER_DIR -lprolifeqml -lprolifestyle -lProLifeLoc -lprolifedata -lprolifedb -lprolifegql -lprolifesdl
LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR -limtappsdl -limtauthsdl -lImtCoreLoc -limtservergql -limtserverapp
