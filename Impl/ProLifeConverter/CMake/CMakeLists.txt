project(ProLifeConverter LANGUAGES CXX)

message("ACFDIR PROLIFE " ${ACFDIR})
message("CMAKE_PROJECT_INCLUDE_BEFORE " ${CMAKE_PROJECT_INCLUDE_BEFORE})

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ProLife.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

message("ARXC_CONFIG " ${ARXC_CONFIG})

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS WebSockets QuickWidgets)
find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Quick Qml REQUIRED)

include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

message("PROJECT_BINARY_DIR ${IMTCOREDIR}/Bin/web")

target_link_libraries(${PROJECT_NAME} imtbase imtgui imtauth imtauthgui imtlic imtlicdb imtlicgui imtqml imtdb imtclientgql imtcom imtgql)
target_link_libraries(${PROJECT_NAME} prolifedata prolifedb)

include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

if(WIN32)
	set(files  \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")

        windeploy(${PROJECT_NAME} "" "${files}")
endif()

