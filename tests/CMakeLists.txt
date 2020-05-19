add_executable(CAO_test
    main.test.cpp
    utils.hpp
    File.test.cpp
    TextureConvert.test.cpp
    TextureFile.test.cpp
    TextureResize.test.cpp)

target_link_libraries(CAO_test CAO_LIB)
