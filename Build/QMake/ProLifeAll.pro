# Root of ProLife project
TEMPLATE = subdirs

# Libraries
SUBDIRS += prolifedata
prolifedata.file = ../../Include/prolifedata/QMake/prolifedata.pro

SUBDIRS += prolifedb
prolifedb.file = ../../Include/prolifedb/QMake/prolifedb.pro

SUBDIRS += prolifegql
prolifegql.file = ../../Include/prolifegql/QMake/prolifegql.pro

SUBDIRS += prolifeqml
prolifeqml.file = ../../Include/prolifeqml/QMake/prolifeqml.pro

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
ProLifeClient.depends = ProLifeLoc

SUBDIRS += ProLifeServer
ProLifeServer.file = ../../Impl/ProLifeServer/QMake/ProLifeServer.pro
ProLifeServer.depends = ProLifeLoc

SUBDIRS += ProLifeClientServer
ProLifeClientServer.file = ../../Impl/ProLifeClientServer/QMake/ProLifeClientServer.pro
ProLifeClientServer.depends = ProLifeLoc

SUBDIRS += ProLifeServerConfigurator
ProLifeServerConfigurator.file = ../../Impl/ProLifeServerConfigurator/QMake/ProLifeServerConfigurator.pro
ProLifeServerConfigurator.depends = ProLifeLoc


