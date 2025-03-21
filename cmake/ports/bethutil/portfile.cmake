vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO Guekka/bethutil
        REF fe6ee404082a02c3030e45d3f2568e4cfeeab295
        SHA512 d7062312bd22a6d5acf0f98422f111f46b19be3775c34b91063126f80d07a0b5833a8aec34b15878aacb30702fddf61897ed7aef2a756638ba3c76591b739fe7
        HEAD_REF main
)

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
