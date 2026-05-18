find_package(Eigen3 REQUIRED)

include_directories(../ivf2d)
file(GLOB SOURCE_FILES1 CONFIGURE_DEPENDS *.cpp)
file(GLOB SOURCE_FILES2 CONFIGURE_DEPENDS *.cxx)
file(GLOB INCLUDE_FILES1 CONFIGURE_DEPENDS *.h)
file(GLOB INCLUDE_FILES2 CONFIGURE_DEPENDS *.H)

# FemGridSolver2 has framework-specific dependencies (FLTK or Qt) so it is
# compiled directly into each executable target rather than into this library.
file(GLOB _remove_femgrid "FemGridSolver2.cpp")
list(REMOVE_ITEM SOURCE_FILES1 ${_remove_femgrid})

add_library(common ${INCLUDE_FILES1} ${INCLUDE_FILES2} ${SOURCE_FILES1} ${SOURCE_FILES2})
set_target_properties(common PROPERTIES DEBUG_POSTFIX d)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(common PUBLIC ivf2d Eigen3::Eigen)
