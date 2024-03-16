TARGET = prolifeqml

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

compyleWeb($$buildwebdir, "prolife")

GENERATED_RESOURCES = $$_PRO_FILE_PWD_/../empty

include($(IMTCOREDIR)/Config/QMake/WebQrc.pri)

include($(ACFDIR)/Config/QMake/StaticConfig.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

DESTDIR = $$OUT_PWD/../../../../Lib/$$COMPILER_DIR
#AUXINCLUDEPATH = ../../../../$$AUXINCLUDEDIR

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)
