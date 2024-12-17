project(prolifesdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Sensors.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Licenses.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Accounts.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Orders.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/SensorBinding.sdl)

ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Sensors.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Sensors"
			QML_NAME	"prolifeSensorsSdl")
ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Licenses.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Licenses"
			QML_NAME	"prolifeLicensesSdl")
ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Orders.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Orders"
			QML_NAME	"prolifeOrdersSdl")
ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Accounts.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Accounts"
			QML_NAME	"prolifeAccountsSdl")
ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/SensorBinding.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"SensorBinding"
			QML_NAME	"prolifeSensorBindingSdl")

