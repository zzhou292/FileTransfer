# Install script for directory: /home/jason/Desktop/FileTransfer/chrono-dev-feature-FSI-release/src/demos/vehicle

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_HMMWV9/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_HMMWV/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_HMMWV_Parallel/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_WheeledGeneric/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_WheeledJSON/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_SuspensionTest/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_TireTest/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_TireTest_Parallel/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_TractorTrailer/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_ArticulatedVehicle/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_SteeringController/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_TwoCars/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_Sedan/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_CityBus/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_ISO2631/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_UAZ/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_DeformableSoil/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_DeformableSoilAndTire/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_GranularTerrain/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_M113/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_M113_Band/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_M113_Parallel/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_TrackedJSON/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_TrackedJSON_Band/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_TrackTestRig/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_TrackTestRig_Band/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_Paths/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_RigidTerrain/cmake_install.cmake")
  include("/home/jason/Desktop/FileTransfer/chrono_fsi_release/src/demos/vehicle/demo_CRGTerrain/cmake_install.cmake")

endif()

