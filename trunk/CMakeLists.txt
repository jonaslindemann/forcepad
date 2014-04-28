CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(forcepad_project)

if(WIN32)
    set(JPEG_LIBRARY "d:/Libraries/depends/jpeg/lib/libjpeg.lib")
    set(JPEG_INCLUDE_DIR "d:/Libraries/depends/jpeg/include")
    set(ZLIB_LIBRARY "d:/Libraries/depends/zlib/lib/zlib.lib")
    set(ZLIB_INCLUDE_DIR "d:/Libraries/depends/zlib/include")
    set(PNG_LIBRARY "d:/Libraries/depends/png/lib/libpng15.lib")
    set(PNG_PNG_INCLUDE_DIR "d:/Libraries/depends/png/include")
endif(WIN32)

#find_package(Qt4 COMPONENTS QtMain QtCore QtGui QtSql QtXml QtNetwork QtOpenGL REQUIRED)

#INCLUDE(${QT_USE_FILE})

find_package(OpenGL)
find_package(JPEG)
find_package(PNG)
find_package(ZLIB)

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
        INCLUDE_DIRECTORIES(${FLTK_ROOT} ${FLTK_ROOT}/jpeg ${FLTK_ROOT}/png ${FLTK_ROOT}/zlib ${GLEW_ROOT}/include src/calfemcxx src/newmat src/defs ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
ELSE(WIN32)
	INCLUDE_DIRECTORIES(/opt/local/include ${FLTK_ROOT}/include ${FLTK_ROOT}/jpeg ${FLTK_ROOT}/png ${FLTK_ROOT}/zlib ${GLEW_ROOT}/include src/calfemcxx src/newmat src/defs)
ENDIF(WIN32)

if (APPLE)
LINK_DIRECTORIES(/opt/local/lib ${FLTK_ROOT}/lib ${PROJECT_SOURCE_DIR}/lib )
else(APPLE)
LINK_DIRECTORIES(${FLTK_ROOT}/lib ${PROJECT_SOURCE_DIR}/lib ) 
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
