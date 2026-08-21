project(ProLifeConverter LANGUAGES CXX)

message(VERBOSE "ACFDIR PROLIFE " ${ACFDIR})
message(VERBOSE "CMAKE_PROJECT_INCLUDE_BEFORE " ${CMAKE_PROJECT_INCLUDE_BEFORE})

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ProLife.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

message(VERBOSE "ARXC_CONFIG " ${ARXC_CONFIG})

include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

message(VERBOSE "PROJECT_BINARY_DIR ${IMTCOREDIR}/Bin/web")

if(WIN32)
	set(files \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")

	windeploy(${PROJECT_NAME} "" "${files}")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
	prolifedata
	prolifedb
	prolifegql
	ImtCore::imtlicdb
)

