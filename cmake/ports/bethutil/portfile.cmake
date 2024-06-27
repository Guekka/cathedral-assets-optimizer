vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO Guekka/bethutil
        REF dev
        SHA512 ded622b3c09536f86e8e2d347911124371c5246b0add7ed6d2c34d7f08df2fef2f5d6cb6b060c3e801e0bbea2cd0b4e9a6bcd8b789efac696a9cf6beb5a657f1
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
