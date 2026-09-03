project(ProLifeClientServer LANGUAGES CXX)

message(VERBOSE "ACFDIR PROLIFE " ${ACFDIR})
message(VERBOSE "CMAKE_PROJECT_INCLUDE_BEFORE " ${CMAKE_PROJECT_INCLUDE_BEFORE})

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ProLife.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

message(VERBOSE "ARXC_CONFIG " ${ARXC_CONFIG})
message(VERBOSE "ARXC_OUTDIR " ${ARXC_OUTDIR})

if(WIN32)
	set(CMAKE_WIN32_EXECUTABLE "on")
#	set(ACF_CONVERT_FILES "${PROJECT_SOURCE_DIR}/../VC/ProLifeClientServer.rc.xtracf")
	set(ACF_CONVERT_REGISTRY "${PROJECT_SOURCE_DIR}/../VC/FileSubstitCopyApp.acc")
	set(ACF_CONVERT_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/BaseOnly.awc")
#	set(RC_FILE "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/ProLifeClientServer.rc")
endif()


if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "on")
endif()

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

if(DEFINED WEB_COMPILE AND WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
endif()

set(DEPLOY_QT_QML_DIRS
	"${IMTCOREDIR}/Qml"
	"${PROJECT_SOURCE_DIR}/../../../Qml/Include/prolifeqml"
)
deploy_qt(TARGET ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
	prolifeqml
	prolifestyle
	prolifedb
	prolifegql
	ProLifeLoc
	ImtCore::ImtCoreLoc
	ImtCore::imtcontrolsqml
	ImtCore::imtguiqml
	ImtCore::imtstylecontrolsqml
	ImtCore::imtdocguiqml
	ImtCore::imtguigqlqml
	ImtCore::imtauthguiqml
	ImtCore::imtlicguiqml
	ImtCore::imtcolguiqml
)



