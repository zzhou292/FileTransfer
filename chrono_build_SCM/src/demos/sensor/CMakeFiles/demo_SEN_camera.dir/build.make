# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jason/Desktop/FileTransfer/chrono_build_SCM

# Include any dependencies generated for this target.
include src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/depend.make

# Include the progress variables for this target.
include src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/progress.make

# Include the compile flags for this target's objects.
include src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/flags.make

src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.o: src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/flags.make
src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.o: /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src/demos/sensor/demo_SEN_camera.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jason/Desktop/FileTransfer/chrono_build_SCM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.o"
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/sensor && /usr/bin/ccache /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.o -c /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src/demos/sensor/demo_SEN_camera.cpp

src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.i"
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/sensor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src/demos/sensor/demo_SEN_camera.cpp > CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.i

src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.s"
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/sensor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src/demos/sensor/demo_SEN_camera.cpp -o CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.s

# Object files for target demo_SEN_camera
demo_SEN_camera_OBJECTS = \
"CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.o"

# External object files for target demo_SEN_camera
demo_SEN_camera_EXTERNAL_OBJECTS =

bin/demo_SEN_camera: src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/demo_SEN_camera.cpp.o
bin/demo_SEN_camera: src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/build.make
bin/demo_SEN_camera: lib/libChronoEngine_sensor.so
bin/demo_SEN_camera: lib/libChronoEngine_vehicle_irrlicht.so
bin/demo_SEN_camera: lib/libChronoModels_vehicle.so
bin/demo_SEN_camera: /home/jason/Desktop/NVIDIA-OptiX-SDK-6.0.0-linux64/lib64/liboptix.so
bin/demo_SEN_camera: /opt/cuda/lib64/libcudart_static.a
bin/demo_SEN_camera: /usr/lib/librt.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnppc.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnppial.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnppicc.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnppicom.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnppidei.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnppif.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnppig.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnppim.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnppist.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnppisu.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnppitc.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnpps.so
bin/demo_SEN_camera: /usr/lib/libglfw.so
bin/demo_SEN_camera: /usr/lib/libGLEW.so
bin/demo_SEN_camera: /usr/lib/libGL.so
bin/demo_SEN_camera: /usr/lib/libGLU.so
bin/demo_SEN_camera: /opt/cuda/lib64/libnvrtc.so
bin/demo_SEN_camera: lib/libChronoEngine_irrlicht.so
bin/demo_SEN_camera: /usr/lib/libIrrlicht.so
bin/demo_SEN_camera: lib/libChronoEngine_vehicle.so
bin/demo_SEN_camera: lib/libChronoEngine.so
bin/demo_SEN_camera: /usr/lib/openmpi/libmpi_cxx.so
bin/demo_SEN_camera: /usr/lib/openmpi/libmpi.so
bin/demo_SEN_camera: src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jason/Desktop/FileTransfer/chrono_build_SCM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../bin/demo_SEN_camera"
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/sensor && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/demo_SEN_camera.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/build: bin/demo_SEN_camera

.PHONY : src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/build

src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/clean:
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/sensor && $(CMAKE_COMMAND) -P CMakeFiles/demo_SEN_camera.dir/cmake_clean.cmake
.PHONY : src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/clean

src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/depend:
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63 /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src/demos/sensor /home/jason/Desktop/FileTransfer/chrono_build_SCM /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/sensor /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/demos/sensor/CMakeFiles/demo_SEN_camera.dir/depend

