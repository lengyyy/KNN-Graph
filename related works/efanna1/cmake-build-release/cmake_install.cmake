<<<<<<< HEAD
# Install script for directory: C:/Users/markz/Desktop/KNN-Graph/KNN-Graph/related works/efanna1
=======
# Install script for directory: /Users/lengyue/Desktop/knn g 2/KNN-Graph/related works/efanna1
>>>>>>> e3810648ef00047bde1bae47f283ab056a85b4c2

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
<<<<<<< HEAD
file(WRITE "C:/Users/markz/Desktop/KNN-Graph/KNN-Graph/related works/efanna1/cmake-build-release/${CMAKE_INSTALL_MANIFEST}"
=======
file(WRITE "/Users/lengyue/Desktop/knn g 2/KNN-Graph/related works/efanna1/cmake-build-release/${CMAKE_INSTALL_MANIFEST}"
>>>>>>> e3810648ef00047bde1bae47f283ab056a85b4c2
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
