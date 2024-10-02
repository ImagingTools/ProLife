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

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

if(DEFINED WEB_COMPILE AND  WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
endif()

target_link_libraries(${PROJECT_NAME} imtbase imtgui imtauth imtauthgui imtlicdb imtlic imtlicgui imtrest imtcrypt imtrepo imtstyle imtdb imtfile imtlicsdl)
target_link_libraries(${PROJECT_NAME} imtlicgql imtdbgql imtguigql imtclientgql imtgql imtauthgql imtauthdb imtcom imtapp imtservice imtlog)
target_link_libraries(${PROJECT_NAME} imtqml imtcontrolsqml imtguiqml imtstylecontrolsqml imtdocguiqml imtguigqlqml imtauthguiqml imtlicguiqml imtcolguiqml)
target_link_libraries(${PROJECT_NAME} prolifeqml prolifestyle ImtCoreLoc ProLifeLoc prolifedata prolifedb prolifegql prolifeqml)
target_link_libraries(${PROJECT_NAME} iqtgui)
target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::QuickWidgets  Qt${QT_VERSION_MAJOR}::WebSockets)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Sql)

if(WIN32)
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Qml")
	list(APPEND listOptions "--qmldir=${PROJECT_SOURCE_DIR}/../../../Qml/Include/prolifeqml")
	set(files  \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")

	windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)
endif()



