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
CMAKE_SOURCE_DIR = /Users/fengbo/CLionProjects/TinyRenderForMyOwn

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/fengbo/CLionProjects/TinyRenderForMyOwn/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/TinyRenderForMyOwn.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/TinyRenderForMyOwn.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TinyRenderForMyOwn.dir/flags.make

CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.o: CMakeFiles/TinyRenderForMyOwn.dir/flags.make
CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/fengbo/CLionProjects/TinyRenderForMyOwn/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.o -c /Users/fengbo/CLionProjects/TinyRenderForMyOwn/main.cpp

CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/fengbo/CLionProjects/TinyRenderForMyOwn/main.cpp > CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.i

CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/fengbo/CLionProjects/TinyRenderForMyOwn/main.cpp -o CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.s

CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.o: CMakeFiles/TinyRenderForMyOwn.dir/flags.make
CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.o: ../tgaimage.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/fengbo/CLionProjects/TinyRenderForMyOwn/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.o -c /Users/fengbo/CLionProjects/TinyRenderForMyOwn/tgaimage.cpp

CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/fengbo/CLionProjects/TinyRenderForMyOwn/tgaimage.cpp > CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.i

CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/fengbo/CLionProjects/TinyRenderForMyOwn/tgaimage.cpp -o CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.s

# Object files for target TinyRenderForMyOwn
TinyRenderForMyOwn_OBJECTS = \
"CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.o" \
"CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.o"

# External object files for target TinyRenderForMyOwn
TinyRenderForMyOwn_EXTERNAL_OBJECTS =

TinyRenderForMyOwn: CMakeFiles/TinyRenderForMyOwn.dir/main.cpp.o
TinyRenderForMyOwn: CMakeFiles/TinyRenderForMyOwn.dir/tgaimage.cpp.o
TinyRenderForMyOwn: CMakeFiles/TinyRenderForMyOwn.dir/build.make
TinyRenderForMyOwn: CMakeFiles/TinyRenderForMyOwn.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/fengbo/CLionProjects/TinyRenderForMyOwn/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable TinyRenderForMyOwn"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TinyRenderForMyOwn.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TinyRenderForMyOwn.dir/build: TinyRenderForMyOwn

.PHONY : CMakeFiles/TinyRenderForMyOwn.dir/build

CMakeFiles/TinyRenderForMyOwn.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TinyRenderForMyOwn.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TinyRenderForMyOwn.dir/clean

CMakeFiles/TinyRenderForMyOwn.dir/depend:
	cd /Users/fengbo/CLionProjects/TinyRenderForMyOwn/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/fengbo/CLionProjects/TinyRenderForMyOwn /Users/fengbo/CLionProjects/TinyRenderForMyOwn /Users/fengbo/CLionProjects/TinyRenderForMyOwn/cmake-build-debug /Users/fengbo/CLionProjects/TinyRenderForMyOwn/cmake-build-debug /Users/fengbo/CLionProjects/TinyRenderForMyOwn/cmake-build-debug/CMakeFiles/TinyRenderForMyOwn.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TinyRenderForMyOwn.dir/depend

