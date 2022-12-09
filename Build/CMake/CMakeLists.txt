cmake_minimum_required(VERSION 3.1.0)

project(ProLifeAll)

if(NOT DEFINED IMTCOREDIR)
	set(IMTCOREDIR "$ENV{IMTCOREDIR}")
endif()

include(${IMTCOREDIR}/Config/CMake/ImtCoreEnv.cmake)

set(INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Include")
set(IMPL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Impl")
set(AUX_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../AuxInclude/${TARGETNAME}/GeneratedFiles")

include_directories("${AUX_INCLUDE_DIR}/../")

# Include

add_subdirectory("${INCLUDE_DIR}/prolifeqml/CMake" "${AUX_INCLUDE_DIR}/prolifeqml")
add_subdirectory("${INCLUDE_DIR}/prolifeauth/CMake" "${AUX_INCLUDE_DIR}/prolifeauth")
add_subdirectory("${INCLUDE_DIR}/prolifelicdb/CMake" "${AUX_INCLUDE_DIR}/prolifelicdb")
add_subdirectory("${INCLUDE_DIR}/prolifelicgql/CMake" "${AUX_INCLUDE_DIR}/prolifelicgql")


# Impl

add_subdirectory("${IMPL_DIR}/ProLifeLoc/CMake" "${AUX_INCLUDE_DIR}/ProLifeLoc")
add_subdirectory("${IMPL_DIR}/ProLifeClientServer/CMake" "${AUX_INCLUDE_DIR}/ProLifeClientServer")
add_subdirectory("${IMPL_DIR}/ProLifeClient/CMake" "${AUX_INCLUDE_DIR}/ProLifeClient")
add_subdirectory("${IMPL_DIR}/ProLifeServer/CMake" "${AUX_INCLUDE_DIR}/ProLifeServer")
add_subdirectory("${IMPL_DIR}/ProLifeAuthPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeAuthPck")
add_subdirectory("${IMPL_DIR}/ProLifeLicenseDbPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeLicenseDbPck")
add_subdirectory("${IMPL_DIR}/ProLifeLicenseGqlPck/CMake" "${AUX_INCLUDE_DIR}/ProLifeLicenseGqlPck")

file(COPY ../Migrations DESTINATION ${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
