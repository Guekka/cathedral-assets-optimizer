vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  Guekka/mpsc-channel-in-cxx
  REF
  cdd4569b58b0518cefce10a99c5a47d7cae78891
  SHA512
  d6f9b0b539066b74eb0c43d391c99a2892c42780dba8a0ae5a28ad20bfd016329d0be237398a1db51d8b839f56152afbf3adea9173ded5a742a36a94001dc5d6
  HEAD_REF
  main)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}" OPTIONS -DBUILD_TESTING=OFF
                      ${FEATURE_OPTIONS})
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME mpsc_channel CONFIG_PATH
                         "lib/cmake/mpsc_channel")

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include
     ${CURRENT_PACKAGES_DIR}/debug/share)

file(
  INSTALL "${SOURCE_PATH}/LICENSE"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright)
