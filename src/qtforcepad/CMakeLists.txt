add_definitions(-DQT_NO_KEYWORDS)

#include(${QT_USE_FILE})

SET(qtforcepad_UIS
    mainwindow.ui
)

SET(qtforcepad_SRCS
  matlabgen.cpp
  FemGridSolver2.cpp
  PaintView.cpp
  qtpaintview.cpp
  mainwindow.cpp
  main.cpp
)

SET(qtforcepad_MOC_HDRS
  forcepad_config.h
  matlabgen.h
  FemGridSolver2.h
  PaintView.h
  qtpaintview.h
  mainwindow.h
)

SET(qtforcepad_RC_SRCS qtforcepad.qrc)

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(qtforcepad_MOC_SRCS ${qtforcepad_MOC_HDRS} )
QT4_WRAP_UI(qtforcepad_UIS_H ${qtforcepad_UIS} )
QT4_ADD_RESOURCES( qtforcepad_RC_SRCS ${qtforcepad_RCS} )

include_directories(${CMAKE_CURRENT_BINARY_DIR} . ../common ../ivf2d)

add_executable(qtforcepad ${qtforcepad_SRCS} ${qtforcepad_MOC_SRCS} ${qtforcepad_RC_SRCS} ${qtforcepad_UIS_H} )

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_link_libraries(qtforcepad ${QT_LIBRARIES} ${OPENGL_LIBRARIES} commond ivf2dd calfemcxxd newmatd ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} winmm.lib)
else(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_link_libraries(qtforcepad ${QT_LIBRARIES} ${OPENGL_LIBRARIES} common ivf2d calfemcxx newmat ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} winmm.lib)
endif(CMAKE_BUILD_TYPE STREQUAL Debug)
