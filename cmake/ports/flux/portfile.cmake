vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  tcbrindle/flux
  REF
  caa30bd4925d25f5bfed7973e265e8eb142d909d
  SHA512
  bcaf31df73aca84afdfaafb54afb5bdcf34cf299c575a636c1d05c4ec00b33fada4b18137dd21532e7de129f92c6fa628f031de1a53a95731180ec79f9b71b57
  HEAD_REF
  main)

# Copy header files
file(
  INSTALL ${SOURCE_PATH}/include
  DESTINATION ${CURRENT_PACKAGES_DIR}
  FILES_MATCHING
  PATTERN "*.hpp")

file(
        INSTALL "${SOURCE_PATH}/LICENSE_1_0.txt"
        DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
        RENAME copyright)
