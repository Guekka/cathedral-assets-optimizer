vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  Guekka/libflow
  REF
  977b0a02b8e66bf13533c3c01016c4b8b2b64cf1
  SHA512
  2aee42ac7fc343631741709ea68bf8ea60c94bddee17884dc73faa8ef9f59dc8c431d52f2fcbe96c1d3e7f831477a821d489249f49ef7adf5a2b7fb2927c36d1
  HEAD_REF
  master)

# Put the licence file where vcpkg expects it
file(COPY ${CMAKE_CURRENT_LIST_DIR}/LICENSE
     DESTINATION ${CURRENT_PACKAGES_DIR}/share/libflow)
file(RENAME ${CURRENT_PACKAGES_DIR}/share/libflow/LICENSE
     ${CURRENT_PACKAGES_DIR}/share/libflow/copyright)

# Copy header files
file(
  INSTALL ${SOURCE_PATH}/include
  DESTINATION ${CURRENT_PACKAGES_DIR}
  FILES_MATCHING
  PATTERN "*.hpp")
