message("Configuring external libraries:")

message("-- plog")
add_subdirectory(plog)

message("-- libbsarch")
add_subdirectory(libbsarch)

message("-- nif")
add_subdirectory(NIF)

message("-- DirectXTex")
add_subdirectory(DirectXTex)

message("-- nlohmann/json")
add_subdirectory(json)

message("-- doctest")
add_library(doctest doctest/doctest.h)
set_target_properties(doctest PROPERTIES LINKER_LANGUAGE CXX)

message("-- rx-ranges")
add_subdirectory(rx-ranges)

function(getExternalLibrariesList list)
    set(${list}
        directxtex
        libbsarch_OOP
        nif
        nlohmann_json::nlohmann_json
        plog
        rx-ranges
        PARENT_SCOPE)
endfunction()
