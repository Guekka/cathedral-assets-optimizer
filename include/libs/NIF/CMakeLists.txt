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
    utils/KDMatcher.hpp
    utils/Miniball.hpp
    utils/Object3d.cpp
    utils/Object3d.h
    utils/Object3d.hpp
    )

target_compile_features(nif PUBLIC cxx_std_17)
