TARGET = ProLifeDbPck

include($(ACFDIR)/Config/QMake/ComponentConfig.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR  -limtbase -limtdb -limtlic
LIBS += -L../../../Lib/$$COMPILER_DIR  -lprolifedb -lprolifedata

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
