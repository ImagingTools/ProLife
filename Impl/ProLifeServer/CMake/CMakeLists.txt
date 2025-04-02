project(ProLifeServer LANGUAGES CXX)

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ProLife.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

if(WIN32)
#	set(CMAKE_WIN32_EXECUTABLE "on")
	set(ACF_CONVERT_FILES "${PROJECT_SOURCE_DIR}/../VC/ProLifeServer.rc.xtracf")
	set(ACF_CONVERT_REGISTRY "${PROJECT_SOURCE_DIR}/../VC/FileSubstitCopyApp.acc")
	set(ACF_CONVERT_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/BaseOnly.awc")
	set(RC_FILE "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/ProLifeServer.rc")
endif()

if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "on")
endif()

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

if(DEFINED WEB_COMPILE AND WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
endif()

target_link_libraries(${PROJECT_NAME} iauth iservice)
target_link_libraries(${PROJECT_NAME} imtbase imtmail imtgui imtauth imtauthgui imtlicdb imtlic imtlicgui ImtCoreLoc imtwidgets imtzip imtrest imtcrypt imt3dgui imtrepo imtstyle imtqml imtdb imtfile imtappsdl imtauthsdl imtlicsdl imtbasesdl imtserverapp)
target_link_libraries(${PROJECT_NAME} imtlicgql imtguigql imtclientgql imtgql imtauthgql imtauthdb imtcom imtapp imtservice imtlog imtservergql imtcol imtbasesdl)
target_link_libraries(${PROJECT_NAME} prolifestyle ImtCoreLoc ProLifeLoc prolifedata prolifedb prolifegql)
target_link_libraries(${PROJECT_NAME} prolifeqml imtstylecontrolsqml)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::QuickWidgets Qt${QT_VERSION_MAJOR}::WebSockets)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Sql)

include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

if(WIN32)
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Qml")
	list(APPEND listOptions "--qmldir=${PROJECT_SOURCE_DIR}/../../../Qml/Include/prolifeqml")
	set(files \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")

	windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)

	file(COPY ${PROJECT_SOURCE_DIR}/../../../../ImtCore/3rdParty/openssl/1.1/lib/x64/libcrypto-1_1-x64.dll DESTINATION ${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
	file(COPY ${PROJECT_SOURCE_DIR}/../../../../ImtCore/3rdParty/openssl/1.1/lib/x64/libssl-1_1-x64.dll DESTINATION ${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
endif()

