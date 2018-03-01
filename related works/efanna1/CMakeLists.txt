cmake_minimum_required(VERSION 3.8)
project(efanna)

set(CMAKE_CXX_COMPILER "/usr/local/Cellar/gcc/7.3.0/bin/g++-7")
set(CMAKE_CXX_STANDARD 11)
set(OPTM "-O3 -march=native -fopenmp")
set(CPFLAGS ${OPTM} "-Wall -DINFO")
set(LDFLAGS ${OPTM} "-Wall -lboost_timer -lboost_chrono -lboost_system -DINFO")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -fopenmp")
set(CMAKE_CXX_FLAGS "-Wall -DINFO")

set(BOOST_ROOT "C:/local/boost_1_66_0")
find_package(Boost)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
endif()

file(GLOB SOURCES "samples/*.cc")
file(GLOB HEADERS "*.hpp */*.hpp")
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/* ${PROJECT_SOURCE_DIR}/*/*)

set(SOURCE_FILES ${SOURCES} ${HEADERS})
add_executable(efanna samples/efanna_index_buildall.cc)

<<<<<<< HEAD
#target_link_libraries(efanna ${LDFLAGS})
=======

target_link_libraries(efanna ${LDFLAGS})
>>>>>>> e3810648ef00047bde1bae47f283ab056a85b4c2
#add_executable(efanna efanna_index_buildgraph.cc)
#add_executable(efanna efanna_index_buildtrees.cc)