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
include CMakeFiles/tx.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tx.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tx.dir/flags.make

CMakeFiles/tx.dir/exec/alveo_tx.cpp.o: CMakeFiles/tx.dir/flags.make
CMakeFiles/tx.dir/exec/alveo_tx.cpp.o: ../exec/alveo_tx.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yufeng/Net4FPGAs/Loop_basic/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tx.dir/exec/alveo_tx.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tx.dir/exec/alveo_tx.cpp.o -c /home/yufeng/Net4FPGAs/Loop_basic/exec/alveo_tx.cpp

CMakeFiles/tx.dir/exec/alveo_tx.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tx.dir/exec/alveo_tx.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yufeng/Net4FPGAs/Loop_basic/exec/alveo_tx.cpp > CMakeFiles/tx.dir/exec/alveo_tx.cpp.i

CMakeFiles/tx.dir/exec/alveo_tx.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tx.dir/exec/alveo_tx.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yufeng/Net4FPGAs/Loop_basic/exec/alveo_tx.cpp -o CMakeFiles/tx.dir/exec/alveo_tx.cpp.s

# Object files for target tx
tx_OBJECTS = \
"CMakeFiles/tx.dir/exec/alveo_tx.cpp.o"

# External object files for target tx
tx_EXTERNAL_OBJECTS =

bin/tx: CMakeFiles/tx.dir/exec/alveo_tx.cpp.o
bin/tx: CMakeFiles/tx.dir/build.make
bin/tx: lib/libalveo_vnx.so
bin/tx: CMakeFiles/tx.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yufeng/Net4FPGAs/Loop_basic/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/tx"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tx.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tx.dir/build: bin/tx

.PHONY : CMakeFiles/tx.dir/build

CMakeFiles/tx.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tx.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tx.dir/clean

CMakeFiles/tx.dir/depend:
	cd /home/yufeng/Net4FPGAs/Loop_basic/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yufeng/Net4FPGAs/Loop_basic /home/yufeng/Net4FPGAs/Loop_basic /home/yufeng/Net4FPGAs/Loop_basic/build /home/yufeng/Net4FPGAs/Loop_basic/build /home/yufeng/Net4FPGAs/Loop_basic/build/CMakeFiles/tx.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tx.dir/depend

