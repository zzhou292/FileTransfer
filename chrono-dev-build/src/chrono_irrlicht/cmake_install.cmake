# Install script for directory: /home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_irrlicht.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_irrlicht.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_irrlicht.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/jason/Desktop/FileTransfer/chrono-dev-build/lib/libChronoEngine_irrlicht.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_irrlicht.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_irrlicht.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_irrlicht.so"
         OLD_RPATH "/home/jason/Desktop/FileTransfer/chrono-dev-build/lib:/usr/local/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_irrlicht.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/chrono_irrlicht" TYPE FILE FILES
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChApiIrr.h"
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChIrrTools.h"
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChIrrAppInterface.h"
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChIrrApp.h"
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChIrrCamera.h"
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChIrrMeshTools.h"
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChIrrWizard.h"
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChIrrAssetConverter.h"
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChIrrNode.h"
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChIrrNodeProxyToAsset.h"
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChIrrNodeAsset.h"
    "/home/jason/Desktop/FileTransfer/chrono-dev-develop/src/chrono_irrlicht/ChIrrEffects.h"
    )
endif()

