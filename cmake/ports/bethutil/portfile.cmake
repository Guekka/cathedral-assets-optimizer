vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO Guekka/bethutil
        REF 61822b39367e84e37fbccb9d52b06b73f3f84099
        SHA512 40a7c0f8bd5eb60b009169060445ae4b9db2a4c0c793b5cad984935280c67f939a1e9265bead1e6c630c438d8e2fe158394e85e7d6ade23966c4c47323443c25
        HEAD_REF main
)

set(SOURCE_PATH "/home/edgar/code/btu")

vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}"
        OPTIONS
        -DBUILD_TESTING=OFF
)
vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/bethutil")

file(REMOVE_RECURSE
        ${CURRENT_PACKAGES_DIR}/debug/include
        ${CURRENT_PACKAGES_DIR}/debug/share
)

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
