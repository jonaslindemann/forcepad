file(GLOB SOURCE_FILES CONFIGURE_DEPENDS *.cpp)
file(GLOB INCLUDE_FILES CONFIGURE_DEPENDS *.h)

add_library(newmat ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(newmat PROPERTIES DEBUG_POSTFIX d)
target_include_directories(newmat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

