project(prolifeqml LANGUAGES CXX)

include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)
include(${ACFDIR}/Config/CMake/StaticConfig.cmake)

if(QT_VERSION_MAJOR EQUAL 6)
	target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()

if(NOT DEFINED PYTHONEXE)
	set(PYTHONEXE "$ENV{PYTHONEXE}")
endif()

set(buildwebdir ${BUILDDIR}/ProLife/AuxInclude/Web)

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

list(APPEND webdirs ${AUX_INCLUDE_DIR}/prolifesdl/SDL/1.0/QML/prolifeDeviceCollectionDocumentServiceSdl)
list(APPEND webdirs ${buildwebdir}/src/prolifeDeviceCollectionDocumentServiceSdl)

list(APPEND webdirs ${PROJECT_SOURCE_DIR}/../../../../Impl/ProLifeLoc/Translations)
list(APPEND webdirs ${buildwebdir}/Resources/Translations)

option(USE_NEW_WEB "Use new web compilation (jq_compile_web)" OFF)

if(DEFINED WEB_COMPILE AND WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
	set(resname "prolife")
	if(USE_NEW_WEB)
		set(inputjs "${PROLIFEDIR}/Qml/Include/prolifeqml/prolife.json")
		set(appicon "../Icons/ProLifeIcon.svg")
		set(dataroot "/ProLife/Views/")
		set(startqml "${PROLIFEDIR}/Qml/Include/prolifeqml/ProLifeWeb.qml")

		jq_compile_web(
			"${webdirs}"
			"${buildwebdir}"
			"${resname}"
			"${inputjs}"
			"${appicon}"
			"${dataroot}"
			"${startqml}"
		)
	else()
		jqml_compile_web2(
			"${webdirs}"
			"${buildwebdir}"
			"${resname}"
		)
	endif()
endif()

