cmake_minimum_required(VERSION 3.10)
SET(CMAKE_SYSTEM_NAME Windows CACHE INTERNAL "")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -m32")

SET(CMAKE_C_COMPILER /usr/bin/i686-w64-mingw32-gcc CACHE INTERNAL "")
SET(CMAKE_RC_COMPILER /usr/bin/i686-w64-mingw32-windres CACHE INTERNAL "")
set(CMAKE_RANLIB /usr/bin/i686-w64-mingw32-ranlib CACHE INTERNAL "")

add_subdirectory(remote_control)
add_subdirectory(thcrap_repo)