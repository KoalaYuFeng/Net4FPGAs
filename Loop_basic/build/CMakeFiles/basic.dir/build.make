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
include CMakeFiles/basic.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/basic.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/basic.dir/flags.make

CMakeFiles/basic.dir/exec/alveo_basic.cpp.o: CMakeFiles/basic.dir/flags.make
CMakeFiles/basic.dir/exec/alveo_basic.cpp.o: ../exec/alveo_basic.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yufeng/Net4FPGAs/Loop_basic/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/basic.dir/exec/alveo_basic.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/basic.dir/exec/alveo_basic.cpp.o -c /home/yufeng/Net4FPGAs/Loop_basic/exec/alveo_basic.cpp

CMakeFiles/basic.dir/exec/alveo_basic.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/basic.dir/exec/alveo_basic.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yufeng/Net4FPGAs/Loop_basic/exec/alveo_basic.cpp > CMakeFiles/basic.dir/exec/alveo_basic.cpp.i

CMakeFiles/basic.dir/exec/alveo_basic.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/basic.dir/exec/alveo_basic.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yufeng/Net4FPGAs/Loop_basic/exec/alveo_basic.cpp -o CMakeFiles/basic.dir/exec/alveo_basic.cpp.s

# Object files for target basic
basic_OBJECTS = \
"CMakeFiles/basic.dir/exec/alveo_basic.cpp.o"

# External object files for target basic
basic_EXTERNAL_OBJECTS =

bin/basic: CMakeFiles/basic.dir/exec/alveo_basic.cpp.o
bin/basic: CMakeFiles/basic.dir/build.make
bin/basic: lib/libalveo_vnx.so
bin/basic: CMakeFiles/basic.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yufeng/Net4FPGAs/Loop_basic/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/basic"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/basic.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/basic.dir/build: bin/basic

.PHONY : CMakeFiles/basic.dir/build

CMakeFiles/basic.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/basic.dir/cmake_clean.cmake
.PHONY : CMakeFiles/basic.dir/clean

CMakeFiles/basic.dir/depend:
	cd /home/yufeng/Net4FPGAs/Loop_basic/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yufeng/Net4FPGAs/Loop_basic /home/yufeng/Net4FPGAs/Loop_basic /home/yufeng/Net4FPGAs/Loop_basic/build /home/yufeng/Net4FPGAs/Loop_basic/build /home/yufeng/Net4FPGAs/Loop_basic/build/CMakeFiles/basic.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/basic.dir/depend
