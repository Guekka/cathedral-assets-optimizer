add_executable(CAO_test
    main.test.cpp
    utils.hpp
    BSA.test.cpp
    File.test.cpp
    JSON.test.cpp
    PatternMap.test.cpp
    TextureConvert.test.cpp
    TextureFile.test.cpp
    TextureResize.test.cpp)

target_link_libraries(CAO_test CAO_LIB)
