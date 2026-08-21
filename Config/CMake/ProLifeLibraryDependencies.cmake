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
# Link scopes are explicit in this file (PUBLIC/PRIVATE/INTERFACE). CMake
# forbids mixing the plain and keyword signatures on the same target. For static
# libraries the dependency still propagates transitively to consumers.
#
# Dependencies are declared *minimally*: each library lists only its direct
# dependencies; transitive dependencies propagate automatically through the graph.
# Do not add a dependency that is already reachable through another listed target.
#
# Included once, centrally, from Build/CMake/CMakeLists.txt after all library
# targets have been created.
# ---------------------------------------------------------------------------

# Dependencies are declared via the shared acf_declare_target_dependencies()
# helper from ACF/Acf/Config/CMake/ProjectRoot.cmake.

# ImtCore's SDL base library only carries the imtgql usage requirement for
# consumers that opt into it. ProLife's SDL is GraphQL-oriented, so expose imtgql
# through imtbasesdl for every ProLife library that builds on the SDL.
acf_declare_target_dependencies(ImtCore::imtbasesdl	LINK_SCOPE INTERFACE	ImtCore::imtgql)


# --- SDL generated libraries ------------------------------------------------
acf_declare_target_dependencies(prolifesdl		LINK_SCOPE PUBLIC	ImtCore::imtbasesdl)

# --- Libraries --------------------------------------------------------------
acf_declare_target_dependencies(prolifedata	LINK_SCOPE PUBLIC	ImtCore::imtauth)
acf_declare_target_dependencies(prolifedb		LINK_SCOPE PUBLIC	ImtCore::imtdb)
acf_declare_target_dependencies(prolifegql		LINK_SCOPE PUBLIC	prolifesdl prolifedata ImtCore::imtguigql ImtCore::imtgui)

# --- QML / style web-resource libraries -------------------------------------
if(QT_VERSION_MAJOR EQUAL 6)
	acf_declare_target_dependencies(prolifeqml		LINK_SCOPE PUBLIC	Qt${QT_VERSION_MAJOR}::Core5Compat)
	acf_declare_target_dependencies(prolifestyle	LINK_SCOPE PUBLIC	Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()

# --- Arxc-generated static libraries ----------------------------------------
acf_declare_target_dependencies(ProLifeLoc		LINK_SCOPE PUBLIC	Acf::icomp)

