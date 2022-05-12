cmake_minimum_required(VERSION 3.2.2)
project(notification_feed)

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src/client)
add_subdirectory(src/server)
add_subdirectory(src/front_end)
