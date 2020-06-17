project(remote_control)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_library(remote MODULE "")
include(src/CMakeLists.txt)
target_link_libraries(remote ws2_32 jansson thcrap)
set_target_properties(remote PROPERTIES PREFIX "")
set_target_properties(remote PROPERTIES SUFFIX ".dll")