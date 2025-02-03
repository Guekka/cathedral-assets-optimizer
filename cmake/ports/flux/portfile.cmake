vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  tcbrindle/flux
  REF
        504a107204a2fc3cd67eada992b987b21dbbbbd2
  SHA512
        53bf6d48a297231f2037758e3a4d58c131d60aa16f8a18f9672c8327f90e9b690ec1923570301d55df037ab3ff46ac562c33e0fbacf6d8d94c78aa73e7661e5d
  HEAD_REF
  main)

# Copy header files
file(
  INSTALL ${SOURCE_PATH}/include
  DESTINATION ${CURRENT_PACKAGES_DIR}
  FILES_MATCHING
        PATTERN "*.h*") # hpp and h

file(
  INSTALL "${SOURCE_PATH}/LICENSE_1_0.txt"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright)
