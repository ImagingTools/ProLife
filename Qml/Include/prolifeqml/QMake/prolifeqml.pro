TARGET = prolifeqml

include($(ACFDIR)/Config/QMake/GeneralConfig.pri)
include($(IMTCOREDIR)/Config/QMake/QmlControls.pri)

buildwebdir = $$PWD/../../../../Bin/web

imtcoredir = $(IMTCOREDIR)
prolifedir = $(PROLIFEDIR)

prepareWebQml($$buildwebdir)

# copy project qml from to
copyToWebDir($$PWD/../, $$buildwebdir/src)
copyToWebDir($$prolifedir/Include/prolifestyle/Resources/html/, $$buildwebdir/Resources)
copyToWebDir($$imtcoredir/Include/imtstylecontrolsqml/Qml/Fonts/, $$buildwebdir/Resources)
copyToWebDir($$imtcoredir/Include/imtstylecontrolsqml/Qml/Acf/, $$buildwebdir/src/Acf)

# copy translations
copyToWebDir($$PWD/../../../../Impl/ProLifeLoc/Translations/, $$buildwebdir/Resources/Translations)
copyToWebDir($$imtcoredir/Impl/ImtCoreLoc/Translations/, $$buildwebdir/Resources/Translations)

copyToWebDir($$prolifedir/$$AUXINCLUDEDIR/GeneratedFiles/prolifesdl/SDL/1.0/QML/prolifeAccountsSdl, $$buildwebdir/src/prolifeAccountsSdl)
copyToWebDir($$prolifedir/$$AUXINCLUDEDIR/GeneratedFiles/prolifesdl/SDL/1.0/QML/prolifeSensorsSdl, $$buildwebdir/src/prolifeSensorsSdl)
copyToWebDir($$prolifedir/$$AUXINCLUDEDIR/GeneratedFiles/prolifesdl/SDL/1.0/QML/prolifeLicensesSdl, $$buildwebdir/src/prolifeLicensesSdl)
copyToWebDir($$prolifedir/$$AUXINCLUDEDIR/GeneratedFiles/prolifesdl/SDL/1.0/QML/prolifeOrdersSdl, $$buildwebdir/src/prolifeOrdersSdl)

compyleWeb($$buildwebdir, "prolife")

GENERATED_RESOURCES = $$_PRO_FILE_PWD_/../empty

include($(IMTCOREDIR)/Config/QMake/WebQrc.pri)

include($(ACFDIR)/Config/QMake/StaticConfig.pri)
DESTDIR = $$OUT_PWD/../../../../Lib/$$COMPILER_DIR

include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)
