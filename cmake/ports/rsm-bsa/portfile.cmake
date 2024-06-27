vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  Ryan-rsm-McKenzie/bsa
  REF
  starfield
  SHA512
  bb91206c3469fc7d896df77272305660ddd71b20f0ae4ddd61b08c11e8618355a3a1d234903adfebffbc91c7b1dea336c1e288acba832bd1bd036ebfc7ec5065
  HEAD_REF
  master)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS FEATURES xmem
                     BSA_SUPPORT_XMEM)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}" OPTIONS -DBUILD_TESTING=OFF
                      ${FEATURE_OPTIONS})
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME bsa CONFIG_PATH "lib/cmake/bsa")

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include
     ${CURRENT_PACKAGES_DIR}/debug/share)

file(
  INSTALL "${SOURCE_PATH}/LICENSE"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright)
