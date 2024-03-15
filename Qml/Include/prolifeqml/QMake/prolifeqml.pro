TARGET = prolifeqml

include($(IMTCOREDIR)/Config/QMake/QmlControls.pri)

buildwebdir = $$PWD/../../../../Bin/web

imtcoredir = $(IMTCOREDIR)

prepareWebQml($$buildwebdir)


copyToWebDir($$PWD/../, $$buildwebdir/src)
copyToWebDir(${PROLIFEDIR}/Include/prolifestyle/Resources/html/, $$buildwebdir/Resources)
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
