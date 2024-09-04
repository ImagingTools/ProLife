project(ProLifeClient LANGUAGES CXX)

message("ACFDIR PROLIFE " ${ACFDIR})
message("CMAKE_PROJECT_INCLUDE_BEFORE " ${CMAKE_PROJECT_INCLUDE_BEFORE})

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ProLife.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

message("ARXC_CONFIG " ${ARXC_CONFIG})

if(WIN32)
	set(CMAKE_WIN32_EXECUTABLE "on")
	set(ACF_CONVERT_FILES "${PROJECT_SOURCE_DIR}/../VC/ProLifeClient.rc.xtracf")
	set(ACF_CONVERT_REGISTRY "${PROJECT_SOURCE_DIR}/../VC/FileSubstitCopyApp.acc")
	set(ACF_CONVERT_CONFIG  "${PROJECT_SOURCE_DIR}/../../../Config/BaseOnly.awc")
	set(RC_FILE "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/ProLifeClient.rc")
endif()

if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "on")
endif()

include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)
include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

message("PROJECT_BINARY_DIR ${IMTCOREDIR}/Bin/web")

target_link_libraries(${PROJECT_NAME} imtbase imtgui imtauth imtauthgui imtlic imtlicgui imtrest imtrepo imtqml imtdb imtdocguiqml)
target_link_libraries(${PROJECT_NAME} imtlicgql imtguigql imtclientgql imtgql imtauthgql imtcom imtguigqlqml imtauthguiqml imtlicguiqml imtguiqml imtcolguiqml imtstylecontrolsqml imtcontrolsqml)
target_link_libraries(${PROJECT_NAME} prolifeqml prolifestyle prolifesdl ImtCoreLoc ProLifeLoc)
target_link_libraries(${PROJECT_NAME} iqtgui)
target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::QuickWidgets  Qt${QT_VERSION_MAJOR}::WebSockets)
target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::QuickControls2)

if(WIN32)
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Qml")
	list(APPEND listOptions "--qmldir=${PROJECT_SOURCE_DIR}/../../../Qml/Include/prolifeqml")
	set(files  \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")

	windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)
endif()

set(QML_IMPORT_DIRS ${QML_IMPORT_PATH})
list(APPEND QML_IMPORT_DIRS "${PROJECT_SOURCE_DIR}/../../../Qml/Include/prolifeqml")
list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Qml")

# Prevent adding duplicate values at each run of CMake.
list(REMOVE_DUPLICATES QML_IMPORT_DIRS)

# The variable is cached in ${BUILD_DIR}/CMakeCache.txt. We need FORCE to
# change it there immediately. Also, add a comment to the cache file.
set(QML_IMPORT_PATH ${QML_IMPORT_DIRS}
	CACHE STRING "Qt Creator extra qml import paths for ${PROJECT_NAME}"
	FORCE
)

