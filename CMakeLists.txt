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

project(forcepad_project)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets Svg)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
message(STATUS "Qt6 version: ${Qt6_VERSION}")

find_package(Eigen3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(FLTK CONFIG REQUIRED)


message(STATUS "Project dir: ${PROJECT_BINARY_DIR}")
message(STATUS "FLTK libraries: ${FLTK_LIBRARIES}")
message(STATUS "JPEG library: ${JPEG_LIBRARY}")

add_definitions(-D_CRT_SECURE_NO_WARNINGS)


if(WIN32)
	include_directories(${FLTK_BUILD_DIR} ${FLTK_INCLUDE_DIR} src/calfem src/defs ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
else(WIN32)
	include_directories(/opt/local/include ${FLTK_INCLUDE_DIR} src/calfem src/defs)
endif(WIN32)

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
