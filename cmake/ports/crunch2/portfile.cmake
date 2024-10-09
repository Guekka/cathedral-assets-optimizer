vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  FrozenStormInteractive/Crunch2
  REF
  d5cd924754a1b39ea597335a979273ce71ef95a3
  SHA512
  56095fbbea8fe405258434a1b2d14cf29d634e4b5c22d4ae6002db4641d8ff8ee74ae7ce4e47781e5b4bc4bb248055fead8921389df3e82bb9fc4dd1a98a4a28
  HEAD_REF
  master
  PATCHES
  "fix_linux_compile.patch"
  "fix_cmake.patch")

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")

vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/crunch2")

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include
     ${CURRENT_PACKAGES_DIR}/debug/share)

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
  file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin"
       "${CURRENT_PACKAGES_DIR}/debug/bin")
endif()

file(
  INSTALL "${SOURCE_PATH}/license.txt"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/crunch2"
  RENAME copyright)
