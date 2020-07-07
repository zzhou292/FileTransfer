#--------------------------------------------------------------
# Always use full RPATH (differentiating between the build and install trees)

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")

#--------------------------------------------------------------

# Enable creation of "application bundles" on MacOSX.
# This is necessary for Irrlicht-based demos and optional for OpenGL or non-graphics demos.
# For simplicity and consistency, we enable here MACOSX_BUNDLE for all subsequent programs.

if(APPLE)
    set(CMAKE_MACOSX_BUNDLE ON)
endif()

#--------------------------------------------------------------

message(STATUS)
message(STATUS "==== Demo programs ====")
message(STATUS)

#  --- Core demos ---
add_subdirectory(synchrono)

# --- Module demos ---
# if(ENABLE_MODULE_INTERFACE)
    # add_subdirectory(interface)
# endif()
if(ENABLE_MODULE_MATLAB)
    add_subdirectory(matlab)
endif()
