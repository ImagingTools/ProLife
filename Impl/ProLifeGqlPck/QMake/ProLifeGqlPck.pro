TARGET = ProLifeGqlPck

include($(ACFDIR)/Config/QMake/ComponentConfig.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR -limtguigql -limtbase -limtgql -limtgui -limtwidgets -limtlic -limtdb -limtauth
LIBS += -L$(PROLIFEDIR)/Lib/$$COMPILER_DIR -lprolifegql -lprolifedata -lprolifesdl

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
