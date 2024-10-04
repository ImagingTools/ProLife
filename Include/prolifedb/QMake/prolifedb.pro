TARGET = prolifedb

include($(ACFDIR)/Config/QMake/StaticConfig.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

QT += sql

LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR -limtbase -limtlic
