vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
        OUT_SOURCE_PATH
        SOURCE_PATH
        REPO
        Guekka/mpsc-channel-in-cxx
        REF
        f71892e19ca647dd2cb4fbc24617a8945092a2d8
        SHA512
        b1537446cc16f69131ff3218e9210067540a5b5ace029fe4d362c7a15a7d8d91dd73b3eef9231296838b26e7151932dacbfc951db2c77a0d67dc2b7c0538d239
        HEAD_REF
        main)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}" OPTIONS -DBUILD_TESTING=OFF
        ${FEATURE_OPTIONS})
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME mpsc_channel CONFIG_PATH "lib/cmake/mpsc_channel")

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include
        ${CURRENT_PACKAGES_DIR}/debug/share)

file(
        INSTALL "${SOURCE_PATH}/LICENSE"
        DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
        RENAME copyright)
