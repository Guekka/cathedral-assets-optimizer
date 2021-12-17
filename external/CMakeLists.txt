add_subdirectory(rx-ranges)
add_subdirectory(libbsarch)

function(linkExternal target)
    message("Configuring external libraries:")

    find_package(directxtex CONFIG REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)
    find_package(doctest CONFIG REQUIRED)
    find_package(nifly CONFIG REQUIRED)


    find_path(PLOG_INCLUDE_DIRS "plog/Appenders/AndroidAppender.h")
    add_library(plog INTERFACE)
    target_include_directories(plog INTERFACE ${PLOG_INCLUDE_DIRS})


    target_link_libraries(${target} PUBLIC
        Microsoft::DirectXTex
        libbsarch_OOP
        nifly
        nlohmann_json::nlohmann_json
        plog
        rx-ranges
        doctest::doctest)
endfunction()
