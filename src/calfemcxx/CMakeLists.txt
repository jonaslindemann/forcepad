include_directories(../newmat)
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS *.cpp)
file(GLOB INCLUDE_FILES CONFIGURE_DEPENDS *.h)

add_library(calfemcxx ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(calfemcxx PROPERTIES DEBUG_POSTFIX d)
target_include_directories(calfemcxx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(calfemcxx PUBLIC newmat)
