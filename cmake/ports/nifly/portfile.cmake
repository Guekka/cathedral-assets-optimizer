vcpkg_fail_port_install(ON_TARGET "UWP")
vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO ousnius/nifly
    REF 22949bc4621af21bea4a866de75ac30d0ab53e11
    SHA512 478899811a3e29a7eaaf237f37ffbcec3dd0375d69d6a41b1d9614bfd28946f589c64435b960798cd46d14c4c59632ef0d3d21cf520b67d736030b29e58fcdde
    HEAD_REF main
)

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
	OPTIONS
		-DBUILD_TESTING=OFF
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH "cmake/")

file(REMOVE_RECURSE
	"${CURRENT_PACKAGES_DIR}/debug/include"
	"${CURRENT_PACKAGES_DIR}/debug/share"

)

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
