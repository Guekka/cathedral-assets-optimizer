vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO Guekka/bethutil
        REF 79a21977d864b876a9af97d78cdf8cf6c9cb261d
        SHA512 f0d40abd02d1db8bfa427c582595216146d614d88f1c7b93d7fff764028069acb61e85c8119802bae1d4159266a295d902c93f434154523c99ab5e80cb0b6d56
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
