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

set(AUX_INCLUDE_DIR "${BUILDDIR}/ProLife/AuxInclude/${TARGETNAME}/GeneratedFiles")

include_directories("${AUX_INCLUDE_DIR}/../")
include_directories("${AUX_INCLUDE_DIR}/")

option(PROLIFE_USE_BUNDLED_LIBRARIES "Use bundled 3rd-party libraries (build from source)" OFF)

if(PROLIFE_USE_BUNDLED_LIBRARIES)
	message(STATUS "Building with bundled libraries...")

	add_subdirectory("${PROLIFEDIR}/3rdParty/Acf/Build/CMake"     "${AUX_INCLUDE_DIR}/Acf")
	add_subdirectory("${PROLIFEDIR}/3rdParty/AcfSln/Build/CMake"  "${AUX_INCLUDE_DIR}/AcfSln")
	add_subdirectory("${PROLIFEDIR}/3rdParty/ImtCore/Build/CMake" "${AUX_INCLUDE_DIR}/ImtCore")
	add_subdirectory("${PROLIFEDIR}/3rdParty/Puma/Build/CMake"    "${AUX_INCLUDE_DIR}/Puma")
	add_subdirectory("${PROLIFEDIR}/3rdParty/Lisa/Build/CMake"    "${AUX_INCLUDE_DIR}/Lisa")
else()
	message(STATUS "Using system libraries (via find_package or pre-installed environment)")
endif()

# Sdl
include("${CMAKE_CURRENT_LIST_DIR}/../../../ImtCore/Config/CMake/ImtCoreSdlCustomConfig.cmake")
add_subdirectory("${INCLUDE_DIR}/../Sdl/prolife/CMake" "${AUX_INCLUDE_DIR}/prolifesdl")
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(prolifesdl ImtCoreAll AcfStatic)
endif()

# Include
add_subdirectory("${INCLUDE_DIR}/prolifedata/CMake" "${AUX_INCLUDE_DIR}/prolifedata")
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(prolifedata ImtCoreAll AcfStatic)
endif()

add_subdirectory("${INCLUDE_DIR}/prolifedb/CMake" "${AUX_INCLUDE_DIR}/prolifedb")
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(prolifedb ImtCoreAll AcfStatic)
endif()

add_subdirectory("${INCLUDE_DIR}/prolifegql/CMake" "${AUX_INCLUDE_DIR}/prolifegql")
add_dependencies(prolifegql prolifesdl)
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(prolifegql ImtCoreAll AcfStatic)
endif()

add_subdirectory("${INCLUDE_DIR}/../Qml/Include/prolifeqml/CMake" "${AUX_INCLUDE_DIR}/prolifeqml")
add_dependencies(prolifeqml prolifesdl)
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(prolifeqml ImtCoreAll AcfStatic)
endif()

add_subdirectory("${INCLUDE_DIR}/prolifestyle/CMake" "${AUX_INCLUDE_DIR}/prolifestyle")
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(prolifestyle ImtCoreAll AcfStatic)
endif()

# Tests
#add_subdirectory("${INCLUDE_DIR}/prolifetest/CMake" "${AUX_INCLUDE_DIR}/prolifetest")

# Plug-ins
add_subdirectory("${IMPL_DIR}/Plugins/ProLifeSettingsPlugin/CMake" "${AUX_INCLUDE_DIR}/ProLifeSettingsPlugin")
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(ProLifeSettingsPlugin ImtCoreAll AcfStatic)
endif()

# Impl
add_subdirectory("${IMPL_DIR}/ProLifeLoc/CMake" "${AUX_INCLUDE_DIR}/ProLifeLoc")
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(ProLifeLoc AcfStatic)
endif()

add_subdirectory("${IMPL_DIR}/ProLifeClient/CMake" "${AUX_INCLUDE_DIR}/ProLifeClient")
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(ProLifeClient ImtCoreAll AcfStatic)
endif()

add_subdirectory("${IMPL_DIR}/ProLifeServer/CMake" "${AUX_INCLUDE_DIR}/ProLifeServer")
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(ProLifeServer AcfStatic)
endif()
add_subdirectory("${IMPL_DIR}/ProLifeDataPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeDataPck")
add_dependencies(ProLifeDataPck prolifedata)
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(ProLifeDataPck ImtCoreAll AcfStatic)
endif()

add_subdirectory("${IMPL_DIR}/ProLifeDbPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeDbPck")
add_dependencies(ProLifeDbPck prolifedb)
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(ProLifeDbPck ImtCoreAll AcfStatic)
endif()

add_subdirectory("${IMPL_DIR}/ProLifeGqlPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeGqlPck")
add_dependencies(ProLifeGqlPck prolifegql)
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(ProLifeGqlPck ImtCoreAll AcfStatic)
endif()

# add_subdirectory("${IMPL_DIR}/ProLifeConverter/CMake" "${AUX_INCLUDE_DIR}/ProLifeConverter")
add_subdirectory("${IMPL_DIR}/ProLifeServerConfigurator/CMake" "${AUX_INCLUDE_DIR}/ProLifeServerConfigurator")
if(PROLIFE_USE_BUNDLED_LIBRARIES)
	add_dependencies(ProLifeServerConfigurator AcfStatic)
endif()

file(COPY ../Migrations DESTINATION ${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})

