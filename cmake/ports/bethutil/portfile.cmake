vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO Guekka/bethutil
        REF de993e54a0522ddb4788c4951aec04ca45255e2d
        SHA512 789dd53817474fa876a9240e210cb4d1ac4de9890a3ca9e2ede68a828936432393319ed0514493a31cde8afa726eacb8b9b1ff3482014aed83e9d432d8df6b5f
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
