vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  ousnius/nifly
  REF
  864b6c37111a027596f2a60e45dca212d040542c
  SHA512
  3d79399eea9640736a56c39c0f96a5c51e44e7a415651cd5e61b9b11fc2d6195d09d7a28db1e3fbaa3e84f70a274cb3c26e6ddd8984cce3092cfec0d968d4076
  HEAD_REF
  main)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}" OPTIONS -DBUILD_TESTING=OFF)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH "cmake/")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include"
     "${CURRENT_PACKAGES_DIR}/debug/share")

file(
  INSTALL "${SOURCE_PATH}/LICENSE"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright)
