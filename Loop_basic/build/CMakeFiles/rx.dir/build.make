# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yufeng/Net4FPGAs/Loop_basic

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yufeng/Net4FPGAs/Loop_basic/build

# Include any dependencies generated for this target.
include CMakeFiles/rx.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/rx.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/rx.dir/flags.make

CMakeFiles/rx.dir/exec/alveo_rx.cpp.o: CMakeFiles/rx.dir/flags.make
CMakeFiles/rx.dir/exec/alveo_rx.cpp.o: ../exec/alveo_rx.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yufeng/Net4FPGAs/Loop_basic/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/rx.dir/exec/alveo_rx.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/rx.dir/exec/alveo_rx.cpp.o -c /home/yufeng/Net4FPGAs/Loop_basic/exec/alveo_rx.cpp

CMakeFiles/rx.dir/exec/alveo_rx.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rx.dir/exec/alveo_rx.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yufeng/Net4FPGAs/Loop_basic/exec/alveo_rx.cpp > CMakeFiles/rx.dir/exec/alveo_rx.cpp.i

CMakeFiles/rx.dir/exec/alveo_rx.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rx.dir/exec/alveo_rx.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yufeng/Net4FPGAs/Loop_basic/exec/alveo_rx.cpp -o CMakeFiles/rx.dir/exec/alveo_rx.cpp.s

# Object files for target rx
rx_OBJECTS = \
"CMakeFiles/rx.dir/exec/alveo_rx.cpp.o"

# External object files for target rx
rx_EXTERNAL_OBJECTS =

bin/rx: CMakeFiles/rx.dir/exec/alveo_rx.cpp.o
bin/rx: CMakeFiles/rx.dir/build.make
bin/rx: lib/libalveo_vnx.so
bin/rx: CMakeFiles/rx.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yufeng/Net4FPGAs/Loop_basic/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/rx"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rx.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/rx.dir/build: bin/rx

.PHONY : CMakeFiles/rx.dir/build

CMakeFiles/rx.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/rx.dir/cmake_clean.cmake
.PHONY : CMakeFiles/rx.dir/clean

CMakeFiles/rx.dir/depend:
	cd /home/yufeng/Net4FPGAs/Loop_basic/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yufeng/Net4FPGAs/Loop_basic /home/yufeng/Net4FPGAs/Loop_basic /home/yufeng/Net4FPGAs/Loop_basic/build /home/yufeng/Net4FPGAs/Loop_basic/build /home/yufeng/Net4FPGAs/Loop_basic/build/CMakeFiles/rx.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/rx.dir/depend
