# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.4

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice

# Include any dependencies generated for this target.
include CMakeFiles/cyber_upnp_renderer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cyber_upnp_renderer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cyber_upnp_renderer.dir/flags.make

CMakeFiles/cyber_upnp_renderer.dir/depend.make.mark: CMakeFiles/cyber_upnp_renderer.dir/flags.make
CMakeFiles/cyber_upnp_renderer.dir/depend.make.mark: RenderDevice.cpp

CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o: CMakeFiles/cyber_upnp_renderer.dir/flags.make
CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o: RenderDevice.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o"
	/usr/bin/c++   $(CXX_FLAGS) -o CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o -c /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice/RenderDevice.cpp

CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.i"
	/usr/bin/c++  $(CXX_FLAGS) -E /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice/RenderDevice.cpp > CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.i

CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.s"
	/usr/bin/c++  $(CXX_FLAGS) -S /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice/RenderDevice.cpp -o CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.s

CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o.requires:

CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o.provides: CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o.requires
	$(MAKE) -f CMakeFiles/cyber_upnp_renderer.dir/build.make CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o.provides.build

CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o.provides.build: CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o

CMakeFiles/cyber_upnp_renderer.dir/depend.make.mark: CMakeFiles/cyber_upnp_renderer.dir/flags.make
CMakeFiles/cyber_upnp_renderer.dir/depend.make.mark: RenderMain.cpp

CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o: CMakeFiles/cyber_upnp_renderer.dir/flags.make
CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o: RenderMain.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o"
	/usr/bin/c++   $(CXX_FLAGS) -o CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o -c /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice/RenderMain.cpp

CMakeFiles/cyber_upnp_renderer.dir/RenderMain.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cyber_upnp_renderer.dir/RenderMain.i"
	/usr/bin/c++  $(CXX_FLAGS) -E /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice/RenderMain.cpp > CMakeFiles/cyber_upnp_renderer.dir/RenderMain.i

CMakeFiles/cyber_upnp_renderer.dir/RenderMain.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cyber_upnp_renderer.dir/RenderMain.s"
	/usr/bin/c++  $(CXX_FLAGS) -S /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice/RenderMain.cpp -o CMakeFiles/cyber_upnp_renderer.dir/RenderMain.s

CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o.requires:

CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o.provides: CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o.requires
	$(MAKE) -f CMakeFiles/cyber_upnp_renderer.dir/build.make CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o.provides.build

CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o.provides.build: CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o

CMakeFiles/cyber_upnp_renderer.dir/depend: CMakeFiles/cyber_upnp_renderer.dir/depend.make.mark

CMakeFiles/cyber_upnp_renderer.dir/depend.make.mark:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --magenta --bold "Scanning dependencies of target cyber_upnp_renderer"
	cd /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice /home/jb/devel/cc/jamm/test/upnp/cyber_upnp/renderdevice/CMakeFiles/cyber_upnp_renderer.dir/DependInfo.cmake

# Object files for target cyber_upnp_renderer
cyber_upnp_renderer_OBJECTS = \
"CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o" \
"CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o"

# External object files for target cyber_upnp_renderer
cyber_upnp_renderer_EXTERNAL_OBJECTS =

cyber_upnp_renderer: CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o
cyber_upnp_renderer: CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o
cyber_upnp_renderer: CMakeFiles/cyber_upnp_renderer.dir/build.make
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable cyber_upnp_renderer"
	$(CMAKE_COMMAND) -P CMakeFiles/cyber_upnp_renderer.dir/cmake_clean_target.cmake
	/usr/bin/c++      -fPIC $(cyber_upnp_renderer_OBJECTS) $(cyber_upnp_renderer_EXTERNAL_OBJECTS)  -o cyber_upnp_renderer -rdynamic -L../../../../../jammbin/src/system/cyber_upnp -ljamm-system-cyber_upnp 

# Rule to build all files generated by this target.
CMakeFiles/cyber_upnp_renderer.dir/build: cyber_upnp_renderer

CMakeFiles/cyber_upnp_renderer.dir/requires: CMakeFiles/cyber_upnp_renderer.dir/RenderDevice.o.requires
CMakeFiles/cyber_upnp_renderer.dir/requires: CMakeFiles/cyber_upnp_renderer.dir/RenderMain.o.requires

CMakeFiles/cyber_upnp_renderer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cyber_upnp_renderer.dir/cmake_clean.cmake

