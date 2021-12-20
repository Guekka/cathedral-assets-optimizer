vcpkg_fail_port_install(ON_TARGET "OSX" "UWP" ON_ARCH "x86")
vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  Guekka/bsa
  REF
  6118820b5f8572ce09b6efc77efe4dd344460ed6
  SHA512
  e6d4f110a060ee50c3eab62f54c1dc75b4117c73b1c5e32c8c0df76fc49f8065b6f3f4a8ae640605a991776eb8e7990ded9fe9853d9ae594112333e1da64cb75
  HEAD_REF
  master)

if(VCPKG_TARGET_IS_LINUX)
  message(WARNING "Build ${PORT} requires at least gcc 10.")
endif()

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS FEATURES xmem
                     BSA_SUPPORT_XMEM)

if(BSA_SUPPORT_XMEM)
  vcpkg_fail_port_install(ON_TARGET "LINUX" MESSAGE
                          "XMem support is only available for windows")
endif()

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
