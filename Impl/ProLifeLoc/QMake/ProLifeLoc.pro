TARGET = ProLifeLoc

include($(ACFDIR)/Config/QMake/StaticConfig.pri)

HEADERS =
SOURCES =
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

TRANSLATIONS += $$files($$_PRO_FILE_PWD_/../Translations/*.ts, false)

# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $$PWD/../../../Config/ProLife.awc
ARXC_FILES += $$files($$_PRO_FILE_PWD_/../*.acc, false)
ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET

include($(ACFDIR)/Config/QMake/CustomBuild.pri)
