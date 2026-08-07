# ---------------------------------------------------------------------------
# Clean, target-based inter-library dependency graph for ProLife.
#
# Instead of relying on the
# final executable/package link to resolve symbols and on a hand-tuned build
# order (the add_dependencies()/inline target_link_libraries() spread across the
# per-library CMake files), the dependencies between the ProLife libraries - and
# their dependencies onto the underlying ImtCore::, Acf::, AcfSln:: and IAcf::
# libraries - are declared here as target usage requirements. Include paths and
# link order then propagate transitively and automatically for the in-tree build.
#
# Every ImtCore::/Acf::/AcfSln:: imported target exposes its whole source include
# tree (acf_register_library() adds INCLUDE_DIR/IMPL_DIR as PUBLIC include
# directories), so a single ImtCore:: dependency transitively provides the full
# ImtCore, Acf, AcfSln and IAcf header search paths to the consuming library.
#
# The target_link_libraries() signature is controlled by ACF_LIBRARY_LINK_SCOPE:
#  * when empty, the plain signature is used (matching the legacy ProLife CMake),
#  * when set to PUBLIC/PRIVATE/INTERFACE, the keyword signature is used.
# CMake forbids mixing the plain and keyword signatures on the same target. For
# static libraries the dependency still propagates transitively to consumers.
#
# Dependencies are declared *minimally*: each library lists only its direct
# dependencies; transitive dependencies propagate automatically through the graph.
# Do not add a dependency that is already reachable through another listed target.
#
# Included once, centrally, from Build/CMake/CMakeLists.txt after all library
# targets have been created.
# ---------------------------------------------------------------------------

# Declare the dependencies of a ProLife library, ignoring any entry whose target
# does not exist in the current configuration (for example feature-gated
# libraries, or ImtCore::/Acf::/AcfSln::/IAcf:: targets that are not available
# because the legacy shim is used instead of find_package).
function(prolife_declare_library_dependencies target)
	cmake_parse_arguments(ARG "" "LINK_SCOPE" "" ${ARGN})

	if(NOT ARG_LINK_SCOPE)
		set(ARG_LINK_SCOPE ${ACF_LIBRARY_LINK_SCOPE})
	endif()

	if(NOT TARGET ${target})
		return()
	endif()

	# The only entry whose *target* is an ImtCore:: name is the imtbasesdl->imtgql
	# usage-requirement augmentation, needed solely for the imported
	# find_package(ImtCore) target. In a unified in-tree build ImtCore::imtbasesdl is
	# an ALIAS: target_link_libraries() is illegal on it, and augmenting the real
	# target injects a dependency cycle through the Qt autogen targets. Skip aliases.
	get_target_property(_prolife_aliased ${target} ALIASED_TARGET)
	if(_prolife_aliased)
		return()
	endif()

	foreach(dependency IN LISTS ARG_UNPARSED_ARGUMENTS)
		if(TARGET ${dependency})
			target_link_libraries(${target} ${ARG_LINK_SCOPE} ${dependency})
		endif()
	endforeach()
endfunction()

# ImtCore's SDL base library only carries the imtgql usage requirement for
# consumers that opt into it. ProLife's SDL is GraphQL-oriented, so expose imtgql
# through imtbasesdl for every ProLife library that builds on the SDL.
prolife_declare_library_dependencies(ImtCore::imtbasesdl	LINK_SCOPE INTERFACE	ImtCore::imtgql)


# --- SDL generated libraries ------------------------------------------------
prolife_declare_library_dependencies(prolifesdl		ImtCore::imtbasesdl)

# --- Libraries --------------------------------------------------------------
prolife_declare_library_dependencies(prolifedata	ImtCore::imtauth)
prolife_declare_library_dependencies(prolifedb		ImtCore::imtdb)
prolife_declare_library_dependencies(prolifegql		prolifesdl prolifedata ImtCore::imtguigql ImtCore::imtgui)

# --- QML / style web-resource libraries -------------------------------------
if(QT_VERSION_MAJOR EQUAL 6)
	prolife_declare_library_dependencies(prolifeqml		Qt${QT_VERSION_MAJOR}::Core5Compat)
	prolife_declare_library_dependencies(prolifestyle	Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()

# --- Arxc-generated static libraries ----------------------------------------
prolife_declare_library_dependencies(ProLifeLoc		Acf::icomp)


# --- Packages ---------------------------------------------------------------
prolife_declare_library_dependencies(ProLifeDataPck	LINK_SCOPE PRIVATE	prolifedata)
prolife_declare_library_dependencies(ProLifeDbPck	LINK_SCOPE PRIVATE	prolifedb prolifedata)
prolife_declare_library_dependencies(ProLifeGqlPck	LINK_SCOPE PRIVATE	prolifegql)

# --- Plug-ins ---------------------------------------------------------------
prolife_declare_library_dependencies(ProLifeSettingsPlugin	LINK_SCOPE PRIVATE
	ImtCore::imtserverapp Qt${QT_VERSION_MAJOR}::Xml)


# --- Applications -----------------------------------------------------------
# Web/desktop client.
prolife_declare_library_dependencies(ProLifeClient	LINK_SCOPE PRIVATE
	prolifeqml prolifestyle prolifesdl
	ProLifeLoc ImtCore::ImtCoreLoc Acf::AcfLoc AcfSln::AcfSlnLoc
	ImtCore::imt2dsdl ImtCore::imtdesksdl ImtCore::imtchatsdl
	ImtCore::imtserverapp ImtCore::imtauthdb ImtCore::imtdeskdb ImtCore::imtchatdb ImtCore::imtrepo
	ImtCore::imtlicgql ImtCore::imtauthgql
	ImtCore::imtcontrolsqml ImtCore::imtstylecontrolsqml ImtCore::imtguiqml ImtCore::imtguigqlqml
	ImtCore::imtauthguiqml ImtCore::imtcolguiqml ImtCore::imtdocguiqml ImtCore::imtlicguiqml
	ImtCore::imtdeskguiqml ImtCore::imtchatguiqml
	Qt${QT_VERSION_MAJOR}::QuickControls2)

# Server and its ServerTest twin. This is a server/console component and
# deliberately does NOT depend on the 3D stack (imt3dgui).
foreach(_prolife_server ProLifeServer ProLifeServerTest)
	prolife_declare_library_dependencies(${_prolife_server}	LINK_SCOPE PRIVATE
		prolifestyle prolifedb prolifegql prolifeqml
		ProLifeLoc ImtCore::ImtCoreLoc Acf::AcfLoc AcfSln::AcfSlnLoc AcfSln::iservice
		ImtCore::imt2dsdl ImtCore::imtserverapp ImtCore::imtlicdb ImtCore::imtzip ImtCore::imtrepo
		ImtCore::imtlicgql ImtCore::imtauthgql ImtCore::imtauthdb ImtCore::imtlog
		ImtCore::imtchatdb ImtCore::imtdeskdb ImtCore::imtdeskgql ImtCore::imtchatgql
		ImtCore::imtcontrolsqml ImtCore::imtstylecontrolsqml ImtCore::imtguiqml ImtCore::imtguigqlqml
		ImtCore::imtauthguiqml ImtCore::imtcolguiqml ImtCore::imtdocguiqml ImtCore::imtlicguiqml)
endforeach()

prolife_declare_library_dependencies(ProLifeServerConfigurator	LINK_SCOPE PRIVATE
	prolifeqml ProLifeLoc ImtCore::ImtCoreLoc Acf::AcfLoc AcfSln::AcfSlnLoc
	ImtCore::imt2dsdl ImtCore::imtserverapp ImtCore::imtauthdb ImtCore::imtdeskdb ImtCore::imtchatdb ImtCore::imtlog
	ImtCore::imtcontrolsqml ImtCore::imtstylecontrolsqml ImtCore::imtguiqml ImtCore::imtguigqlqml
	ImtCore::imtauthguiqml ImtCore::imtcolguiqml ImtCore::imtdocguiqml ImtCore::imtlicguiqml)

# --- Legacy single-binary / tooling targets (not part of the default in-tree build) ----------
# Combined client/server single binary. imtserverapp pulls the imtbase/imtgui/imtqml/imtdb/... core.
prolife_declare_library_dependencies(ProLifeClientServer	LINK_SCOPE PRIVATE
	prolifeqml prolifestyle prolifedb prolifegql
	ProLifeLoc ImtCore::ImtCoreLoc
	ImtCore::imtserverapp ImtCore::imtlicdb ImtCore::imtrepo ImtCore::imtlicgql ImtCore::imtdbgql
	ImtCore::imtauthgql ImtCore::imtauthdb ImtCore::imtlog
	ImtCore::imtcontrolsqml ImtCore::imtguiqml ImtCore::imtstylecontrolsqml ImtCore::imtdocguiqml
	ImtCore::imtguigqlqml ImtCore::imtauthguiqml ImtCore::imtlicguiqml ImtCore::imtcolguiqml)

prolife_declare_library_dependencies(ProLifeConverter	LINK_SCOPE PRIVATE
	prolifedata prolifedb
	ImtCore::imtauthgui ImtCore::imtlicdb ImtCore::imtlicgui)

prolife_declare_library_dependencies(prolifetest	LINK_SCOPE PRIVATE
	Acf::itest Acf::ipackage
	ImtCore::imtdb ImtCore::imtauthgql
	Qt${QT_VERSION_MAJOR}::Test)
