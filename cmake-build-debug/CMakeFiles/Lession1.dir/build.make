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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/fengbo/CLionProjects/TinyRenderPractice

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/fengbo/CLionProjects/TinyRenderPractice/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Lession1.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Lession1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Lession1.dir/flags.make

CMakeFiles/Lession1.dir/tgaimage.cpp.o: CMakeFiles/Lession1.dir/flags.make
CMakeFiles/Lession1.dir/tgaimage.cpp.o: ../tgaimage.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/fengbo/CLionProjects/TinyRenderPractice/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Lession1.dir/tgaimage.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Lession1.dir/tgaimage.cpp.o -c /Users/fengbo/CLionProjects/TinyRenderPractice/tgaimage.cpp

CMakeFiles/Lession1.dir/tgaimage.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Lession1.dir/tgaimage.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/fengbo/CLionProjects/TinyRenderPractice/tgaimage.cpp > CMakeFiles/Lession1.dir/tgaimage.cpp.i

CMakeFiles/Lession1.dir/tgaimage.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Lession1.dir/tgaimage.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/fengbo/CLionProjects/TinyRenderPractice/tgaimage.cpp -o CMakeFiles/Lession1.dir/tgaimage.cpp.s

CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.o: CMakeFiles/Lession1.dir/flags.make
CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.o: ../L1_Bresenham_Line_Algorithm.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/fengbo/CLionProjects/TinyRenderPractice/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.o -c /Users/fengbo/CLionProjects/TinyRenderPractice/L1_Bresenham_Line_Algorithm.cpp

CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/fengbo/CLionProjects/TinyRenderPractice/L1_Bresenham_Line_Algorithm.cpp > CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.i

CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/fengbo/CLionProjects/TinyRenderPractice/L1_Bresenham_Line_Algorithm.cpp -o CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.s

# Object files for target Lession1
Lession1_OBJECTS = \
"CMakeFiles/Lession1.dir/tgaimage.cpp.o" \
"CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.o"

# External object files for target Lession1
Lession1_EXTERNAL_OBJECTS =

Lession1: CMakeFiles/Lession1.dir/tgaimage.cpp.o
Lession1: CMakeFiles/Lession1.dir/L1_Bresenham_Line_Algorithm.cpp.o
Lession1: CMakeFiles/Lession1.dir/build.make
Lession1: CMakeFiles/Lession1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/fengbo/CLionProjects/TinyRenderPractice/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable Lession1"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Lession1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Lession1.dir/build: Lession1

.PHONY : CMakeFiles/Lession1.dir/build

CMakeFiles/Lession1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Lession1.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Lession1.dir/clean

CMakeFiles/Lession1.dir/depend:
	cd /Users/fengbo/CLionProjects/TinyRenderPractice/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/fengbo/CLionProjects/TinyRenderPractice /Users/fengbo/CLionProjects/TinyRenderPractice /Users/fengbo/CLionProjects/TinyRenderPractice/cmake-build-debug /Users/fengbo/CLionProjects/TinyRenderPractice/cmake-build-debug /Users/fengbo/CLionProjects/TinyRenderPractice/cmake-build-debug/CMakeFiles/Lession1.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Lession1.dir/depend
