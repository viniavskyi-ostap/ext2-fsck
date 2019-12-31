cmake_minimum_required(VERSION 3.14)
project(ext2-fsck)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc)
add_library(biofs src/biofs.cpp)
add_library(filesystem src/Filesystem.cpp src/BlockGroup.cpp src/INode.cpp)
target_link_libraries(filesystem biofs)

add_executable(ext2-fsck main.cpp)
target_link_libraries(ext2-fsck filesystem)

add_executable(ext2-fsbreak break.cpp)
target_link_libraries(ext2-fsbreak filesystem biofs)

add_executable(life_is_awesome life_is_awesome.cpp)
target_link_libraries(life_is_awesome biofs)
