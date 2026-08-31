project(prolifetest)

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Sql Widgets)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Acf::itest
	Acf::ipackage
	ImtCore::imtdb
	ImtCore::imtauthgql
	Qt${QT_VERSION_MAJOR}::Test
)

deploy_qt(TARGET ${PROJECT_NAME})
