cmake_minimum_required(VERSION 3.26)

# Version placeholder - updated by release workflow
project(ProLifeAll VERSION 1.0.1)

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

# Configure paths to dependent repositories
# Priority: 1. CMake variable, 2. Environment variable, 3. Submodule path

if(NOT DEFINED IMTCOREDIR)
	set(IMTCOREDIR "$ENV{IMTCOREDIR}")
	if("${IMTCOREDIR}" STREQUAL "")
		get_filename_component(IMTCOREDIR "${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/ImtCore" ABSOLUTE)
		message(STATUS "IMTCOREDIR not set, using submodule: ${IMTCOREDIR}")
	endif()
endif()

if(NOT DEFINED PUMADIR)
	set(PUMADIR "$ENV{PUMADIR}")
	if("${PUMADIR}" STREQUAL "")
		get_filename_component(PUMADIR "${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/Puma" ABSOLUTE)
		message(STATUS "PUMADIR not set, using submodule: ${PUMADIR}")
	endif()
endif()

if(NOT DEFINED LISADIR)
	set(LISADIR "$ENV{LISADIR}")
	if("${LISADIR}" STREQUAL "")
		get_filename_component(LISADIR "${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/Lisa" ABSOLUTE)
		message(STATUS "LISADIR not set, using submodule: ${LISADIR}")
	endif()
endif()

if(NOT DEFINED ACFDIR)
	set(ACFDIR "$ENV{ACFDIR}")
	if("${ACFDIR}" STREQUAL "")
		get_filename_component(ACFDIR "${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/Acf" ABSOLUTE)
		message(STATUS "ACFDIR not set, using submodule: ${ACFDIR}")
	endif()
endif()

if(NOT DEFINED ACFSLNDIR)
	set(ACFSLNDIR "$ENV{ACFSLNDIR}")
	if("${ACFSLNDIR}" STREQUAL "")
		get_filename_component(ACFSLNDIR "${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/AcfSln" ABSOLUTE)
		message(STATUS "ACFSLNDIR not set, using submodule: ${ACFSLNDIR}")
	endif()
endif()

if(NOT DEFINED AGENTINODIR)
	set(AGENTINODIR "$ENV{AGENTINODIR}")
	if("${AGENTINODIR}" STREQUAL "")
		get_filename_component(AGENTINODIR "${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/Agentino" ABSOLUTE)
		message(STATUS "AGENTINODIR not set, using submodule: ${AGENTINODIR}")
	endif()
endif()

if(NOT DEFINED PROLIFEDIR)
	set(PROLIFEDIR "$ENV{PROLIFEDIR}")
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

set(AUX_INCLUDE_DIR "${BUILDDIR}/ProLife/AuxInclude/${TARGETNAME}/GeneratedFiles")

include_directories("${AUX_INCLUDE_DIR}/../")
include_directories("${AUX_INCLUDE_DIR}/")

# Configure 3rd-party dependencies to build from source
# Disable building tests, examples, and samples
set(BUILD_TESTING OFF CACHE BOOL "Disable building tests for 3rd-party dependencies" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Disable building examples for 3rd-party dependencies" FORCE)
set(ACF_BUILD_SAMPLES OFF CACHE BOOL "Disable building ACF samples" FORCE)

# Add 3rd-party dependencies as subdirectories
if(EXISTS "${ACFDIR}/CMakeLists.txt")
	add_subdirectory("${ACFDIR}" "${AUX_INCLUDE_DIR}/Acf")
endif()
if(EXISTS "${ACFSLNDIR}/CMakeLists.txt")
	add_subdirectory("${ACFSLNDIR}" "${AUX_INCLUDE_DIR}/AcfSln")
endif()
if(EXISTS "${IMTCOREDIR}/CMakeLists.txt")
	add_subdirectory("${IMTCOREDIR}" "${AUX_INCLUDE_DIR}/ImtCore")
endif()
if(EXISTS "${PUMADIR}/CMakeLists.txt")
	add_subdirectory("${PUMADIR}" "${AUX_INCLUDE_DIR}/Puma")
endif()
if(EXISTS "${LISADIR}/CMakeLists.txt")
	add_subdirectory("${LISADIR}" "${AUX_INCLUDE_DIR}/Lisa")
endif()
if(EXISTS "${AGENTINODIR}/CMakeLists.txt")
	add_subdirectory("${AGENTINODIR}" "${AUX_INCLUDE_DIR}/Agentino")
endif()

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
add_subdirectory("${IMPL_DIR}/ProLifeDataPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeDataPck")
add_dependencies(ProLifeDataPck prolifedata)

add_subdirectory("${IMPL_DIR}/ProLifeDbPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeDbPck")
add_dependencies(ProLifeDbPck prolifedb)

add_subdirectory("${IMPL_DIR}/ProLifeGqlPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeGqlPck")
add_dependencies(ProLifeGqlPck prolifegql)

# add_subdirectory("${IMPL_DIR}/ProLifeConverter/CMake" "${AUX_INCLUDE_DIR}/ProLifeConverter")
add_subdirectory("${IMPL_DIR}/ProLifeServerConfigurator/CMake" "${AUX_INCLUDE_DIR}/ProLifeServerConfigurator")

file(COPY ../Migrations DESTINATION ${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})

