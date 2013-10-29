INCLUDE_DIRECTORIES(../ivf2d)
FILE(GLOB SOURCE_FILES1 *.cpp)
FILE(GLOB SOURCE_FILES2 *.cxx)
FILE(GLOB INCLUDE_FILES1 *.h)
FILE(GLOB INCLUDE_FILES2 *.H)

message("Build type in common = " ${CMAKE_BUILD_TYPE})

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	message("Building debug version of libcommond")
	ADD_LIBRARY(commond ${INCLUDE_FILES1} ${INCLUDE_FILES2} ${SOURCE_FILES1} ${SOURCE_FILES2})
else()
	message("Buiding release version of libcommond")
	ADD_LIBRARY(common ${INCLUDE_FILES1} ${INCLUDE_FILES2} ${SOURCE_FILES1} ${SOURCE_FILES2})
endif()


