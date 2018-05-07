
# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/marcelo/cpp/gitProjects/tooldoce

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/marcelo/cpp/gitProjects/tooldoce

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/cmake-gui -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/marcelo/cpp/gitProjects/tooldoce/CMakeFiles /home/marcelo/cpp/gitProjects/tooldoce/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/marcelo/cpp/gitProjects/tooldoce/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named libtooldoce

# Build rule for target.
libtooldoce: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 libtooldoce
.PHONY : libtooldoce

# fast build rule for target.
libtooldoce/fast:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/build
.PHONY : libtooldoce/fast

src/libnfe/NFref.o: src/libnfe/NFref.c.o

.PHONY : src/libnfe/NFref.o

# target to build an object file
src/libnfe/NFref.c.o:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/NFref.c.o
.PHONY : src/libnfe/NFref.c.o

src/libnfe/NFref.i: src/libnfe/NFref.c.i

.PHONY : src/libnfe/NFref.i

# target to preprocess a source file
src/libnfe/NFref.c.i:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/NFref.c.i
.PHONY : src/libnfe/NFref.c.i

src/libnfe/NFref.s: src/libnfe/NFref.c.s

.PHONY : src/libnfe/NFref.s

# target to generate assembly for a file
src/libnfe/NFref.c.s:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/NFref.c.s
.PHONY : src/libnfe/NFref.c.s

src/libnfe/endereco.o: src/libnfe/endereco.c.o

.PHONY : src/libnfe/endereco.o

# target to build an object file
src/libnfe/endereco.c.o:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/endereco.c.o
.PHONY : src/libnfe/endereco.c.o

src/libnfe/endereco.i: src/libnfe/endereco.c.i

.PHONY : src/libnfe/endereco.i

# target to preprocess a source file
src/libnfe/endereco.c.i:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/endereco.c.i
.PHONY : src/libnfe/endereco.c.i

src/libnfe/endereco.s: src/libnfe/endereco.c.s

.PHONY : src/libnfe/endereco.s

# target to generate assembly for a file
src/libnfe/endereco.c.s:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/endereco.c.s
.PHONY : src/libnfe/endereco.c.s

src/libnfe/ide.o: src/libnfe/ide.c.o

.PHONY : src/libnfe/ide.o

# target to build an object file
src/libnfe/ide.c.o:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/ide.c.o
.PHONY : src/libnfe/ide.c.o

src/libnfe/ide.i: src/libnfe/ide.c.i

.PHONY : src/libnfe/ide.i

# target to preprocess a source file
src/libnfe/ide.c.i:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/ide.c.i
.PHONY : src/libnfe/ide.c.i

src/libnfe/ide.s: src/libnfe/ide.c.s

.PHONY : src/libnfe/ide.s

# target to generate assembly for a file
src/libnfe/ide.c.s:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/ide.c.s
.PHONY : src/libnfe/ide.c.s

src/libnfe/refNF.o: src/libnfe/refNF.c.o

.PHONY : src/libnfe/refNF.o

# target to build an object file
src/libnfe/refNF.c.o:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/refNF.c.o
.PHONY : src/libnfe/refNF.c.o

src/libnfe/refNF.i: src/libnfe/refNF.c.i

.PHONY : src/libnfe/refNF.i

# target to preprocess a source file
src/libnfe/refNF.c.i:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/refNF.c.i
.PHONY : src/libnfe/refNF.c.i

src/libnfe/refNF.s: src/libnfe/refNF.c.s

.PHONY : src/libnfe/refNF.s

# target to generate assembly for a file
src/libnfe/refNF.c.s:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/refNF.c.s
.PHONY : src/libnfe/refNF.c.s

src/libnfe/utils.o: src/libnfe/utils.c.o

.PHONY : src/libnfe/utils.o

# target to build an object file
src/libnfe/utils.c.o:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/utils.c.o
.PHONY : src/libnfe/utils.c.o

src/libnfe/utils.i: src/libnfe/utils.c.i

.PHONY : src/libnfe/utils.i

# target to preprocess a source file
src/libnfe/utils.c.i:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/utils.c.i
.PHONY : src/libnfe/utils.c.i

src/libnfe/utils.s: src/libnfe/utils.c.s

.PHONY : src/libnfe/utils.s

# target to generate assembly for a file
src/libnfe/utils.c.s:
	$(MAKE) -f CMakeFiles/libtooldoce.dir/build.make CMakeFiles/libtooldoce.dir/src/libnfe/utils.c.s
.PHONY : src/libnfe/utils.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... libtooldoce"
	@echo "... edit_cache"
	@echo "... src/libnfe/NFref.o"
	@echo "... src/libnfe/NFref.i"
	@echo "... src/libnfe/NFref.s"
	@echo "... src/libnfe/endereco.o"
	@echo "... src/libnfe/endereco.i"
	@echo "... src/libnfe/endereco.s"
	@echo "... src/libnfe/ide.o"
	@echo "... src/libnfe/ide.i"
	@echo "... src/libnfe/ide.s"
	@echo "... src/libnfe/refNF.o"
	@echo "... src/libnfe/refNF.i"
	@echo "... src/libnfe/refNF.s"
	@echo "... src/libnfe/utils.o"
	@echo "... src/libnfe/utils.i"
	@echo "... src/libnfe/utils.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

