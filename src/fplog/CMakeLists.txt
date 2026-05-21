find_package(spdlog CONFIG REQUIRED)

add_library(fplog FPLog.cpp FPLog.h)
target_link_libraries(fplog PUBLIC spdlog::spdlog)
target_include_directories(fplog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(fplog PROPERTIES DEBUG_POSTFIX d)
