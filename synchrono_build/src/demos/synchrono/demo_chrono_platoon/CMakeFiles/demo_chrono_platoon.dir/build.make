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
CMAKE_SOURCE_DIR = /home/jason/Desktop/FileTransfer/synchrono-name

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jason/Desktop/FileTransfer/synchrono_build

# Include any dependencies generated for this target.
include src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/depend.make

# Include the progress variables for this target.
include src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/progress.make

# Include the compile flags for this target's objects.
include src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/flags.make

src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.o: src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/flags.make
src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.o: /home/jason/Desktop/FileTransfer/synchrono-name/src/demos/synchrono/demo_chrono_platoon/demo_chrono_platoon.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jason/Desktop/FileTransfer/synchrono_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.o"
	cd /home/jason/Desktop/FileTransfer/synchrono_build/src/demos/synchrono/demo_chrono_platoon && /usr/bin/ccache /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.o -c /home/jason/Desktop/FileTransfer/synchrono-name/src/demos/synchrono/demo_chrono_platoon/demo_chrono_platoon.cpp

src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.i"
	cd /home/jason/Desktop/FileTransfer/synchrono_build/src/demos/synchrono/demo_chrono_platoon && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jason/Desktop/FileTransfer/synchrono-name/src/demos/synchrono/demo_chrono_platoon/demo_chrono_platoon.cpp > CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.i

src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.s"
	cd /home/jason/Desktop/FileTransfer/synchrono_build/src/demos/synchrono/demo_chrono_platoon && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jason/Desktop/FileTransfer/synchrono-name/src/demos/synchrono/demo_chrono_platoon/demo_chrono_platoon.cpp -o CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.s

# Object files for target demo_chrono_platoon
demo_chrono_platoon_OBJECTS = \
"CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.o"

# External object files for target demo_chrono_platoon
demo_chrono_platoon_EXTERNAL_OBJECTS =

bin/demo_chrono_platoon: src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/demo_chrono_platoon.cpp.o
bin/demo_chrono_platoon: src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/build.make
bin/demo_chrono_platoon: /usr/lib/libIrrlicht.so
bin/demo_chrono_platoon: /home/jason/Desktop/NVIDIA-OptiX-SDK-6.0.0-linux64/lib64/liboptix.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libcudart_static.a
bin/demo_chrono_platoon: /usr/lib/librt.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppc.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppial.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppicc.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppicom.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppidei.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppif.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppig.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppim.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppist.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppisu.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppitc.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnpps.so
bin/demo_chrono_platoon: /usr/lib/libglfw.so
bin/demo_chrono_platoon: /usr/lib/libGLEW.so
bin/demo_chrono_platoon: /usr/lib/libGL.so
bin/demo_chrono_platoon: /usr/lib/libGLU.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnvrtc.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoEngine.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoEngine_irrlicht.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoEngine_vehicle.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoModels_vehicle.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoEngine_vehicle_irrlicht.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoEngine_sensor.so
bin/demo_chrono_platoon: lib/libSynchrono.so
bin/demo_chrono_platoon: /usr/lib/openmpi/libmpi_cxx.so
bin/demo_chrono_platoon: /usr/lib/openmpi/libmpi.so
bin/demo_chrono_platoon: /usr/lib/libIrrlicht.so
bin/demo_chrono_platoon: /home/jason/Desktop/NVIDIA-OptiX-SDK-6.0.0-linux64/lib64/liboptix.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libcudart_static.a
bin/demo_chrono_platoon: /usr/lib/librt.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppc.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppial.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppicc.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppicom.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppidei.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppif.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppig.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppim.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppist.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppisu.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnppitc.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnpps.so
bin/demo_chrono_platoon: /usr/lib/libglfw.so
bin/demo_chrono_platoon: /usr/lib/libGLEW.so
bin/demo_chrono_platoon: /usr/lib/libGL.so
bin/demo_chrono_platoon: /usr/lib/libGLU.so
bin/demo_chrono_platoon: /opt/cuda/lib64/libnvrtc.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoEngine.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoEngine_irrlicht.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoEngine_vehicle.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoModels_vehicle.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoEngine_vehicle_irrlicht.so
bin/demo_chrono_platoon: /home/jason/Desktop/FileTransfer/chrono_build_SCM/lib/libChronoEngine_sensor.so
bin/demo_chrono_platoon: /usr/lib/openmpi/libmpi_cxx.so
bin/demo_chrono_platoon: /usr/lib/openmpi/libmpi.so
bin/demo_chrono_platoon: src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jason/Desktop/FileTransfer/synchrono_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../../bin/demo_chrono_platoon"
	cd /home/jason/Desktop/FileTransfer/synchrono_build/src/demos/synchrono/demo_chrono_platoon && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/demo_chrono_platoon.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/build: bin/demo_chrono_platoon

.PHONY : src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/build

src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/clean:
	cd /home/jason/Desktop/FileTransfer/synchrono_build/src/demos/synchrono/demo_chrono_platoon && $(CMAKE_COMMAND) -P CMakeFiles/demo_chrono_platoon.dir/cmake_clean.cmake
.PHONY : src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/clean

src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/depend:
	cd /home/jason/Desktop/FileTransfer/synchrono_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jason/Desktop/FileTransfer/synchrono-name /home/jason/Desktop/FileTransfer/synchrono-name/src/demos/synchrono/demo_chrono_platoon /home/jason/Desktop/FileTransfer/synchrono_build /home/jason/Desktop/FileTransfer/synchrono_build/src/demos/synchrono/demo_chrono_platoon /home/jason/Desktop/FileTransfer/synchrono_build/src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/demos/synchrono/demo_chrono_platoon/CMakeFiles/demo_chrono_platoon.dir/depend

