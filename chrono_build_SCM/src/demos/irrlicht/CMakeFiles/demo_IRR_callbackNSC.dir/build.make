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
include src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/depend.make

# Include the progress variables for this target.
include src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/progress.make

# Include the compile flags for this target's objects.
include src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/flags.make

src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.o: src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/flags.make
src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.o: /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src/demos/irrlicht/demo_IRR_callbackNSC.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jason/Desktop/FileTransfer/chrono_build_SCM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.o"
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/irrlicht && /usr/bin/ccache /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.o -c /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src/demos/irrlicht/demo_IRR_callbackNSC.cpp

src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.i"
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/irrlicht && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src/demos/irrlicht/demo_IRR_callbackNSC.cpp > CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.i

src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.s"
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/irrlicht && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src/demos/irrlicht/demo_IRR_callbackNSC.cpp -o CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.s

# Object files for target demo_IRR_callbackNSC
demo_IRR_callbackNSC_OBJECTS = \
"CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.o"

# External object files for target demo_IRR_callbackNSC
demo_IRR_callbackNSC_EXTERNAL_OBJECTS =

bin/demo_IRR_callbackNSC: src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/demo_IRR_callbackNSC.cpp.o
bin/demo_IRR_callbackNSC: src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/build.make
bin/demo_IRR_callbackNSC: lib/libChronoEngine_irrlicht.so
bin/demo_IRR_callbackNSC: lib/libChronoEngine.so
bin/demo_IRR_callbackNSC: /usr/lib/libIrrlicht.so
bin/demo_IRR_callbackNSC: src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jason/Desktop/FileTransfer/chrono_build_SCM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../bin/demo_IRR_callbackNSC"
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/irrlicht && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/demo_IRR_callbackNSC.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/build: bin/demo_IRR_callbackNSC

.PHONY : src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/build

src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/clean:
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/irrlicht && $(CMAKE_COMMAND) -P CMakeFiles/demo_IRR_callbackNSC.dir/cmake_clean.cmake
.PHONY : src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/clean

src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/depend:
	cd /home/jason/Desktop/FileTransfer/chrono_build_SCM && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63 /home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src/demos/irrlicht /home/jason/Desktop/FileTransfer/chrono_build_SCM /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/irrlicht /home/jason/Desktop/FileTransfer/chrono_build_SCM/src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/demos/irrlicht/CMakeFiles/demo_IRR_callbackNSC.dir/depend

