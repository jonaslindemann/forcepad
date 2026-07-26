if(NOT TARGET Eigen3::Eigen)
    find_package(Eigen3 REQUIRED)
endif()

include_directories(../ivf2d)
file(GLOB SOURCE_FILES1 CONFIGURE_DEPENDS *.cpp)
file(GLOB SOURCE_FILES2 CONFIGURE_DEPENDS *.cxx)
file(GLOB INCLUDE_FILES1 CONFIGURE_DEPENDS *.h)
file(GLOB INCLUDE_FILES2 CONFIGURE_DEPENDS *.H)

# FemGridSolver2 has framework-specific dependencies (FLTK or Qt) so it is
# compiled directly into each executable target rather than into this library.
file(GLOB _remove_femgrid "FemGridSolver2.cpp")
list(REMOVE_ITEM SOURCE_FILES1 ${_remove_femgrid})

# When building without FLTK, exclude all FLTK-specific sources so their
# headers (FL/*.H) are never pulled in during the Qt-only build.
if(NOT BUILD_FLTK_APP)
    file(GLOB _remove_fltk
        "Fl_*.cpp"
        "fl_*.cpp"
        "LogWindow.cpp"
        "NewModelDlg.cpp"
        # v1 FemGrid/ImageGrid are used only by the legacy FLTK app; the Qt build
        # uses FemGrid2/ImageGrid2. Excluding them keeps the (fixed-function)
        # legacy OpenGL they contain out of the modern Qt/WebGL build.
        "FemGrid.cpp"
        "ImageGrid.cpp"
    )
    file(GLOB _remove_fltk_cxx "Fl_*.cxx" "fl_*.cxx")
    list(REMOVE_ITEM SOURCE_FILES1 ${_remove_fltk})
    list(REMOVE_ITEM SOURCE_FILES2 ${_remove_fltk_cxx})
endif()

add_library(common ${INCLUDE_FILES1} ${INCLUDE_FILES2} ${SOURCE_FILES1} ${SOURCE_FILES2})
set_target_properties(common PROPERTIES DEBUG_POSTFIX d)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(common PUBLIC ivf2d Eigen3::Eigen fplog)
if(OpenMP_FOUND)
    target_link_libraries(common PUBLIC OpenMP::OpenMP_CXX)
endif()
