find_package(Eigen3 REQUIRED)

file(GLOB SOURCE_FILES CONFIGURE_DEPENDS *.cpp)
file(GLOB INCLUDE_FILES CONFIGURE_DEPENDS *.h)

add_library(calfem ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(calfem PROPERTIES DEBUG_POSTFIX d)
target_include_directories(calfem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(calfem PUBLIC Eigen3::Eigen fplog)
if(OpenMP_FOUND)
    target_link_libraries(calfem PUBLIC OpenMP::OpenMP_CXX)
endif()
