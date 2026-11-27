vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  Guekka/bethutil
  REF
  e6ec7504368fd5da1bced1bfbdc1c6bb629313c6
  SHA512
  90f4a23831bb5f6c6a8daab3862f510263847beb3d78f7b01ed2ba523da1d186c12e2f08c6a4708e433d49bed9c8311e6eee2bc4c5105ee6bf5addaa89f5aa6a
  HEAD_REF
  main)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}" OPTIONS -DBUILD_TESTING=OFF)
vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/bethutil")

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include
     ${CURRENT_PACKAGES_DIR}/debug/share)

file(
  INSTALL "${SOURCE_PATH}/LICENSE"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright)
