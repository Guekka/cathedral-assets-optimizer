Project(nif)

cmake_minimum_required(VERSION 3.15)

add_library(nif STATIC
    Animation.cpp
    Animation.h
    BasicTypes.cpp
    BasicTypes.h
    bhk.cpp
    bhk.h
    ExtraData.cpp
    ExtraData.h
    Factory.cpp
    Factory.h
    Geometry.cpp
    Geometry.h
    Keys.h
    NifFile.cpp
    NifFile.h
    NifUtil.h
    Nodes.cpp
    Nodes.h
    Objects.cpp
    Objects.h
    Particles.cpp
    Particles.h
    Shaders.cpp
    Shaders.h
    Skin.cpp
    Skin.h
    VertexData.h
    utils/half.hpp
    utils/KDMatcher.h
    utils/Miniball.hpp
    utils/Object3d.cpp
    utils/Object3d.h
    )

target_compile_features(nif PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(nif PUBLIC "/EHsc")
endif()

