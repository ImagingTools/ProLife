project(ProLifeClientServer LANGUAGES CXX)

message("ACFDIR PROLIFE " ${ACFDIR})
message("CMAKE_PROJECT_INCLUDE_BEFORE " ${CMAKE_PROJECT_INCLUDE_BEFORE})

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ProLife.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

message("ARXC_CONFIG " ${ARXC_CONFIG})
message("ARXC_OUTDIR " ${ARXC_OUTDIR})

if(WIN32)
	set(CMAKE_WIN32_EXECUTABLE "on")
#	set(ACF_CONVERT_FILES "${PROJECT_SOURCE_DIR}/../VC/ProLifeClientServer.rc.xtracf")
	set(ACF_CONVERT_REGISTRY "${PROJECT_SOURCE_DIR}/../VC/FileSubstitCopyApp.acc")
	set(ACF_CONVERT_CONFIG  "${PROJECT_SOURCE_DIR}/../../../Config/BaseOnly.awc")
#	set(RC_FILE "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/ProLifeClientServer.rc")
endif()


if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "on")
endif()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS WebSockets QuickWidgets)
find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Quick Qml REQUIRED)
find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Sql REQUIRED)

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

if(DEFINED WEB_COMPILE AND  WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
endif()

target_link_libraries(${PROJECT_NAME} iauth)
target_link_libraries(${PROJECT_NAME} imtbase imtgui imtauth imtauthgui imtlicdb imtlic imtlicgui ImtCoreLoc imtwidgets imtzip imtrest imtcrypt imt3dgui imtrepo imtstyle imtqml imtdb imtfile)
target_link_libraries(${PROJECT_NAME} imtlicgql imtguigql imtgql imtauthgql imtauthdb imtcom imtapp)
target_link_libraries(${PROJECT_NAME} prolifeqml ImtCoreLoc ProLifeLoc prolifedata prolifedb prolifegql)
#target_link_libraries(${PROJECT_NAME} ProLifeLoc AcfSlnLoc IAcfLoc AcfLoc)
target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::QuickWidgets  Qt${QT_VERSION_MAJOR}::WebSockets)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Sql)

if(WIN32)
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Include/imtqml/Resources/qml")
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Include/imtgui/Qml/imtgui")
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Include/imtlicgui/Qml/imtlicgui")
	set(files  \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")

	windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)
endif()

list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include/imtqml/Qml")
list(APPEND QML_IMPORT_DIRS "${PROJECT_SOURCE_DIR}/../../../Include/prolifeqml/Qml")
list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include/imtgui/Qml")
list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include/imtauthgui/Qml")
list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include/imtlicgui/Qml")
list(APPEND QML_IMPORT_DIRS "qrc:/qml")
list(APPEND QML_IMPORT_DIRS ":/qml")

# Prevent adding duplicate values at each run of CMake.
list(REMOVE_DUPLICATES QML_IMPORT_DIRS)

# The variable is cached in ${BUILD_DIR}/CMakeCache.txt. We need FORCE to
# change it there immediately. Also, add a comment to the cache file.
set(QML_IMPORT_PATH ${QML_IMPORT_DIRS}
	CACHE STRING "Qt Creator extra qml import paths for ${PROJECT_NAME}"
	FORCE
)

