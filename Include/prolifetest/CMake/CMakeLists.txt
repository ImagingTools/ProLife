project(prolifetest)

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Sql Widgets)

if(WIN32)
	set(listFiles \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")
	windeploy(${PROJECT_NAME} "" "${listFiles}")
endif()
