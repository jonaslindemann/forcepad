include_directories(../forcepad ../common ../ivf2d ../calfemcxx ../newmat ../defs)

add_definitions(-DQT_NO_KEYWORDS -DUSE_QT)

set(SOURCES
    main.cpp
    MainWindow.cpp
    QtPaintView.cpp
    ../forcepad/PaintView.cpp
)

set(HEADERS
    MainWindow.h
    QtPaintView.h
)

qt_add_executable(qtforcepad ${SOURCES} ${HEADERS})

set_target_properties(qtforcepad PROPERTIES DEBUG_POSTFIX d)

target_link_libraries(qtforcepad PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Svg
    common
    ivf2d
    calfemcxx
    newmat
    JPEG::JPEG
    PNG::PNG
    ZLIB::ZLIB
    OpenGL::GL
)

if(WIN32)
    target_link_libraries(qtforcepad PRIVATE winmm comctl32 gdiplus)
endif()
