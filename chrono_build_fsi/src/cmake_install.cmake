# Install script for directory: /home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src

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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_thirdparty" FILES_MATCHING REGEX "/[^/]*\\.h$" REGEX "/[^/]*\\.cuh$" REGEX "/[^/]*\\.hpp$")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/chrono" TYPE FILE FILES "/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/chrono/ChConfig.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/chrono" TYPE FILE FILES "/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/chrono/ChVersion.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE RENAME "ChronoConfig.cmake" FILES "/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/cmake/ChronoConfig.cmake.install")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_mkl/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_mumps/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_matlab/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_irrlicht/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_cascade/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_postprocess/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_cosimulation/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_python/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_parallel/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_granular/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_opengl/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_distributed/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_vehicle/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_fsi/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/chrono_models/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/demos/cmake_install.cmake")

endif()

