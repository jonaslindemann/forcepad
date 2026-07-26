# WasmDeps.cmake — dependency provisioning for the Qt-for-WebAssembly build.
#
# vcpkg does not build for wasm32-emscripten in any practical way, so the wasm
# build (configured via Qt's `qt-cmake`, which chainloads the Emscripten
# toolchain) gets its non-Qt dependencies here instead:
#
#   * Eigen3  — header-only, fetched (pinned to the desktop/vcpkg version).
#   * spdlog  — fetched and compiled for wasm (pinned to the desktop version).
#   * zlib / libpng / libjpeg — Emscripten *ports* (`-sUSE_ZLIB/USE_LIBPNG/
#     USE_LIBJPEG`), exposed as the same imported-target names the rest of the
#     build already links (ZLIB::ZLIB, PNG::PNG, JPEG::JPEG).
#
# Include this from the top-level CMakeLists when EMSCRIPTEN, *before*
# add_subdirectory(src). The per-library CMakeLists guard their find_package()
# calls with `if(NOT TARGET <target>)`, so the targets created here satisfy them.

include(FetchContent)

# --- Eigen3 (header-only) -----------------------------------------------------
# Avoid add_subdirectory(Eigen): its CMake pulls in tests/BLAS probing/install
# rules we don't want. Populate the headers and expose a plain INTERFACE target.
FetchContent_Declare(eigen
    URL      https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
    URL_HASH SHA256=8586084f71f9bde545ee7fa6d00288b264a2b7ac3607b974e54d13e7162c1c72
)
FetchContent_GetProperties(eigen)
if(NOT eigen_POPULATED)
    FetchContent_Populate(eigen)
endif()
add_library(_fp_eigen INTERFACE)
target_include_directories(_fp_eigen SYSTEM INTERFACE "${eigen_SOURCE_DIR}")
add_library(Eigen3::Eigen ALIAS _fp_eigen)

# --- spdlog (compiled for wasm) ----------------------------------------------
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL        OFF CACHE BOOL "" FORCE)
FetchContent_Declare(spdlog
    URL      https://github.com/gabime/spdlog/archive/refs/tags/v1.17.0.tar.gz
)
FetchContent_MakeAvailable(spdlog)   # defines spdlog::spdlog

# --- Emscripten ports: zlib / libpng / libjpeg -------------------------------
# Each port needs its `-sUSE_*` flag at BOTH compile time (for the headers,
# e.g. <png.h>, <jpeglib.h>) and link time. INTERFACE targets propagate both to
# every consumer, mirroring the imported targets the desktop build gets from
# find_package. libpng's port already pulls in zlib.
function(_fp_add_emscripten_port target_alias flag)
    string(MAKE_C_IDENTIFIER "_fp_port_${target_alias}" _tgt)
    add_library(${_tgt} INTERFACE)
    target_compile_options(${_tgt} INTERFACE "SHELL:-s ${flag}")
    target_link_options(${_tgt}    INTERFACE "SHELL:-s ${flag}")
    add_library(${target_alias} ALIAS ${_tgt})
endfunction()

_fp_add_emscripten_port(ZLIB::ZLIB USE_ZLIB=1)
_fp_add_emscripten_port(PNG::PNG   USE_LIBPNG=1)
_fp_add_emscripten_port(JPEG::JPEG USE_LIBJPEG=1)

message(STATUS "WasmDeps: Eigen3::Eigen, spdlog::spdlog, and ZLIB/PNG/JPEG Emscripten ports configured")
