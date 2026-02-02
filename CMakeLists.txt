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
set(JPH_DEBUG_RENDERER ON)

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

# imgui
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.0
)
FetchContent_MakeAvailable(imgui)

find_package(OpenGL REQUIRED)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC OpenGL::GL)
if (TARGET glfw)
    target_link_libraries(imgui PUBLIC glfw)
endif ()

add_executable(VehicleDemo
    main.cpp
    JoltDebugRenderer.cpp
    PhysicsWorld.cpp
    PhysicsVehicle.cpp
    VehicleController.cpp
    VehicleFactory.cpp
    TestScene.cpp
)
target_link_libraries(VehicleDemo PRIVATE threepp::threepp Jolt imgui)
target_compile_definitions(VehicleDemo PRIVATE JPH_DEBUG_RENDERER)
