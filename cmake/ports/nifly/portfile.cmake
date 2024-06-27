vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  ousnius/nifly
  REF
  868b648b72392d3f8f582a28f819253dc5c6063e
  SHA512
  da76ba7ea47c65aab685ab7e83e62ddfe9975ffe23b34b6971e20d69d693859668a61beb77dccfdff4e6d9f4adf984ec9ccb394afc591932936ec8b888914ffb
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
