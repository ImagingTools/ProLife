project(prolifegqlTest)


include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	prolifegql
	prolifedata
	ImtCore::imtservergql
	ImtCore::imtgql
	ImtCore::imtauth
	ImtCore::imtaccount
	ImtCore::imtlic
	ImtCore::imtbase
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
)

deploy_qt(TARGET ${PROJECT_NAME})
