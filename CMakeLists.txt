CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(forcepad_project)

if(WIN32)
	set(FLTK_DIR "d:/Libraries/fltk-1.4")
	IF(CMAKE_BUILD_TYPE STREQUAL Debug)
		set(FLTK_BUILD_DIR "d:/Libraries/fltk-1.4/build_debug")
		set(JPEG_LIBRARY "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltk_jpegd.lib")
		set(JPEG_INCLUDE_DIR "d:/Libraries/fltk-1.4/jpeg")
		set(ZLIB_LIBRARY "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltk_zd.lib")
		set(ZLIB_INCLUDE_DIR "d:/Libraries/fltk-1.4/zlib")
		set(PNG_LIBRARY "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltk_pngd.lib")
		set(PNG_INCLUDE_DIR "d:/Libraries/fltk-1.4/png")
		set(FLTK_INCLUDE_DIR "d:/Libraries/fltk-1.4")
		set(FLTK_LIBRARY "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltkd.lib")
		set(FLTK_LIBRARIES "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltkd.lib" "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltk_gld.lib" "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltk_formsd.lib" "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltk_imagesd.lib")
		set(CMAKE_PREFIX_PATH "e:/Qt/5.4/msvc2013_opengl")
	ELSE()
		set(FLTK_BUILD_DIR "d:/Libraries/fltk-1.4/build_release")
		set(JPEG_LIBRARY "d:/Libraries/fltk-1.4/build_release/lib/Release/fltk_jpeg.lib")
		set(JPEG_INCLUDE_DIR "d:/Libraries/fltk-1.4/jpeg")
		set(ZLIB_LIBRARY "d:/Libraries/fltk-1.4/build_release/lib/Release/fltk_z.lib")
		set(ZLIB_INCLUDE_DIR "d:/Libraries/fltk-1.4/zlib")
		set(PNG_LIBRARY "d:/Libraries/fltk-1.4/build_release/lib/Release/fltk_png.lib")
		set(PNG_INCLUDE_DIR "d:/Libraries/fltk-1.4/png")
		set(FLTK_INCLUDE_DIR "d:/Libraries/fltk-1.4")
		set(FLTK_LIBRARY "d:/Libraries/fltk-1.4/build_release/lib/Release/fltk.lib")
		set(FLTK_LIBRARIES "d:/Libraries/fltk-1.4/build_release/lib/Release/fltk.lib" "d:/Libraries/fltk-1.4/build_release/lib/Release/fltk_gl.lib" "d:/Libraries/fltk-1.4/build_release/lib/Release/fltk_forms.lib" "d:/Libraries/fltk-1.4/build_release/lib/Release/fltk_images.lib")
		set(CMAKE_PREFIX_PATH "d:/Qt/5.4/msvc2013_opengl")
	ENDIF()
endif(WIN32)

#find_package( Qt5Core REQUIRED )
#find_package( Qt5Gui REQUIRED )
#find_package( Qt5Widgets REQUIRED )
#find_package( Qt5OpenGL REQUIRED )
#find_package( Qt5Svg REQUIRED )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)

#find_package(Qt4 COMPONENTS QtMain QtCore QtGui QtSql QtXml QtNetwork QtOpenGL REQUIRED)

#INCLUDE(${QT_USE_FILE})

#find_package(FLTK)
find_package(OpenGL)
#find_package(JPEG)
#find_package(PNG)
#find_package(ZLIB)

message(${PROJECT_BINARY_DIR})
message(${FLTK_LIBRARY})
message(${JPEG_LIBRARY})

IF(APPLE)
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch i386")
#SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -arch i386")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -arch i386")
ENDIF(APPLE)

if (WIN32)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /GS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GS")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
endif(WIN32)

ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)


IF(WIN32)
	INCLUDE_DIRECTORIES(${FLTK_BUILD_DIR} ${FLTK_INCLUDE_DIR} src/calfemcxx src/newmat src/defs ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
ELSE(WIN32)
	INCLUDE_DIRECTORIES(/opt/local/include ${FLTK_INCLUDE_DIR} src/calfemcxx src/newmat src/defs)
ENDIF(WIN32)

if (APPLE)
LINK_DIRECTORIES(/opt/local/lib ${PROJECT_SOURCE_DIR}/lib )
else(APPLE)
	if(WIN32)
		LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib ) 
	else()
		LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib ) 
	endif()
endif(APPLE)

#LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../fltk-1.1.8/build_xcode/bin ${PROJECT_SOURCE_DIR}/../calfemcxx-1.0/lib ${PROJECT_SOURCE_DIR}/../newmat-10.A/lib)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

IF(WIN32)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
ELSE(WIN32)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/release)
ENDIF(WIN32)

message("Build type in main = " ${CMAKE_BUILD_TYPE})

if (CMAKE_BUILD_TYPE STREQUAL Release)
	message("Building release version.")
else()
	message("Building debug version.")
endif()

SUBDIRS(src)
