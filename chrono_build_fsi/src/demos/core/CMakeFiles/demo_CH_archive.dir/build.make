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
CMAKE_SOURCE_DIR = /home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi

# Include any dependencies generated for this target.
include src/demos/core/CMakeFiles/demo_CH_archive.dir/depend.make

# Include the progress variables for this target.
include src/demos/core/CMakeFiles/demo_CH_archive.dir/progress.make

# Include the compile flags for this target's objects.
include src/demos/core/CMakeFiles/demo_CH_archive.dir/flags.make

src/demos/core/CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.o: src/demos/core/CMakeFiles/demo_CH_archive.dir/flags.make
src/demos/core/CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.o: /home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/demos/core/demo_CH_archive.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/demos/core/CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.o"
	cd /home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/demos/core && /usr/bin/ccache /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.o -c /home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/demos/core/demo_CH_archive.cpp

src/demos/core/CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.i"
	cd /home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/demos/core && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/demos/core/demo_CH_archive.cpp > CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.i

src/demos/core/CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.s"
	cd /home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/demos/core && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/demos/core/demo_CH_archive.cpp -o CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.s

# Object files for target demo_CH_archive
demo_CH_archive_OBJECTS = \
"CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.o"

# External object files for target demo_CH_archive
demo_CH_archive_EXTERNAL_OBJECTS =

bin/demo_CH_archive: src/demos/core/CMakeFiles/demo_CH_archive.dir/demo_CH_archive.cpp.o
bin/demo_CH_archive: src/demos/core/CMakeFiles/demo_CH_archive.dir/build.make
bin/demo_CH_archive: lib/libChronoEngine.so
bin/demo_CH_archive: src/demos/core/CMakeFiles/demo_CH_archive.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../bin/demo_CH_archive"
	cd /home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/demos/core && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/demo_CH_archive.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/demos/core/CMakeFiles/demo_CH_archive.dir/build: bin/demo_CH_archive

.PHONY : src/demos/core/CMakeFiles/demo_CH_archive.dir/build

src/demos/core/CMakeFiles/demo_CH_archive.dir/clean:
	cd /home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/demos/core && $(CMAKE_COMMAND) -P CMakeFiles/demo_CH_archive.dir/cmake_clean.cmake
.PHONY : src/demos/core/CMakeFiles/demo_CH_archive.dir/clean

src/demos/core/CMakeFiles/demo_CH_archive.dir/depend:
	cd /home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular /home/jason/Desktop/FileTransfer/FileTransfer/chrono-dev-sandbox-fsi-I2SPH-granular/src/demos/core /home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi /home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/demos/core /home/jason/Desktop/FileTransfer/FileTransfer/chrono_build_fsi/src/demos/core/CMakeFiles/demo_CH_archive.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/demos/core/CMakeFiles/demo_CH_archive.dir/depend

