project(ProLifeGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)

target_link_libraries(${PROJECT_NAME} prolifegql imtguigql imtbase imtgql imtgui imtwidgets imtlic imtdb prolifedata imtbasesdl)

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Gui)

