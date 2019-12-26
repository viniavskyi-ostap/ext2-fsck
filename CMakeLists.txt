cmake_minimum_required(VERSION 3.14)
project(ext2-fsck)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc)
add_library(filesystem src/Filesystem.cpp src/BlockGroup.cpp src/INode.cpp)

add_executable(ext2-fsck main.cpp)
target_link_libraries(ext2-fsck filesystem)

add_executable(ext2-fsbreak break.cpp)
target_link_libraries(ext2-fsbreak filesystem)
