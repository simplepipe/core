# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /home/manh/Documents/cmake-3.12.2-Linux-x86_64/bin/cmake

# The command to remove a file.
RM = /home/manh/Documents/cmake-3.12.2-Linux-x86_64/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/manh/Documents/Projects/core/physics

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/manh/Documents/Projects/core/physics/build/droid/mips/Release

# Include any dependencies generated for this target.
include CMakeFiles/physics.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/physics.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/physics.dir/flags.make

CMakeFiles/physics.dir/src/particle.c.o: CMakeFiles/physics.dir/flags.make
CMakeFiles/physics.dir/src/particle.c.o: ../../../../src/particle.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/manh/Documents/Projects/core/physics/build/droid/mips/Release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/physics.dir/src/particle.c.o"
	/home/manh/Documents/android-ndk-r17c/toolchains/llvm/prebuilt/linux-x86_64/bin/clang --target=armv7-none-linux-androideabi --gcc-toolchain=/home/manh/Documents/android-ndk-r17c/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64 --sysroot=/home/manh/Documents/android-ndk-r17c/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/physics.dir/src/particle.c.o   -c /home/manh/Documents/Projects/core/physics/src/particle.c

CMakeFiles/physics.dir/src/particle.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/physics.dir/src/particle.c.i"
	/home/manh/Documents/android-ndk-r17c/toolchains/llvm/prebuilt/linux-x86_64/bin/clang --target=armv7-none-linux-androideabi --gcc-toolchain=/home/manh/Documents/android-ndk-r17c/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64 --sysroot=/home/manh/Documents/android-ndk-r17c/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/manh/Documents/Projects/core/physics/src/particle.c > CMakeFiles/physics.dir/src/particle.c.i

CMakeFiles/physics.dir/src/particle.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/physics.dir/src/particle.c.s"
	/home/manh/Documents/android-ndk-r17c/toolchains/llvm/prebuilt/linux-x86_64/bin/clang --target=armv7-none-linux-androideabi --gcc-toolchain=/home/manh/Documents/android-ndk-r17c/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64 --sysroot=/home/manh/Documents/android-ndk-r17c/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/manh/Documents/Projects/core/physics/src/particle.c -o CMakeFiles/physics.dir/src/particle.c.s

# Object files for target physics
physics_OBJECTS = \
"CMakeFiles/physics.dir/src/particle.c.o"

# External object files for target physics
physics_EXTERNAL_OBJECTS =

libphysics.a: CMakeFiles/physics.dir/src/particle.c.o
libphysics.a: CMakeFiles/physics.dir/build.make
libphysics.a: CMakeFiles/physics.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/manh/Documents/Projects/core/physics/build/droid/mips/Release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libphysics.a"
	$(CMAKE_COMMAND) -P CMakeFiles/physics.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/physics.dir/link.txt --verbose=$(VERBOSE)
	/home/manh/Documents/android-ndk-r17c/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-strip libphysics.a

# Rule to build all files generated by this target.
CMakeFiles/physics.dir/build: libphysics.a

.PHONY : CMakeFiles/physics.dir/build

CMakeFiles/physics.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/physics.dir/cmake_clean.cmake
.PHONY : CMakeFiles/physics.dir/clean

CMakeFiles/physics.dir/depend:
	cd /home/manh/Documents/Projects/core/physics/build/droid/mips/Release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/manh/Documents/Projects/core/physics /home/manh/Documents/Projects/core/physics /home/manh/Documents/Projects/core/physics/build/droid/mips/Release /home/manh/Documents/Projects/core/physics/build/droid/mips/Release /home/manh/Documents/Projects/core/physics/build/droid/mips/Release/CMakeFiles/physics.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/physics.dir/depend

