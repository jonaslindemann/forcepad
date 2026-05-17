include_directories(../forcepad ../common ../ivf2d ../calfemcxx ../newmat ../defs)

add_definitions(-DQT_NO_KEYWORDS -DUSE_QT)

set(SOURCES
    main.cpp
    MainWindow.cpp
    QtPaintView.cpp
    CalcSettingsDialog.cpp
    GeneralSettingsDialog.cpp
    AboutDialog.cpp
    ../forcepad/PaintView.cpp
    ../common/FemGridSolver2.cpp
)

set(HEADERS
    MainWindow.h
    QtPaintView.h
    CalcSettingsDialog.h
    GeneralSettingsDialog.h
    AboutDialog.h
)

qt_add_executable(qtforcepad ${SOURCES} ${HEADERS})

qt_add_resources(qtforcepad "qtforcepad_resources"
    PREFIX "/"
    FILES ../../images/splash.png
)

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

    # Copy Qt plugin subdirectories next to the executable.
    # vcpkg's applocal.ps1 handles the Qt DLLs; we copy the plugin dirs it misses.
    set(_vcpkg_debug   "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug")
    set(_vcpkg_release "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows")
    set(_vcpkg    "$<IF:$<CONFIG:Debug>,${_vcpkg_debug},${_vcpkg_release}>")
    set(_qt_plugins "${_vcpkg}/Qt6/plugins")
    add_custom_command(TARGET qtforcepad POST_BUILD
        # Plugin subdirectories (applocal.ps1 only copies top-level DLLs)
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${_qt_plugins}/platforms"
            "$<TARGET_FILE_DIR:qtforcepad>/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${_qt_plugins}/styles"
            "$<TARGET_FILE_DIR:qtforcepad>/styles"
        # Qt6Svg is a direct link dep but applocal.ps1 sometimes misses it
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_vcpkg}/bin/$<IF:$<CONFIG:Debug>,Qt6Svgd.dll,Qt6Svg.dll>"
            "$<TARGET_FILE_DIR:qtforcepad>/"
        COMMENT "Copying Qt plugins and missing DLLs..."
    )
endif()
