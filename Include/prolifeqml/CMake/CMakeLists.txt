project(prolifeqml LANGUAGES CXX)

include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)
include(${ACFDIR}/Config/CMake/StaticConfig.cmake)

if(QT_VERSION_MAJOR EQUAL 6)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core5Compat REQUIRED)
	target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()

message("PROJECT_BINARY_DIR ${IMTCOREDIR}/Bin/web")
set(buildwebdir ${CMAKE_CURRENT_LIST_DIR}/../../../Bin/web)
message("LASA binary dir ${buildwebdir}")

#copy compiler from to
#list(APPEND webdirs ${IMTCOREDIR}/3rdParty/WebCompiler)
#list(APPEND webdirs ${IMTCOREDIR}/Bin/web)
#list(APPEND webdirs ${buildwebdir})

list(APPEND webdirs ${IMTCOREDIR}/3rdParty/JQML/Core/dist)
list(APPEND webdirs ${buildwebdir}/Resources)

# copy prolife qml from to
list(APPEND webdirs ${PROJECT_SOURCE_DIR}/../Qml)
list(APPEND webdirs ${buildwebdir}/src)

list(APPEND webdirs ${PROJECT_SOURCE_DIR}/../Resources/Translations)
#list(APPEND webdirs ${IMTCOREDIR}/Bin/web/src)
list(APPEND webdirs ${buildwebdir}/src)

#list(APPEND webdirs ${PROJECT_SOURCE_DIR}/../Resources/html)
list(APPEND webdirs ${PROJECT_SOURCE_DIR}/../Resources/html)
#list(APPEND webdirs ${IMTCOREDIR}/Bin/web/Resources)
list(APPEND webdirs ${buildwebdir}/Resources)

# copy base qml sources from to
list(APPEND webdirs ${IMTCOREDIR}/Include/imtgui/Qml/Acf)
#list(APPEND webdirs ${IMTCOREDIR}/Bin/web/src/Acf)
list(APPEND webdirs ${buildwebdir}/src/Acf)

list(APPEND webdirs ${IMTCOREDIR}/Include/imtgui/Qml/imtgui)
#list(APPEND webdirs ${IMTCOREDIR}/Bin/web/src/imtgui)
list(APPEND webdirs ${buildwebdir}/src/imtgui)

list(APPEND webdirs ${IMTCOREDIR}/Include/imtqml/Qml/imtqml)
#list(APPEND webdirs ${IMTCOREDIR}/Bin/web/src/imtqml)
list(APPEND webdirs ${buildwebdir}/src/imtqml)

list(APPEND webdirs ${IMTCOREDIR}/Include/imtauthgui/Qml/imtauthgui)
#list(APPEND webdirs ${IMTCOREDIR}/Bin/web/src/imtauthgui)
list(APPEND webdirs ${buildwebdir}/src/imtauthgui)

list(APPEND webdirs ${IMTCOREDIR}/Include/imtlicgui/Qml/imtlicgui)
#list(APPEND webdirs ${IMTCOREDIR}/Bin/web/src/imtlicgui)
list(APPEND webdirs ${buildwebdir}/src/imtlicgui)


message("PROLIFEDIR ${PROLIFEDIR}")

# copy dist from to
list(APPEND webdirs ${PROJECT_SOURCE_DIR}/../Resources/dist)
#list(APPEND webdirs ${PROJECT_SOURCE_DIR}/../Resources/dist)
#list(APPEND webdirs ${IMTCOREDIR}/Bin/web/dist)
list(APPEND webdirs ${buildwebdir}/Resources)

# copy result from to
list(APPEND webdirs ${IMTCOREDIR}/Bin/web/build.web)
#list(APPEND webdirs ${IMTCOREDIR}/Bin/web/Resources)
list(APPEND webdirs ${buildwebdir}/Resources)

if(DEFINED WEB_COMPILE AND  WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
	jqml_compile_web("${webdirs}", "${buildwebdir}")
endif()

