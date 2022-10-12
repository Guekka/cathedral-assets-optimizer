vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Guekka/bethutil
    REF 81f882ed4d3fbb3c04b0c90658a94b3b2eaade02
    SHA512 4c0e6ce892bd292972832e8e808ee8761a331a73c9062adc4b4655f2e531c2a8afd67163cc1995fbae2968e632b9b1e6caf9f651fb560926c9987e2312f44ce6
    HEAD_REF main
)

set(SOURCE_PATH "C:/IT/Code_perso/bethutil")

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
	OPTIONS
		-DBUILD_TESTING=OFF
)
vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/bethutil")

file(REMOVE_RECURSE
	${CURRENT_PACKAGES_DIR}/debug/include
	${CURRENT_PACKAGES_DIR}/debug/share
)

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
