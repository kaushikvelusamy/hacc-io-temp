# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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
CMAKE_COMMAND = /opt/aurora/24.180.3/spack/unified/0.8.0/install/linux-sles15-x86_64/gcc-12.2.0/cmake-3.27.9-ph5bjg4/bin/cmake

# The command to remove a file.
RM = /opt/aurora/24.180.3/spack/unified/0.8.0/install/linux-sles15-x86_64/gcc-12.2.0/cmake-3.27.9-ph5bjg4/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io/mpi

# Include any dependencies generated for this target.
include CMakeFiles/GenericIOPrint.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/GenericIOPrint.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/GenericIOPrint.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/GenericIOPrint.dir/flags.make

CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.o: CMakeFiles/GenericIOPrint.dir/flags.make
CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.o: /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io/GenericIOPrint.cxx
CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.o: CMakeFiles/GenericIOPrint.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io/mpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.o"
	/opt/aurora/24.180.3/updates/oneapi/compiler/eng-20240629/bin/icpx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.o -MF CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.o.d -o CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.o -c /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io/GenericIOPrint.cxx

CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.i"
	/opt/aurora/24.180.3/updates/oneapi/compiler/eng-20240629/bin/icpx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io/GenericIOPrint.cxx > CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.i

CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.s"
	/opt/aurora/24.180.3/updates/oneapi/compiler/eng-20240629/bin/icpx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io/GenericIOPrint.cxx -o CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.s

# Object files for target GenericIOPrint
GenericIOPrint_OBJECTS = \
"CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.o"

# External object files for target GenericIOPrint
GenericIOPrint_EXTERNAL_OBJECTS =

frontend/GenericIOPrint: CMakeFiles/GenericIOPrint.dir/GenericIOPrint.cxx.o
frontend/GenericIOPrint: CMakeFiles/GenericIOPrint.dir/build.make
frontend/GenericIOPrint: lib/libgenericio.a
frontend/GenericIOPrint: thirdparty/libblosc.a
frontend/GenericIOPrint: /opt/aurora/24.180.3/updates/oneapi/compiler/eng-20240629/lib/libiomp5.so
frontend/GenericIOPrint: /usr/lib64/libpthread.so
frontend/GenericIOPrint: CMakeFiles/GenericIOPrint.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io/mpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable frontend/GenericIOPrint"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/GenericIOPrint.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/GenericIOPrint.dir/build: frontend/GenericIOPrint
.PHONY : CMakeFiles/GenericIOPrint.dir/build

CMakeFiles/GenericIOPrint.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/GenericIOPrint.dir/cmake_clean.cmake
.PHONY : CMakeFiles/GenericIOPrint.dir/clean

CMakeFiles/GenericIOPrint.dir/depend:
	cd /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io/mpi && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io/mpi /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io/mpi /lus/flare/projects/Aurora_deployment/kaushik/april2-daos/hacc-cpr-easy/genric-io/mpi/CMakeFiles/GenericIOPrint.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/GenericIOPrint.dir/depend

