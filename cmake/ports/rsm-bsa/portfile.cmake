vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  Ryan-rsm-McKenzie/bsa
  REF
  938c6b5c960f5ba9c4f457f34a41665fd38023e0
  SHA512
  f00508678be91302b597c19b91968960c9d1aefbfdaded68b9d1f6648a1955170eb56d7c4a55d3a159d54158181a9b6dfbf0a34c958b2cd494e1674ad1766f2a
  HEAD_REF
  master)

if(VCPKG_TARGET_IS_LINUX)
  message(WARNING "Build ${PORT} requires at least gcc 10.")
endif()

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
