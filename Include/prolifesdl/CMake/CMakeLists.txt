project(prolifesdl)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreProjectSdlConfig.cmake)

ImtCoreConfigureSdlCppQml("SensorsAPIv1_0" "CPP/Sensors" "prolife::sdl::Sensors" "QML" "prolifeSensorsSdl")
ImtCoreConfigureSdlCppQml("LicensesAPIv1_0" "CPP/Licenses" "prolife::sdl::Licenses" "QML" "prolifeLicensesSdl")
ImtCoreConfigureSdlCppQml("OrdersAPIv1_0" "CPP/Orders" "prolife::sdl::Orders" "QML" "prolifeOrdersSdl")
ImtCoreConfigureSdlCppQml("AccountsAPIv1_0" "CPP/Accounts" "prolife::sdl::Accounts" "QML" "prolifeAccountsSdl")

