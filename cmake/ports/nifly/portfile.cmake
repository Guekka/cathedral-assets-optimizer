vcpkg_fail_port_install(ON_TARGET "UWP")
vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO ousnius/nifly
    REF bab4c6ec7cac88df2926d60cf5d80bb590e4c58a
    SHA512 5e0dea29452cb0cb86e8be7d3595703edfc7525e76b33685f803bb55b078ca2026ee2f2739eac956dc6ff183e4ae6f28b55d4b9f280c2c36218bdf6767420938
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
