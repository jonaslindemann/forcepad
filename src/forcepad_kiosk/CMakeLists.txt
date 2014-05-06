INCLUDE_DIRECTORIES(../common ../ivf2d ../forcepad)
FILE(GLOB SOURCE_FILES ../forcepad/*.cpp)
FILE(GLOB INCLUDE_FILES ../forcepad/*.h)

FILE(GLOB to_remove_cpp "../forcepad/MainFrame2.cpp")
FILE(GLOB to_remove_h "../forcepad/MainFrame2.h")
LIST(REMOVE_ITEM SOURCE_FILES ${to_remove_cpp})
LIST(REMOVE_ITEM INCLUDE_FILES ${to_remove_h})

FILE(GLOB to_remove_cpp "../forcepad/TabletToolbar.cpp")
FILE(GLOB to_remove_h "../forcepad/TabletToolbar.h")
LIST(REMOVE_ITEM SOURCE_FILES ${to_remove_cpp})
LIST(REMOVE_ITEM INCLUDE_FILES ${to_remove_h})

ADD_DEFINITIONS(-DFORCEPAD_KIOSK)

#IF(CMAKE_BUILD_TYPE STREQUAL Debug)
#	ADD_EXECUTABLE(forcepadd ${INCLUDE_FILES} ${SOURCE_FILES})
#ELSE(CMAKE_BUILD_TYPE STREQUAL Debug)

IF(APPLE)
#ADD_EXECUTABLE(forcepad ${INCLUDE_FILES} ${SOURCE_FILES})
ADD_EXECUTABLE(
	ForcePAD_Kiosk 
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
	../../bin/release/kiosk/ex1.fp2
	../../bin/release/kiosk/ex2.fp2
	../../bin/release/kiosk/ex3.fp2
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
	if (CMAKE_BUILD_TYPE STREQUAL Release)
		ADD_EXECUTABLE(forcepad_kiosk ${INCLUDE_FILES} ${SOURCE_FILES})
	else()
		ADD_EXECUTABLE(forcepad_kioskd ${INCLUDE_FILES} ${SOURCE_FILES})
	endif()
ENDIF(APPLE)

IF(WIN32)
	IF(CMAKE_BUILD_TYPE STREQUAL Debug)
		TARGET_LINK_LIBRARIES(forcepad_kioskd commond ivf2dd ${FLTK_LIBRARIES} ${JPEG_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY} calfemcxxd newmatd wsock32 comctl32 winmm.lib )
	ELSE(CMAKE_BUILD_TYPE STREQUAL Debug)
		TARGET_LINK_LIBRARIES(forcepad_kiosk common ivf2d ${FLTK_LIBRARIES} ${JPEG_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY} calfemcxx newmat wsock32 comctl32 winmm.lib)
	ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)
ELSE(WIN32)
	IF(APPLE)
                if (CMAKE_BUILD_TYPE STREQUAL Release)
                    TARGET_LINK_LIBRARIES(ForcePAD_Kiosk common ivf2d fltk fltk_gl fltk_images fltk_forms fltk fltk calfemcxx newmat
jpeg png "-framework Cocoa" "-framework ApplicationServices" "-framework AGL" "-framework OpenGL" z)
                else()
                    TARGET_LINK_LIBRARIES(ForcePAD_Kiosk commond ivf2dd fltk fltk_gl fltk_images fltk_forms fltk fltk calfemcxxd newmatd
jpeg png "-framework Cocoa" "-framework ApplicationServices" "-framework AGL" "-framework OpenGL" z)
                endif()
        ELSE(APPLE)
		TARGET_LINK_LIBRARIES(forcepad_kiosk common ivf2d fltk fltk_gl fltk_images fltk_forms jpeg png calfemcxx newmat z GLU GL)
	ENDIF(APPLE)
ENDIF(WIN32)

#TARGET_LINK_LIBRARIES(forcepad common ivf2d fltk fltk_gl fltk_images fltk_forms fltk_jpeg fltk_png calfemcxx newmat "-framework Carbon" "-framework Cocoa" "-framework ApplicationServices" "-framework AGL" "-framework OpenGL" z)

