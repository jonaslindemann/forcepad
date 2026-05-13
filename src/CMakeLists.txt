add_subdirectory(newmat)
add_subdirectory(calfemcxx)
add_subdirectory(ivf2d)
add_subdirectory(common)
add_subdirectory(forcepad)

if(BUILD_QT_APP)
    add_subdirectory(qtforcepad)
endif()