cmake_minimum_required(VERSION 3.15)

project(ProLifeAll)

if(NOT DEFINED QT_VERSION_MAJOR)
	find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core Widgets QuickWidgets Core Gui Xml Network Svg Sql WebSockets Qml Concurrent QuickControls2 REQUIRED)
	if(QT_VERSION_MAJOR EQUAL 6)
		find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core5Compat REQUIRED)
	endif()
endif()

if(NOT DEFINED WEB_COMPILE)
	set(WEB_COMPILE ON CACHE BOOL "Build whith web compatibility")
endif()

if(NOT DEFINED BUILDDIR)
	get_filename_component(BUILDDIR "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
else()
	get_filename_component(BUILDDIR "${BUILDDIR}" ABSOLUTE)
endif()

if(NOT DEFINED IMTCOREDIR)
	set(IMTCOREDIR "$ENV{IMTCOREDIR}")
endif()

if(NOT DEFINED PUMADIR)
	set(PUMADIR "$ENV{PUMADIR}")
endif()

if(NOT DEFINED PROLIFEDIR)
	set(PROLIFEDIR "$ENV{PROLIFEDIR}")
endif()

include(${IMTCOREDIR}/Config/CMake/ImtCoreEnv.cmake)

get_filename_component(INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Include" ABSOLUTE)
get_filename_component(IMPL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Impl" ABSOLUTE)

set(AUX_INCLUDE_DIR "${BUILDDIR}/ProLife/AuxInclude/${TARGETNAME}/GeneratedFiles")

include_directories("${AUX_INCLUDE_DIR}/../")

# Sdl
include("${CMAKE_CURRENT_LIST_DIR}/../../../ImtCore/Config/CMake/ImtCoreSdlCustomConfig.cmake")
# initialize SDL
if (NOT IMT_DevAll_BUILD)
	get_filename_component(CACHE_BASE_DIR_PATH "${IMTCOREDIR}/AuxInclude/${TARGETNAME}" ABSOLUTE)
	ImtCoreAddSdlSearchPath("${CACHE_BASE_DIR_PATH}/GeneratedFiles")
	ImtCoreAddSdlSearchPath("${AUX_INCLUDE_DIR}")
	ImtCoreSetSdlMainCacheFile("${BUILDDIR}/ProLife/AuxInclude/${TARGETNAME}/SDL_CACHE/cache.jcache")
	ImtCoreAddSdlCacheFile("${CACHE_BASE_DIR_PATH}/SDL_CACHE/cache.jcache")
	make_directory("${BUILDDIR}/ProLife/AuxInclude/${TARGETNAME}/SDL_CACHE")
endif()
add_subdirectory("${INCLUDE_DIR}/../Sdl/prolife/CMake" "${AUX_INCLUDE_DIR}/prolifesdl")

# Include
add_subdirectory("${INCLUDE_DIR}/prolifedata/CMake" "${AUX_INCLUDE_DIR}/prolifedata")
add_subdirectory("${INCLUDE_DIR}/prolifedb/CMake" "${AUX_INCLUDE_DIR}/prolifedb")

add_subdirectory("${INCLUDE_DIR}/prolifegql/CMake" "${AUX_INCLUDE_DIR}/prolifegql")
add_dependencies(prolifegql prolifesdl)

add_subdirectory("${INCLUDE_DIR}/../Qml/Include/prolifeqml/CMake" "${AUX_INCLUDE_DIR}/prolifeqml")
add_dependencies(prolifeqml prolifesdl)

add_subdirectory("${INCLUDE_DIR}/prolifestyle/CMake" "${AUX_INCLUDE_DIR}/prolifestyle")

# Tests
#add_subdirectory("${INCLUDE_DIR}/prolifetest/CMake" "${AUX_INCLUDE_DIR}/prolifetest")

# Plug-ins
add_subdirectory("${IMPL_DIR}/Plugins/ProLifeSettingsPlugin/CMake" "${AUX_INCLUDE_DIR}/ProLifeSettingsPlugin")

# Impl
add_subdirectory("${IMPL_DIR}/ProLifeLoc/CMake" "${AUX_INCLUDE_DIR}/ProLifeLoc")
add_subdirectory("${IMPL_DIR}/ProLifeClient/CMake" "${AUX_INCLUDE_DIR}/ProLifeClient")
add_subdirectory("${IMPL_DIR}/ProLifeServer/CMake" "${AUX_INCLUDE_DIR}/ProLifeServer")
add_subdirectory("${IMPL_DIR}/ProLifeDataPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeDataPck")
add_dependencies(ProLifeDataPck prolifedata)

add_subdirectory("${IMPL_DIR}/ProLifeDbPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeDbPck")
add_dependencies(ProLifeDbPck prolifedb)

add_subdirectory("${IMPL_DIR}/ProLifeGqlPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeGqlPck")
add_dependencies(ProLifeGqlPck prolifegql)

# add_subdirectory("${IMPL_DIR}/ProLifeConverter/CMake" "${AUX_INCLUDE_DIR}/ProLifeConverter")
add_subdirectory("${IMPL_DIR}/ProLifeServerConfigurator/CMake" "${AUX_INCLUDE_DIR}/ProLifeServerConfigurator")

file(COPY ../Migrations DESTINATION ${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
file(COPY ${IMTCOREDIR}/Install/Database/LogMigrations DESTINATION ${PROJECT_SOURCE_DIR}/../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})

