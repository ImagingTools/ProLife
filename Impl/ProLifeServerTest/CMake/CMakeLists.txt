project(ProLifeServerTest LANGUAGES CXX)

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ProLife.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

if(WIN32)
#	set(CMAKE_WIN32_EXECUTABLE "on")
	set(ACF_CONVERT_FILES "${PROJECT_SOURCE_DIR}/../VC/ProLifeServerTest.rc.xtracf")
	set(ACF_CONVERT_REGISTRY "${PROJECT_SOURCE_DIR}/../VC/FileSubstitCopyApp.acc")
	set(ACF_CONVERT_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/BaseOnly.awc")
	set(RC_FILE "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/ProLifeServerTest.rc")
endif()

if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "on")
endif()

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
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

if(WIN32)
	file(COPY ${OPENSSLDIR}/lib/x64/libcrypto-1_1-x64.dll DESTINATION ${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
	file(COPY ${OPENSSLDIR}/lib/x64/libssl-1_1-x64.dll DESTINATION ${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
endif()

include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${IACFDIR}/Config/CMake/zlib.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	prolifestyle
	prolifedb
	prolifegql
	prolifeqml
	ProLifeLoc
	ImtCore::ImtCoreLoc
	AcfSln::iservice
	ImtCore::imtauthgql
	ImtCore::imtauthdb
	ImtCore::imtlog
	ImtCore::imtchatdb
	ImtCore::imtdeskdb
	ImtCore::imtdeskgql
	ImtCore::imtchatgql
	ImtCore::imtlicguiqml
)
