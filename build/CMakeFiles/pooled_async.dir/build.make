# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/cmake/715/bin/cmake

# The command to remove a file.
RM = /snap/cmake/715/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/build

# Include any dependencies generated for this target.
include CMakeFiles/pooled_async.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/pooled_async.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/pooled_async.dir/flags.make

CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.o: CMakeFiles/pooled_async.dir/flags.make
CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.o: ../backbone/async_serving_v2.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.o -c /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/backbone/async_serving_v2.c

CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/backbone/async_serving_v2.c > CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.i

CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/backbone/async_serving_v2.c -o CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.s

CMakeFiles/pooled_async.dir/backbone/async_threading.c.o: CMakeFiles/pooled_async.dir/flags.make
CMakeFiles/pooled_async.dir/backbone/async_threading.c.o: ../backbone/async_threading.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/pooled_async.dir/backbone/async_threading.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/pooled_async.dir/backbone/async_threading.c.o -c /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/backbone/async_threading.c

CMakeFiles/pooled_async.dir/backbone/async_threading.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/pooled_async.dir/backbone/async_threading.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/backbone/async_threading.c > CMakeFiles/pooled_async.dir/backbone/async_threading.c.i

CMakeFiles/pooled_async.dir/backbone/async_threading.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/pooled_async.dir/backbone/async_threading.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/backbone/async_threading.c -o CMakeFiles/pooled_async.dir/backbone/async_threading.c.s

CMakeFiles/pooled_async.dir/backbone/async_process.c.o: CMakeFiles/pooled_async.dir/flags.make
CMakeFiles/pooled_async.dir/backbone/async_process.c.o: ../backbone/async_process.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/pooled_async.dir/backbone/async_process.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/pooled_async.dir/backbone/async_process.c.o -c /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/backbone/async_process.c

CMakeFiles/pooled_async.dir/backbone/async_process.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/pooled_async.dir/backbone/async_process.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/backbone/async_process.c > CMakeFiles/pooled_async.dir/backbone/async_process.c.i

CMakeFiles/pooled_async.dir/backbone/async_process.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/pooled_async.dir/backbone/async_process.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/backbone/async_process.c -o CMakeFiles/pooled_async.dir/backbone/async_process.c.s

# Object files for target pooled_async
pooled_async_OBJECTS = \
"CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.o" \
"CMakeFiles/pooled_async.dir/backbone/async_threading.c.o" \
"CMakeFiles/pooled_async.dir/backbone/async_process.c.o"

# External object files for target pooled_async
pooled_async_EXTERNAL_OBJECTS =

libpooled_async.so: CMakeFiles/pooled_async.dir/backbone/async_serving_v2.c.o
libpooled_async.so: CMakeFiles/pooled_async.dir/backbone/async_threading.c.o
libpooled_async.so: CMakeFiles/pooled_async.dir/backbone/async_process.c.o
libpooled_async.so: CMakeFiles/pooled_async.dir/build.make
libpooled_async.so: CMakeFiles/pooled_async.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C shared library libpooled_async.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pooled_async.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/pooled_async.dir/build: libpooled_async.so

.PHONY : CMakeFiles/pooled_async.dir/build

CMakeFiles/pooled_async.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/pooled_async.dir/cmake_clean.cmake
.PHONY : CMakeFiles/pooled_async.dir/clean

CMakeFiles/pooled_async.dir/depend:
	cd /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/build /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/build /home/karel/Documents/program/COMP-4981_super_dc/c-saw_server/build/CMakeFiles/pooled_async.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/pooled_async.dir/depend

