include_directories(../common ../ivf2d)
file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h)

file(GLOB to_remove_cpp "MainFrame2_kiosk.cpp")
file(GLOB to_remove_h "MainFrame2_kiosk.h")
list(REMOVE_ITEM SOURCE_FILES ${to_remove_cpp})
list(REMOVE_ITEM INCLUDE_FILES ${to_remove_h})

file(GLOB to_remove_cpp "TabletToolbar_kiosk.cpp")
file(GLOB to_remove_h "TabletToolbar_kiosk.h")
list(REMOVE_ITEM SOURCE_FILES ${to_remove_cpp})
list(REMOVE_ITEM INCLUDE_FILES ${to_remove_h})

#if(CMAKE_BUILD_TYPE STREQUAL Debug)
#	add_executable(forcepadd ${INCLUDE_FILES} ${SOURCE_FILES})
#else(CMAKE_BUILD_TYPE STREQUAL Debug)

if(APPLE)
add_executable(
	forcepad 
	MACOSX_BUNDLE
	${SOURCE_FILES}
	../../bin/release/brushes/rbrush16.rgb
	../../bin/release/brushes/rbrush32.rgb
	../../bin/release/brushes/rbrush4.rgb
	../../bin/release/brushes/rbrush64.rgb
	../../bin/release/brushes/rbrush8.rgb
	../../bin/release/brushes/sbrush16.rgb
	../../bin/release/brushes/sbrush32.rgb
	../../bin/release/brushes/sbrush4.rgb
	../../bin/release/brushes/sbrush64.rgb
	../../bin/release/brushes/sbrush8.rgb
		../../bin/release/colormaps/colormap1.map
		../../bin/release/colormaps/colormap2.map
		../../images/forcepad_icon.icns
	../../bin/release/kiosk/ex1.fp2
	../../bin/release/kiosk/ex1.png
	../../bin/release/kiosk/ex2.fp2
	../../bin/release/kiosk/ex2.png
	../../bin/release/kiosk/ex3.fp2
	../../bin/release/kiosk/ex3.png
	../../bin/release/kiosk/ex4.fp2
	../../bin/release/kiosk/ex4.png
	../../bin/release/kiosk/ex5.fp2
	../../bin/release/kiosk/ex5.png
	../../bin/release/kiosk/ex6.fp2
	../../bin/release/kiosk/ex6.png
	../../bin/release/kiosk/ex7.fp2
	../../bin/release/kiosk/ex7.png
	../../bin/release/kiosk/ex8.fp2
	../../bin/release/kiosk/ex8.png
	../../bin/release/kiosk/ex9.fp2
	../../bin/release/kiosk/ex9.png

)

set( MACOSX_BUNDLE_ICON_FILE forcepad_icon.icns )
set( MACOSX_BUNDLE_SHORT_VERSION_STRING 2.4 )
set( MACOSX_BUNDLE_VERSION 2.4 )
set( MACOSX_BUNDLE_LONG_VERSION_STRING 2.4 )

set_source_files_properties(
		../../bin/release/brushes/rbrush16.rgb
		../../bin/release/brushes/rbrush32.rgb
		../../bin/release/brushes/rbrush4.rgb
		../../bin/release/brushes/rbrush64.rgb
		../../bin/release/brushes/rbrush8.rgb
		../../bin/release/brushes/sbrush16.rgb
		../../bin/release/brushes/sbrush32.rgb
		../../bin/release/brushes/sbrush4.rgb
		../../bin/release/brushes/sbrush64.rgb
		../../bin/release/brushes/sbrush8.rgb
	PROPERTIES
	MACOSX_PACKAGE_LOCATION MacOS/brushes
)
set_source_files_properties(
		../../bin/release/colormaps/colormap1.map
		../../bin/release/colormaps/colormap2.map
	PROPERTIES
	MACOSX_PACKAGE_LOCATION MacOS/colormaps
)
set_source_files_properties(
		../../images/forcepad_icon.icns
	PROPERTIES
	MACOSX_PACKAGE_LOCATION Resources
)
set_source_files_properties(
	../../bin/release/kiosk/ex1.fp2
	../../bin/release/kiosk/ex1.png
	../../bin/release/kiosk/ex2.fp2
	../../bin/release/kiosk/ex2.png
	../../bin/release/kiosk/ex3.fp2
	../../bin/release/kiosk/ex3.png
	../../bin/release/kiosk/ex4.fp2
	../../bin/release/kiosk/ex4.png
	../../bin/release/kiosk/ex5.fp2
	../../bin/release/kiosk/ex5.png
	../../bin/release/kiosk/ex6.fp2
	../../bin/release/kiosk/ex6.png
	../../bin/release/kiosk/ex7.fp2
	../../bin/release/kiosk/ex7.png
	../../bin/release/kiosk/ex8.fp2
	../../bin/release/kiosk/ex8.png
	../../bin/release/kiosk/ex9.fp2
	../../bin/release/kiosk/ex9.png
	PROPERTIES
	MACOSX_PACKAGE_LOCATION MacOS/kiosk
)
else(APPLE)
	if (CMAKE_BUILD_TYPE STREQUAL Release)
		add_executable(forcepad ${INCLUDE_FILES} ${SOURCE_FILES})
	else()
		add_executable(forcepadd ${INCLUDE_FILES} ${SOURCE_FILES})
	endif()
endif(APPLE)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	target_link_libraries(forcepadd PRIVATE commond ivf2dd fltk fltk_gl fltk_forms fltk_images JPEG::JPEG PNG::PNG ZLIB::ZLIB calfemcxxd newmatd wsock32 comctl32 winmm gdiplus)
else(CMAKE_BUILD_TYPE STREQUAL Debug)
	target_link_libraries(forcepad common ivf2d fltk fltk_gl fltk_forms fltk_images JPEG::JPEG PNG::PNG ZLIB::ZLIB calfemcxx newmat wsock32 comctl32 winmm gdiplus)
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

#target_link_libraries(forcepad common ivf2d fltk fltk_gl fltk_images fltk_forms fltk_jpeg fltk_png calfemcxx newmat "-framework Carbon" "-framework Cocoa" "-framework ApplicationServices" "-framework AGL" "-framework OpenGL" z)
