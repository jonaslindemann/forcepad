cmake_minimum_required(VERSION 3.20)

project(forcepad_project)



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

#include(${QT_USE_FILE})

find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(FLTK CONFIG REQUIRED)



message("Project dir" ${PROJECT_BINARY_DIR})
message("FLTK Library" ${FLTK_LIBRARY})
message("JPEH Library" ${JPEG_LIBRARY})

add_definitions(-D_CRT_SECURE_NO_WARNINGS)


if(WIN32)
	include_directories(${FLTK_BUILD_DIR} ${FLTK_INCLUDE_DIR} src/calfemcxx src/newmat src/defs ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
else(WIN32)
	include_directories(/opt/local/include ${FLTK_INCLUDE_DIR} src/calfemcxx src/newmat src/defs)
endif(WIN32)

if (APPLE)
	link_directories(/opt/local/lib ${PROJECT_SOURCE_DIR}/lib )
else(APPLE)
	if(WIN32)
		link_directories(${PROJECT_SOURCE_DIR}/lib ) 
	else()
		link_directories(${PROJECT_SOURCE_DIR}/lib ) 
	endif()
endif(APPLE)

#link_directories(${PROJECT_SOURCE_DIR}/../fltk-1.1.8/build_xcode/bin ${PROJECT_SOURCE_DIR}/../calfemcxx-1.0/lib ${PROJECT_SOURCE_DIR}/../newmat-10.A/lib)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

if(WIN32)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
else(WIN32)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/release)
endif(WIN32)

message("Build type in main = " ${CMAKE_BUILD_TYPE})

if (CMAKE_BUILD_TYPE STREQUAL Release)
	message("Building release version.")
else()
	message("Building debug version.")
endif()

subdirs(src)
