include_directories(../common ../ivf2d)

add_definitions(-DQT_NO_KEYWORDS)

SET(qtforcepad_FORMS
    mainwindow.ui
)

SET(qtforcepad_SOURCES
  matlabgen.cpp
  FemGridSolver2.cpp
  PaintView.cpp
  qtpaintview.cpp
  mainwindow.cpp
  main.cpp
)

SET(qtforcepad_HEADERS 
	matlabgen.h
	FemGridSolver2.h
	PaintView.h
	qtpaintview.h
	mainwindow.h
)



add_executable(qtforcepad ${qtforcepad_SOURCES} qtforcepad.qrc)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
if (WIN32)
    target_link_libraries(qtforcepad Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::Svg commond ivf2dd calfemcxxd newmatd ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} ${RESOURCES_ADDED} winmm.lib )
else()
    target_link_libraries(qtforcepad Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::Svg "-framework Cocoa" "-framework ApplicationServices" "-framework AGL" "-framework OpenGL" commond ivf2dd calfemcxxd newmatd ${JPEG_LIBRARIES} ${PNG_LIBRARIES} )
endif()
else(CMAKE_BUILD_TYPE STREQUAL Debug)
if (WIN32)
    target_link_libraries(qtforcepad Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::Svg common ivf2d calfemcxx newmat ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} winmm.lib )
else()
    target_link_libraries(qtforcepad Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::Svg "-framework Cocoa" "-framework ApplicationServices" "-framework AGL" "-framework OpenGL" common ivf2d calfemcxx newmat ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} )
endif()
endif(CMAKE_BUILD_TYPE STREQUAL Debug)
