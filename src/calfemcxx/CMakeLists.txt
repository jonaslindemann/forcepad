include_directories(../newmat)
file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_library(calfemcxxd ${INCLUDE_FILES} ${SOURCE_FILES})
else(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_library(calfemcxx ${INCLUDE_FILES} ${SOURCE_FILES})
endif(CMAKE_BUILD_TYPE STREQUAL Debug)
