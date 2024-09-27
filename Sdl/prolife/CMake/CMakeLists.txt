project(prolifesdl)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlAutoConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Sensors.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Licenses.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Accounts.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Orders.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/SensorBinding.sdl)

ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Sensors.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Sensors.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Sensors.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/prolifeSensorsSdl/prolifeSensorsSdl.qrc")
ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Licenses.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Licenses.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Licenses.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/prolifeLicensesSdl/prolifeLicensesSdl.qrc")
ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Orders.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Orders.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Orders.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/prolifeOrdersSdl/prolifeOrdersSdl.qrc")
ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Accounts.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Accounts.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Accounts.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/prolifeAccountsSdl/prolifeAccountsSdl.qrc")
ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/SensorBinding.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/SensorBinding.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/SensorBinding.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/prolifeSensorBindingSdl/prolifeSensorBindingSdl.qrc")

