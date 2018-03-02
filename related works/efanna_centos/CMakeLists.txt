cmake_minimum_required(VERSION 3.6)
project(efanna)

set(CMAKE_CXX_STANDARD 11)
set(OPTM "-O3 -march=native -fopenmp")
set(CPFLAGS ${OPTM} "-DINFO")
set(LDFLAGS ${OPTM} "-lboost_timer -lboost_chrono -lboost_system -L /usr/lib64/mysql -I /usr/include/mysql -DINFO")

set(CMAKE_CXX_FLAGS ${LDFLAGS} "-lmysqlclient" )

set(BOOST_ROOT "C:/local/boost_1_66_0")
find_package(Boost)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
endif()

file(GLOB SOURCES "samples/*.cc mysql/MyDB.cpp")
file(GLOB HEADERS "*.hpp */*.hpp mysql/MyDB.h")
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/* ${PROJECT_SOURCE_DIR}/*/*)

set(SOURCE_FILES ${SOURCES} ${HEADERS})
add_executable(efanna ${HEADERS} samples/efanna_index_buildall.cc)

target_link_libraries(efanna ${LDFLAGS} mysqlclient)
#add_executable(efanna efanna_index_buildgraph.cc)
#add_executable(efanna efanna_index_buildtrees.cc)