TARGET = prolifesdl

include($(ACFDIR)/Config/QMake/StaticConfig.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)


# SDL
SDL_SCHEMES_LIST = $$PWD/../1.0/Sensors.sdl \
					$$PWD/../1.0/Licenses.sdl \
					$$PWD/../1.0/Orders.sdl \
					$$PWD/../1.0/Accounts.sdl

include($(IMTCOREDIR)/Config/QMake/SdlConfiguration.pri)



