vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  ousnius/nifly
  REF
  3fbe3a0bfc5de0ff86bfc6190c3c4ee6ac189cf7
  SHA512
  45a633458d9b2ef90207a0d23ab57b7489e8e084d15183080545b54ec85b42e4f20f29acb97f2ece1a34b1b5012d8661bbc60cccd092e5fb1370a0f616bffc24
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
