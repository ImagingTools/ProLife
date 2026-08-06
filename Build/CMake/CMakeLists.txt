cmake_minimum_required(VERSION 3.26)

# Version placeholder - updated by release workflow
project(ProLifeAll VERSION 1.0.1)

if(NOT DEFINED QT_VERSION_MAJOR)
	find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core Widgets QuickWidgets Core Gui Xml Network Svg Sql WebSockets Qml Concurrent QuickControls2 PrintSupport REQUIRED)
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

# Configure paths to dependent repositories
# Priority: 1. CMake variable, 2. Environment variable
# Dependencies must be provided via environment variables or CMake variables

if(NOT DEFINED IMTCOREDIR)
	set(IMTCOREDIR "$ENV{IMTCOREDIR}")
	if("${IMTCOREDIR}" STREQUAL "")
		message(FATAL_ERROR "IMTCOREDIR not set. Please set IMTCOREDIR environment variable or CMake variable.")
	endif()
endif()

if(NOT DEFINED PUMADIR)
	set(PUMADIR "$ENV{PUMADIR}")
	if("${PUMADIR}" STREQUAL "")
		message(FATAL_ERROR "PUMADIR not set. Please set PUMADIR environment variable or CMake variable.")
	endif()
endif()

if(NOT DEFINED LISADIR)
	set(LISADIR "$ENV{LISADIR}")
	if("${LISADIR}" STREQUAL "")
		message(FATAL_ERROR "LISADIR not set. Please set LISADIR environment variable or CMake variable.")
	endif()
endif()

if(NOT DEFINED ACFDIR)
	set(ACFDIR "$ENV{ACFDIR}")
	if("${ACFDIR}" STREQUAL "")
		message(FATAL_ERROR "ACFDIR not set. Please set ACFDIR environment variable or CMake variable.")
	endif()
endif()

if(NOT DEFINED ACFSLNDIR)
	set(ACFSLNDIR "$ENV{ACFSLNDIR}")
	if("${ACFSLNDIR}" STREQUAL "")
		message(FATAL_ERROR "ACFSLNDIR not set. Please set ACFSLNDIR environment variable or CMake variable.")
	endif()
endif()

if(NOT DEFINED PROLIFEDIR)
	set(PROLIFEDIR "$ENV{PROLIFEDIR}")
	if("${PROLIFEDIR}" STREQUAL "")
		get_filename_component(PROLIFEDIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
		message(STATUS "PROLIFEDIR not set, using: ${PROLIFEDIR}")
	endif()
endif()

if(NOT DEFINED IMTCOREDIR_BUILD)
	set(IMTCOREDIR_BUILD "$ENV{IMTCOREDIR}")
	if("${IMTCOREDIR_BUILD}" STREQUAL "")
		set(IMTCOREDIR_BUILD "${IMTCOREDIR}")
	endif()
endif()

include(${IMTCOREDIR}/Config/CMake/ImtCoreEnv.cmake)
include(${IMTCOREDIR}/Config/CMake/PartituraUpgrade.cmake)
imt_core_upgrade_partitura("${CMAKE_CURRENT_LIST_DIR}/../..")

get_filename_component(INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Include" ABSOLUTE)
get_filename_component(IMPL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Impl" ABSOLUTE)

set(AUX_INCLUDE_DIR "${PROLIFEDIR}/AuxInclude/${TARGETNAME}/GeneratedFiles")

include_directories("${AUX_INCLUDE_DIR}/../")
include_directories("${AUX_INCLUDE_DIR}/")

set(ACF_QT_MODULE_LINK_SCOPE "")
set(ACF_LIBRARY_LINK_SCOPE "")
set(ACF_PACKAGE_LINK_SCOPE "")
set(ACF_APPLICATION_LINK_SCOPE "")

# Dependencies are provided via environment variables
message(STATUS "Using libraries from environment variables")

# Sdl
include("${CMAKE_CURRENT_LIST_DIR}/../../../ImtCore/Config/CMake/ImtCoreSdlCustomConfig.cmake")
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
add_subdirectory("${IMPL_DIR}/ProLifeServerTest/CMake" "${AUX_INCLUDE_DIR}/ProLifeServerTest")
add_subdirectory("${IMPL_DIR}/ProLifeDataPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeDataPck")
add_dependencies(ProLifeDataPck prolifedata)

add_subdirectory("${IMPL_DIR}/ProLifeDbPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeDbPck")
add_dependencies(ProLifeDbPck prolifedb)

add_subdirectory("${IMPL_DIR}/ProLifeGqlPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeGqlPck")
add_dependencies(ProLifeGqlPck prolifegql)

# add_subdirectory("${IMPL_DIR}/ProLifeConverter/CMake" "${AUX_INCLUDE_DIR}/ProLifeConverter")
add_subdirectory("${IMPL_DIR}/ProLifeServerConfigurator/CMake" "${AUX_INCLUDE_DIR}/ProLifeServerConfigurator")

# Declare target-based inter-library dependencies after all targets
include("${PROLIFEDIR}/Config/CMake/ProLifeLibraryDependencies.cmake")

file(COPY ../Migrations DESTINATION ${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})

