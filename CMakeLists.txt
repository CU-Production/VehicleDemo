cmake_minimum_required(VERSION 4.1)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(VehicleDemo)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# threepp
set(THREEPP_BUILD_TESTS OFF)
set(THREEPP_BUILD_EXAMPLES OFF)
FetchContent_Declare(threepp
    GIT_REPOSITORY https://github.com/markaren/threepp.git
    GIT_TAG e3e81a449ff20dfe64a85c48844e06261f25a29b
)
FetchContent_MakeAvailable(threepp)

# joltphysics
set(DOUBLE_PRECISION OFF)
set(GENERATE_DEBUG_SYMBOLS ON)
set(OVERRIDE_CXX_FLAGS ON)
set(CROSS_PLATFORM_DETERMINISTIC OFF)
set(INTERPROCEDURAL_OPTIMIZATION ON)
set(FLOATING_POINT_EXCEPTIONS_ENABLED OFF)
set(CPP_EXCEPTIONS_ENABLED OFF)
set(CPP_RTTI_ENABLED OFF)
set(OBJECT_LAYER_BITS 16)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)

set(USE_SSE4_1 ON)
set(USE_SSE4_2 ON)
set(USE_AVX ON)
set(USE_AVX2 ON)
set(USE_AVX512 OFF)
set(USE_LZCNT ON)
set(USE_TZCNT ON)
set(USE_F16C ON)
set(USE_FMADD ON)

FetchContent_Declare(JoltPhysics
    GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
    GIT_TAG "v5.5.0"
    SOURCE_SUBDIR "Build"
)
FetchContent_MakeAvailable(JoltPhysics)

add_executable(VehicleDemo main.cpp)
target_link_libraries(VehicleDemo PRIVATE threepp::threepp Jolt)
