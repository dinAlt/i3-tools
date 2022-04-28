cmake_minimum_required(VERSION 3.10)

project(i3-tools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(libs/i3ipc++)
include_directories(${I3IPCpp_INCLUDE_DIRS})
link_directories(${I3IPCpp_LIBRARY_DIRS})

file(GLOB_RECURSE SRC src/*.cpp)
add_executable(i3-tools
  ${SRC}
  )

target_link_libraries(i3-tools
  ${I3IPCpp_LIBRARIES}
  )
