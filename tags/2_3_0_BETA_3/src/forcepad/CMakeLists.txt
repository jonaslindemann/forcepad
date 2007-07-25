INCLUDE_DIRECTORIES(../common ../ivf2d)
FILE(GLOB SOURCE_FILES *.cpp)
FILE(GLOB INCLUDE_FILES *.h)

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
	ADD_EXECUTABLE(forcepadd ${INCLUDE_FILES} ${SOURCE_FILES})
ELSE(CMAKE_BUILD_TYPE STREQUAL Debug)
	ADD_EXECUTABLE(forcepad ${INCLUDE_FILES} ${SOURCE_FILES})
ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)

# ADD_EXECUTABLE(
	# forcepad 
	# MACOSX_BUNDLE
	# ${SOURCE_FILES}
	# brushes/rbrush16.rgb
	# brushes/rbrush32.rgb
	# brushes/rbrush4.rgb
	# brushes/rbrush64.rgb
	# brushes/rbrush8.rgb
	# brushes/sbrush16.rgb
	# brushes/sbrush32.rgb
	# brushes/sbrush4.rgb
	# brushes/sbrush64.rgb
	# brushes/sbrush8.rgb
# )

# SET_SOURCE_FILES_PROPERTIES(
        # brushes/rbrush16.rgb
        # brushes/rbrush32.rgb
        # brushes/rbrush4.rgb
        # brushes/rbrush64.rgb
        # brushes/rbrush8.rgb
        # brushes/sbrush16.rgb
        # brushes/sbrush32.rgb
        # brushes/sbrush4.rgb
        # brushes/sbrush64.rgb
        # brushes/sbrush8.rgb
	# PROPERTIES
	# MACOSX_PACKAGE_LOCATION MacOS/brushes
# )

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
	TARGET_LINK_LIBRARIES(forcepadd commond ivf2dd fltkd fltkgld fltkimagesd fltkformsd fltkjpegd fltkpngd calfemcxxd newmatd fltkzd wsock32 comctl32 )
ELSE(CMAKE_BUILD_TYPE STREQUAL Debug)
	TARGET_LINK_LIBRARIES(forcepad common ivf2d fltk fltkgl fltkimages fltkforms fltkjpeg fltkpng calfemcxx newmat fltkz wsock32 comctl32 )
ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)

#TARGET_LINK_LIBRARIES(forcepad common ivf2d fltk fltk_gl fltk_images fltk_forms fltk_jpeg fltk_png calfemcxx newmat "-framework Carbon" "-framework Cocoa" "-framework ApplicationServices" "-framework AGL" "-framework OpenGL" z)

