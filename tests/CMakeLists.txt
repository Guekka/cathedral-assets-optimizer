find_package(bethutil CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(cao_cli_profile_tests cli_profile_tests.cpp)
target_include_directories(cao_cli_profile_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../src")
target_link_libraries(cao_cli_profile_tests
    PRIVATE
        btu::bethutil
        doctest::doctest
        nlohmann_json::nlohmann_json
        Qt6::Core
)

add_test(NAME cao_cli_profile_tests COMMAND cao_cli_profile_tests)
