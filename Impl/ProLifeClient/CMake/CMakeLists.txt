project(ProLifeClient LANGUAGES CXX)

message(VERBOSE "ACFDIR PROLIFE " ${ACFDIR})
message(VERBOSE "CMAKE_PROJECT_INCLUDE_BEFORE " ${CMAKE_PROJECT_INCLUDE_BEFORE})

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ProLife.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

message(VERBOSE "ARXC_CONFIG " ${ARXC_CONFIG})

if(WIN32)
	set(CMAKE_WIN32_EXECUTABLE "on")
	set(ACF_CONVERT_FILES "${PROJECT_SOURCE_DIR}/../VC/ProLifeClient.rc.xtracf")
	set(ACF_CONVERT_REGISTRY "${PROJECT_SOURCE_DIR}/../VC/FileSubstitCopyApp.acc")
	set(ACF_CONVERT_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/BaseOnly.awc")
	set(RC_FILE "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/ProLifeClient.rc")
endif()

if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "on")
endif()

include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)
include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

message(VERBOSE "PROJECT_BINARY_DIR ${IMTCOREDIR}/Bin/web")

set(DEPLOY_QT_QML_DIRS
	"${IMTCOREDIR}/Qml"
	"${IMTCOREDIR}/Include/imtstylecontrolsqml/Qml"
	"${PROJECT_SOURCE_DIR}/../../../Qml/Include/prolifeqml"
)
deploy_qt(TARGET ${PROJECT_NAME})

set(QML_IMPORT_DIRS ${QML_IMPORT_PATH})
list(APPEND QML_IMPORT_DIRS "${PROJECT_SOURCE_DIR}/../../../Qml/Include/prolifeqml")
list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Qml")
list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include/imtstylecontrolsqml/Qml")

# Prevent adding duplicate values at each run of CMake.
list(REMOVE_DUPLICATES QML_IMPORT_DIRS)

# The variable is cached in ${BUILD_DIR}/CMakeCache.txt. We need FORCE to
# change it there immediately. Also, add a comment to the cache file.
set(QML_IMPORT_PATH ${QML_IMPORT_DIRS}
	CACHE STRING "Qt Creator extra qml import paths for ${PROJECT_NAME}"
	FORCE
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	prolifeqml
	prolifestyle
	prolifesdl
	ProLifeLoc
	ImtCore::ImtCoreLoc
	ImtCore::imtdesksdl
	ImtCore::imtlicsdl
	ImtCore::imtcontrolsqml
	ImtCore::imtstylecontrolsqml
	ImtCore::imtguiqml
	ImtCore::imtguigqlqml
	ImtCore::imtauthguiqml
	ImtCore::imtcolguiqml
	ImtCore::imtdocguiqml
	ImtCore::imtlicguiqml
	ImtCore::imtdeskguiqml
	Qt${QT_VERSION_MAJOR}::QuickControls2
)

