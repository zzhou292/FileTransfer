# Install script for directory: /home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/chrono_fsi" TYPE FILE FILES "/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/chrono_fsi/ChConfigFSI.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_fsi.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_fsi.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_fsi.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/lib/libChronoEngine_fsi.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_fsi.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_fsi.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_fsi.so"
         OLD_RPATH "/opt/cuda/lib64:/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/lib:/usr/lib/openmpi:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libChronoEngine_fsi.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/chrono_fsi" TYPE FILE FILES
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/ChApiFsi.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/ChSystemFsi.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/ChFsiInterface.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/ChFsiDataManager.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/utils/ChUtilsDevice.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/utils/ChUtilsTypeConvert.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/physics/ChBce.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/physics/ChFluidDynamics.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/physics/ChCollisionSystemFsi.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/physics/ChFsiForce.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/physics/ChFsiForceExplicitSPH.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/physics/ChFsiForceI2SPH.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/physics/ChFsiForceIISPH.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/physics/ChFsiGeneral.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/physics/ChSphGeneral.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/physics/ChParams.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/math/custom_math.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/math/ExactLinearSolvers.cuh"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/math/ChFsiLinearSolver.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/math/ChFsiLinearSolverBiCGStab.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/math/ChFsiLinearSolverGMRES.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/chrono_fsi/utils" TYPE FILE FILES
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/utils/ChUtilsJSON.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/utils/ChUtilsGeneratorBce.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/utils/ChUtilsGeneratorFluid.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/utils/ChUtilsGeneratorFsi.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/utils/ChUtilsPrintStruct.h"
    "/home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/chrono_fsi/utils/ChUtilsPrintSph.cuh"
    )
endif()

