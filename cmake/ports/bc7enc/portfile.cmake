vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  Guekka/bc7enc_rdo
  REF
  7628f7341edd8a3f483c777ebe71ad85557ee371
  SHA512
  83c97aef64fc6eaa6e1823e40c10f68c9cbe4cc3d62f86990d5baa17d29cfd90ccece3871865afb58158a3d2e588916464f8380139a44b8c6f1247996fc1b348
  HEAD_REF
  master)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME bc7enc CONFIG_PATH "lib/cmake/bc7enc")

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include
     ${CURRENT_PACKAGES_DIR}/debug/share)

file(
  INSTALL "${SOURCE_PATH}/LICENSE"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright)
