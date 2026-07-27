cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(_forcepad_vcpkg_roots)

    if(DEFINED ENV{VCPKG_ROOT})
        file(TO_CMAKE_PATH "$ENV{VCPKG_ROOT}" _forcepad_env_vcpkg_root)
        list(APPEND _forcepad_vcpkg_roots "${_forcepad_env_vcpkg_root}")
    endif()

    if(WIN32)
        list(APPEND _forcepad_vcpkg_roots
            "C:/vcpkg"
            "E:/vcpkg"
        )
    elseif(APPLE)
        list(APPEND _forcepad_vcpkg_roots
            "$ENV{HOME}/vcpkg"
            "/Users/lindemann/vcpkg"
        )
    else()
        list(APPEND _forcepad_vcpkg_roots
            "$ENV{HOME}/vcpkg"
            "/opt/vcpkg"
            "/home/bmjl/vcpkg"
        )
    endif()

    list(APPEND _forcepad_vcpkg_roots "${CMAKE_SOURCE_DIR}/vcpkg")
    list(REMOVE_DUPLICATES _forcepad_vcpkg_roots)

    foreach(_forcepad_vcpkg_root IN LISTS _forcepad_vcpkg_roots)
        set(_forcepad_vcpkg_toolchain "${_forcepad_vcpkg_root}/scripts/buildsystems/vcpkg.cmake")
        if(EXISTS "${_forcepad_vcpkg_toolchain}" AND EXISTS "${_forcepad_vcpkg_root}/ports")
            set(CMAKE_TOOLCHAIN_FILE "${_forcepad_vcpkg_toolchain}" CACHE FILEPATH "vcpkg toolchain file")
            message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
            break()
        endif()
    endforeach()
endif()

option(BUILD_FLTK_APP "Build the legacy FLTK forcepad application" OFF)

project(forcepad_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(APPLE)
    # vcpkg's qtbase port builds Qt from source on macOS, which is slow and, as
    # of the current vcpkg baseline, broken against recent Xcode/libc++
    # (harfbuzz's -D_LIBCPP_ENABLE_ASSERTIONS flag was removed from newer
    # libc++ headers). qtbase/qtsvg are therefore excluded from vcpkg.json on
    # macOS ("platform": "!osx") and Qt6 is expected from a prebuilt Homebrew
    # install instead; the other dependencies still come from vcpkg.
    foreach(_forcepad_brew_qt "/opt/homebrew/opt/qt" "/usr/local/opt/qt")
        if(EXISTS "${_forcepad_brew_qt}/lib/cmake/Qt6")
            list(APPEND CMAKE_PREFIX_PATH "${_forcepad_brew_qt}")
        endif()
    endforeach()

    # CMake auto-adds /opt/homebrew to its default search prefixes on Apple
    # Silicon. If mesa/mesa-glu/glew happen to be installed there (a common
    # transitive pull-in, unrelated to this project), find_package(OpenGL)
    # resolves OPENGL_gl_LIBRARY to Homebrew's Mesa libGL.dylib instead of the
    # system OpenGL.framework — and since Qt6Gui's own CMake config
    # (find_dependency(WrapOpenGL) inside Qt6GuiConfig.cmake) triggers the
    # very first find_package(OpenGL) call as a side effect of find_package(Qt6)
    # below, this must be forced BEFORE that call, not just before our own
    # later find_package(OpenGL REQUIRED) — once the OpenGL::GL imported
    # target exists, its IMPORTED_LOCATION is locked in and a later cache
    # override has no effect. Mixing the two GL implementations in one
    # process fails at launch (missing libgallium-*.dylib / X11 deps that
    # never get bundled).
    # The on-disk /System/Library/Frameworks/OpenGL.framework has no Headers
    # subdirectory on modern macOS (headers only ship inside the SDK bundled
    # with Xcode/CLT), so the framework path must be resolved through the
    # active SDK rather than hardcoded.
    execute_process(
        COMMAND xcrun --sdk macosx --show-sdk-path
        OUTPUT_VARIABLE _forcepad_macos_sdk
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    set(_forcepad_opengl_framework "${_forcepad_macos_sdk}/System/Library/Frameworks/OpenGL.framework")
    if(EXISTS "${_forcepad_opengl_framework}/Headers")
        set(OPENGL_INCLUDE_DIR "${_forcepad_opengl_framework}/Headers" CACHE PATH "" FORCE)
        set(OPENGL_gl_LIBRARY "${_forcepad_opengl_framework}" CACHE FILEPATH "" FORCE)
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets Svg)
# Desktop only: downloads a model given as an http(s) command-line argument.
# The wasm build uses emscripten's XHR wrapper instead (see ModelUrlLoader.cpp).
if(NOT EMSCRIPTEN)
    find_package(Qt6 REQUIRED COMPONENTS Network)
endif()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
message(STATUS "Qt6 version: ${Qt6_VERSION}")

if(EMSCRIPTEN)
    # Qt-for-WebAssembly build: no vcpkg. Provide Eigen3::Eigen, spdlog::spdlog
    # and the ZLIB/PNG/JPEG Emscripten-port targets here. No OpenMP (single-
    # threaded wasm) and no system OpenGL (Qt supplies WebGL via its own libs).
    include(${CMAKE_SOURCE_DIR}/cmake/WasmDeps.cmake)
else()
    find_package(Eigen3 REQUIRED)
    find_package(OpenMP COMPONENTS CXX)
    if(APPLE AND NOT OpenMP_FOUND)
        # AppleClang ships no OpenMP runtime, so plain find_package(OpenMP)
        # always fails here even if libomp is installed — it needs to be
        # pointed at the separate frontend flag/library explicitly. Hint it
        # at Homebrew's libomp (`brew install libomp`) if present; otherwise
        # this is a no-op and the build stays single-threaded, same as before.
        foreach(_forcepad_brew_omp "/opt/homebrew/opt/libomp" "/usr/local/opt/libomp")
            if(EXISTS "${_forcepad_brew_omp}/include/omp.h")
                set(OpenMP_CXX_FLAGS "-Xclang -fopenmp -I${_forcepad_brew_omp}/include")
                set(OpenMP_CXX_LIB_NAMES "omp")
                set(OpenMP_omp_LIBRARY "${_forcepad_brew_omp}/lib/libomp.dylib")
                find_package(OpenMP COMPONENTS CXX)
                break()
            endif()
        endforeach()
    endif()
    find_package(spdlog CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(PNG REQUIRED)
    find_package(JPEG REQUIRED)
    if(BUILD_FLTK_APP)
        find_package(FLTK CONFIG REQUIRED)
    endif()
endif()

message(STATUS "Project dir: ${PROJECT_BINARY_DIR}")
if(BUILD_FLTK_APP)
    message(STATUS "FLTK libraries: ${FLTK_LIBRARIES}")
endif()
message(STATUS "JPEG library: ${JPEG_LIBRARY}")

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
if(WIN32)
    add_definitions(-DNOMINMAX)
endif()


if(WIN32)
    if(BUILD_FLTK_APP)
        include_directories(${FLTK_BUILD_DIR} ${FLTK_INCLUDE_DIR} src/calfem src/defs ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
    else()
        include_directories(src/calfem src/defs ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
    endif()
else()
    if(BUILD_FLTK_APP)
        include_directories(/opt/local/include ${FLTK_INCLUDE_DIR} src/calfem src/defs)
    else()
        include_directories(/opt/local/include src/calfem src/defs)
    endif()
endif()

if (APPLE)
	link_directories(/opt/local/lib ${PROJECT_SOURCE_DIR}/lib )
else(APPLE)
	if(WIN32)
		link_directories(${PROJECT_SOURCE_DIR}/lib ) 
	else()
		link_directories(${PROJECT_SOURCE_DIR}/lib ) 
	endif()
endif(APPLE)

#link_directories(${PROJECT_SOURCE_DIR}/../fltk-1.1.8/build_xcode/bin ${PROJECT_SOURCE_DIR}/../calfemcxx-1.0/lib ${PROJECT_SOURCE_DIR}/../newmat-10.A/lib)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

if(WIN32)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
else(WIN32)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/release)
endif(WIN32)

if(CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Configuration types: ${CMAKE_CONFIGURATION_TYPES}")
else()
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()

add_subdirectory(src)

# Opt-in guard target: fails if legacy / fixed-function OpenGL reappears in the
# migrated (Qt6 + WebGL-targeted) render path. Run with:
#   cmake --build <dir> --target check_legacy_gl
# Suitable for CI. Not part of the default build (and skipped if bash is absent).
find_program(BASH_PROGRAM bash)
if(BASH_PROGRAM)
    add_custom_target(check_legacy_gl
        COMMAND ${BASH_PROGRAM} ${CMAKE_SOURCE_DIR}/scripts/check-legacy-gl.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Checking for legacy/fixed-function OpenGL in the migrated render path")
endif()
