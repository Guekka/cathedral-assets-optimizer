vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  Guekka/bethutil
  REF
  5f1ab07d7e2b4edf7980dccc9be87f829ecc9d52
  SHA512
  0834d60ec1634b7c5671ced65e97b32b8b33bcaeb9819c2fc15d614c933502c98061d16a7cd3d0315ae643b871fe45ad02545d7b1cd3442f6174c8a982c9ebc5
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
