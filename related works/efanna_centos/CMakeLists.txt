cmake_minimum_required(VERSION 3.6)
project(efanna)

set(CMAKE_CXX_STANDARD 11)
set(OPTM "-O3 -march=native -fopenmp")
set(CPFLAGS ${OPTM} "")
set(LDFLAGS ${OPTM} "-L /usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lssl -lcrypto -ldl -lboost_timer -lboost_chrono -lboost_system")

set(CMAKE_CXX_FLAGS  ${CPFLAGS})

#mysql
include_directories("C:/Program Files/MySQL/mysql-connector-c-6.1.11-win32/include")

set(BOOST_ROOT "C:/local/boost_1_66_0/boost_1_66_0")
#find_package(Boost)
#IF (Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIR})
#    ADD_DEFINITIONS( "-DHAS_BOOST" )
#endif()

file(GLOB SOURCES "*/*.cc")
file(GLOB HEADERS "*.hpp */*.hpp")
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/* ${PROJECT_SOURCE_DIR}/*/*)

set(SOURCE_FILES ${SOURCES} ${HEADERS})
add_executable(efanna samples/efanna_index_buildall.cc)

target_link_libraries(efanna ${LDFLAGS})
#add_executable(efanna efanna_index_buildgraph.cc)
#add_executable(efanna efanna_index_buildtrees.cc)
