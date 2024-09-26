TARGET = ProLifeSettingsPlugin

ARXC_CONFIG = $$PWD/../../../../Config/ProLife.awc
ARXC_FILES += $$PWD/../ProLifeSettingsPlugin.acc

include($(IMTCOREDIR)/Config/QMake/Plugin.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

DESTDIR = $$OUT_PWD/../../../../Bin/$$COMPILER_DIR/Plugins
OBJECTS_DIR = ../$$AUXINCLUDEPATH/GeneratedFiles/$$CONFIGURATION_NAME/"$$TARGET"

QT += sql

LIBS += -lifile -listd -lidoc
LIBS += -limtauth -limtbase -limtapp -limtdb -limtfile -limtservice

