vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  Ryan-rsm-McKenzie/bsa
  REF
  99e12f907bc1e47deb9d9cc025c3ea2c8fab7faf
  SHA512
  8396ee693e20fdd1fdd610cba16c6e3226c4d7bdbeae36d6a93dd0e11a6f50f98bebfb51f09364d999ad9529d0c4e1f809dea4f2578208f851f0ad9980d56130
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
