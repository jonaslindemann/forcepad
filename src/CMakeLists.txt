add_subdirectory(fplog)
add_subdirectory(calfem)
add_subdirectory(ivf2d)
add_subdirectory(common)

if(BUILD_FLTK_APP)
    add_subdirectory(forcepad)
endif()

add_subdirectory(qtforcepad)
