INCLUDE_DIRECTORIES(../common ../ivf2d)
FILE(GLOB SOURCE_FILES *.cpp)
FILE(GLOB INCLUDE_FILES *.h)

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
	ADD_EXECUTABLE(forcepadd ${INCLUDE_FILES} ${SOURCE_FILES})
ELSE(CMAKE_BUILD_TYPE STREQUAL Debug)
IF(APPLE)
#ADD_EXECUTABLE(forcepad ${INCLUDE_FILES} ${SOURCE_FILES})
ADD_EXECUTABLE(
	ForcePAD 
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
)

SET( MACOSX_BUNDLE_ICON_FILE forcepad_icon.icns )
SET( MACOSX_BUNDLE_SHORT_VERSION_STRING 2.4 )
SET( MACOSX_BUNDLE_VERSION 2.4 )
SET( MACOSX_BUNDLE_LONG_VERSION_STRING 2.4 )

SET_SOURCE_FILES_PROPERTIES(
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
SET_SOURCE_FILES_PROPERTIES(
        ../../bin/release/colormaps/colormap1.map
        ../../bin/release/colormaps/colormap2.map
	PROPERTIES
	MACOSX_PACKAGE_LOCATION MacOS/colormaps
)
SET_SOURCE_FILES_PROPERTIES(
        ../../images/forcepad_icon.icns
	PROPERTIES
	MACOSX_PACKAGE_LOCATION Resources
)
ELSE(APPLE)
	ADD_EXECUTABLE(forcepad ${INCLUDE_FILES} ${SOURCE_FILES})
ENDIF(APPLE)
ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)



IF(WIN32)
	IF(CMAKE_BUILD_TYPE STREQUAL Debug)
		TARGET_LINK_LIBRARIES(forcepadd commond ivf2dd fltkd fltkgld fltkimagesd fltkformsd fltkjpegd fltkpngd calfemcxxd newmatd fltkzd wsock32 comctl32 winmm.lib glew32 )
	ELSE(CMAKE_BUILD_TYPE STREQUAL Debug)
		TARGET_LINK_LIBRARIES(forcepad common ivf2d fltk fltkgl fltkimages fltkforms fltkjpeg fltkpng calfemcxx newmat fltkz wsock32 comctl32 winmm.lib glew32)
	ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)
ELSE(WIN32)
	IF(APPLE)
		#TARGET_LINK_LIBRARIES(forcepad common ivf2d fltk fltk_gl fltk_images fltk_forms fltk_jpeg fltk_png calfemcxx newmat fltk_zlib GLU GL)
		TARGET_LINK_LIBRARIES(ForcePAD common ivf2d fltk fltk_gl fltk_images fltk_forms fltk_jpeg fltk_png calfemcxx newmat "-framework Carbon" "-framework Cocoa" "-framework ApplicationServices" "-framework AGL" "-framework OpenGL" GLEW z)
	ELSE(APPLE)
		TARGET_LINK_LIBRARIES(forcepad common ivf2d fltk fltk_gl fltk_images fltk_forms jpeg png calfemcxx newmat z GLU GL)
	ENDIF(APPLE)
ENDIF(WIN32)

#TARGET_LINK_LIBRARIES(forcepad common ivf2d fltk fltk_gl fltk_images fltk_forms fltk_jpeg fltk_png calfemcxx newmat "-framework Carbon" "-framework Cocoa" "-framework ApplicationServices" "-framework AGL" "-framework OpenGL" z)

