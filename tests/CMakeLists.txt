add_executable(
  CAO_test
  Algorithms.test.cpp
  main.test.cpp
  utils.hpp
  BSA.test.cpp
  File.test.cpp
  JSON.test.cpp
  TextureAddAlpha.test.cpp
  TextureConvert.test.cpp
  TextureFile.test.cpp
  TextureResize.test.cpp)

target_link_libraries(CAO_test CAO_LIB)
