project(ProLifeDataPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtlic prolifedata imtgql)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
