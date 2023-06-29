vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  Guekka/bc7enc_rdo
  REF
  9352a6ec738430b6ce5dd4e7fe0bc9c3f9fee2d6
  SHA512
  6e0cb10bc8d06651637361718d2555d44c4bdfe393144b1c4abb4c6f5367db30f31e382a1b7ec1789f6678f255192547114331543e3e5c87ec7aa15261b4f613
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
