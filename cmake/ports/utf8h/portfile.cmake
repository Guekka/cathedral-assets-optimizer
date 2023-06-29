vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  sheredom/utf8.h
  REF
  a35aefddd730e87f8c88a6aecf4da2dac6c04604
  SHA512
  219e91acd363dfd07eeaaef2c0deec86cdd8ed1b497fa510c3d5c6140b28ea380c5f2f8366e87e88fbe40b6f508323e2427b8131e162acd46bd4ff58a732801d
  HEAD_REF
  master)

file(COPY "${SOURCE_PATH}/utf8.h"
     DESTINATION "${CURRENT_PACKAGES_DIR}/include/utf8h")

file(
  INSTALL "${SOURCE_PATH}/LICENSE"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright)
