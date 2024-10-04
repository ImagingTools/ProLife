# Root of ProLife project
TEMPLATE = subdirs

# Libraries
SUBDIRS += prolifeqml
prolifeqml.file = ../../Qml/Include/prolifeqml/QMake/prolifeqml.pro

SUBDIRS += prolifesdl
prolifesdl.file = ../../Sdl/prolife/QMake/prolifesdl.pro

SUBDIRS += prolifedata
prolifedata.file = ../../Include/prolifedata/QMake/prolifedata.pro

SUBDIRS += prolifedb
prolifedb.file = ../../Include/prolifedb/QMake/prolifedb.pro

SUBDIRS += prolifegql
prolifegql.file = ../../Include/prolifegql/QMake/prolifegql.pro

SUBDIRS += prolifestyle
prolifestyle.file = ../../Include/prolifestyle/QMake/prolifestyle.pro

SUBDIRS += ProLifeLoc
ProLifeLoc.file = ../../Impl/ProLifeLoc/QMake/ProLifeLoc.pro

SUBDIRS += ProLifeDataPck
ProLifeDataPck.file = ../../Impl/ProLifeDataPck/QMake/ProLifeDataPck.pro
ProLifeDataPck.depends = prolifedata

SUBDIRS += ProLifeDbPck
ProLifeDbPck.file = ../../Impl/ProLifeDbPck/QMake/ProLifeDbPck.pro
ProLifeDbPck.depends = prolifedb

SUBDIRS += ProLifeGqlPck
ProLifeGqlPck.file = ../../Impl/ProLifeGqlPck/QMake/ProLifeGqlPck.pro
ProLifeGqlPck.depends = prolifegql


# Application
SUBDIRS += ProLifeClient
ProLifeClient.file = ../../Impl/ProLifeClient/QMake/ProLifeClient.pro
ProLifeClient.depends = ProLifeLoc prolifestyle prolifeqml

SUBDIRS += ProLifeServer
ProLifeServer.file = ../../Impl/ProLifeServer/QMake/ProLifeServer.pro
# ProLifeServer.depends = ProLifeLoc prolifestyle prolifeqml

SUBDIRS += ProLifeServerConfigurator
ProLifeServerConfigurator.file = ../../Impl/ProLifeServerConfigurator/QMake/ProLifeServerConfigurator.pro
ProLifeServerConfigurator.depends = ProLifeLoc

# Plug-ins
SUBDIRS += ProLifeSettingsPlugin
ProLifeSettingsPlugin.file = ../../Impl/Plugins/ProLifeSettingsPlugin/QMake/ProLifeSettingsPlugin.pro


