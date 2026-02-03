project(prolifeqml LANGUAGES CXX)

include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)
include(${ACFDIR}/Config/CMake/StaticConfig.cmake)

if(QT_VERSION_MAJOR EQUAL 6)
	target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()

if(NOT DEFINED PYTHONEXE)
	set(PYTHONEXE "$ENV{PYTHONEXE}")
endif()

set(buildwebdir ${BUILDDIR}/ProLife/Bin/web)

# copy prolife qml from to
list(APPEND webdirs ${PROLIFEDIR}/Qml/Include/prolifeqml)
list(APPEND webdirs ${buildwebdir}/src)

list(APPEND webdirs ${PROLIFEDIR}/Include/prolifestyle/Resources/html)
list(APPEND webdirs ${buildwebdir}/Resources)

# copy base qml sources from to
getImtCoreQmlWebDirs(webdirs "${buildwebdir}")

list(APPEND webdirs ${AUX_INCLUDE_DIR}/prolifesdl/SDL/1.0/QML/prolifeAccountsSdl)
list(APPEND webdirs ${buildwebdir}/src/prolifeAccountsSdl)

list(APPEND webdirs ${AUX_INCLUDE_DIR}/prolifesdl/SDL/1.0/QML/prolifeSensorsSdl)
list(APPEND webdirs ${buildwebdir}/src/prolifeSensorsSdl)

list(APPEND webdirs ${AUX_INCLUDE_DIR}/prolifesdl/SDL/1.0/QML/prolifeLicensesSdl)
list(APPEND webdirs ${buildwebdir}/src/prolifeLicensesSdl)

list(APPEND webdirs ${AUX_INCLUDE_DIR}/prolifesdl/SDL/1.0/QML/prolifeOrdersSdl)
list(APPEND webdirs ${buildwebdir}/src/prolifeOrdersSdl)

list(APPEND webdirs ${AUX_INCLUDE_DIR}/prolifesdl/SDL/1.0/QML/prolifeWorkspaceSdl)
list(APPEND webdirs ${buildwebdir}/src/prolifeWorkspaceSdl)

list(APPEND webdirs ${PROJECT_SOURCE_DIR}/../../../../Impl/ProLifeLoc/Translations)
list(APPEND webdirs ${buildwebdir}/Resources/Translations)

if(DEFINED WEB_COMPILE AND WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
	set(resname "prolife")
	message("PYTHONEXE ${PYTHONEXE}")
	jqml_compile_web2("${webdirs}" "${buildwebdir}" "${resname}")
endif()

