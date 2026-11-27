find_package(bethutil CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
add_executable(cao_cli_profile_tests cli_profile_tests.cpp)
target_include_directories(cao_cli_profile_tests
                           PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../src")
target_link_libraries(
  cao_cli_profile_tests PRIVATE btu::bethutil doctest::doctest
                                nlohmann_json::nlohmann_json Qt6::Core)

add_test(NAME cao_cli_profile_tests COMMAND cao_cli_profile_tests)
find_package(platform_folders CONFIG REQUIRED)
find_package(
  Qt6
  COMPONENTS Core
  REQUIRED)

add_executable(
  CAO_test PatternTests.cpp ${PROJECT_SOURCE_DIR}/src/settings/profile.cpp
           ${PROJECT_SOURCE_DIR}/src/settings/settings.cpp)
target_include_directories(CAO_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(CAO_test PRIVATE cxx_std_20)
target_link_libraries(
  CAO_test PRIVATE btu::bethutil doctest::doctest Qt6::Core
                   nlohmann_json::nlohmann_json sago::platform_folders)

add_test(NAME CAO_test COMMAND CAO_test)
