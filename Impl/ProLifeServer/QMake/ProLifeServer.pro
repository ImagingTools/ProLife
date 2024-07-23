TARGET = ProLifeServer

include($(ACFDIR)/Config/QMake/ApplicationConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)
include($(IMTCOREDIR)/Config/QMake/OpenSSL.pri)
include($(PROLIFEDIR)/Config/QMake/ProLife.pri)

HEADERS =
QT += xml network sql quick qml

#INCLUDEPATH += ../../../Include
#INCLUDEPATH += ../../../Impl
#INCLUDEPATH +=  $$AUXINCLUDEDIR

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

DEFINES += WEB_COMPILE

LIBS += -L../../../Lib/$$COMPILER_DIR -liauth -liqtgui -liservice
LIBS += -limtbase -limtgui -limtauth -limtauthgui -limtlicdb -limtlic -limtlicgui -lImtCoreLoc -limtwidgets -limtzip -limtrest -limtcrypt -limt3dgui -limtrepo -limtstyle -limtqml -limtdb -limtfile -limtstylecontrolsqml
LIBS += -limtlicgql -limtguigql -limtgql -limtauthgql -limtauthdb -limtcom -limtapp -limtclientgql -limtservice -limtlog
LIBS += -lprolifeqml -lprolifestyle -lImtCoreLoc -lProLifeLoc -lprolifedata -lprolifedb -lprolifegql

# Set OS-specific build options:
win32-msvc*{
        QMAKE_CXXFLAGS += /wd4264

        # copying all Qt DLLs to destination directory
        greaterThan(QT_MAJOR_VERSION, 4): QMAKE_POST_LINK = set path=$(QTDIR)\bin;%path% && $(QTDIR)\bin\windeployqt --qmldir=$(IMTCOREDIR)/Qml $$DESTDIR
}

# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $$PWD/../../../Config/ProLife.awc
ARXC_FILES += $$PWD/../ProLifeServer.acc
ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET

# Conversion of resource templates:
#win*{
#	# File transformation
#	ACF_CONVERT_FILES = $$PWD/../VC/ProLifeServer.rc.xtracf
#	ACF_CONVERT_OUTDIR = $$AUXINCLUDEPATH/GeneratedFiles/$$TARGET
#	ACF_CONVERT_REGISTRY =  $$PWD/../VC/FileSubstitCopyApp.acc
#	ACF_CONVERT_CONFIG = $$PWD/../../../Config/BaseOnly.awc

#    RC_FILE = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET/ProLifeServer.rc
#	RC_INCLUDEPATH = $$_PRO_FILE_PWD_
#}

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
include($(ACFDIR)/Config/QMake/CustomBuild.pri)


