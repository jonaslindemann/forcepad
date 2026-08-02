file(GLOB SOURCE_FILES CONFIGURE_DEPENDS *.cpp)
file(GLOB INCLUDE_FILES CONFIGURE_DEPENDS *.h)

if(NOT TARGET JPEG::JPEG)
    find_package(JPEG REQUIRED)
endif()

# The modern-GL rendering layer (GLProgram / Renderer2D / StreamTexture) uses
# Qt's QOpenGLExtraFunctions for GL entry-point resolution, which works on both
# desktop OpenGL and Qt for WebAssembly (WebGL 2) with no separate GL loader.
find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL)

add_library(ivf2d ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(ivf2d PROPERTIES DEBUG_POSTFIX d)
target_include_directories(ivf2d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Raised warning level, scoped to this target only (the modernisation cleared
# ivf2d's backlog; the other libraries have not been through it yet). The old
# CommonDefs.h used to blanket-disable C4244/C4305/C4800 for the whole project,
# which is exactly the class of narrowing bug this is meant to surface.
if(MSVC)
    target_compile_options(ivf2d PRIVATE /W4)
else()
    target_compile_options(ivf2d PRIVATE -Wall -Wextra)
endif()
target_link_libraries(ivf2d
    PUBLIC
        # Image loaders live here: JpegImage.cpp / PngImage.cpp need the jpeg,
        # png and zlib headers at compile time. On desktop these come from the
        # find_package imported targets; on wasm they are Emscripten-port
        # INTERFACE targets (WasmDeps.cmake) that add -sUSE_LIBJPEG/LIBPNG/ZLIB,
        # which is what makes <png.h>/<jpeglib.h> resolve under em++.
        JPEG::JPEG
        PNG::PNG
        ZLIB::ZLIB
    PRIVATE
        # Qt is linked PRIVATE so its INTERFACE compile definitions (notably
        # UNICODE/_UNICODE on Windows) do not leak into consumers such as
        # `common`, whose Win32 code (SystemInfo.cpp) assumes the ANSI TCHAR
        # mapping. Consumers that include the modern-GL headers (Renderer2D.h
        # etc.) link Qt themselves, so they still get the include paths/symbols.
        Qt6::Core
        Qt6::Gui
        Qt6::OpenGL
        fplog
)
