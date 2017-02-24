# project
cmake_minimum_required (VERSION 2.6)
project (stereo_viso2)

# directories
set (LIBVISO2_SRC_DIR /home/lawrence/libviso2/src)
set (TARA_INCLUDES /home/lawrence/CMU/research/tara/Tara_SDK_LINUX_REL_package_2.0.4/Source/Common)
set (OPENCV_PREFIX /usr/local/tara-opencv)
set (TARA_LIBS /home/lawrence/CMU/research/tara/Tara_SDK_LINUX_REL_package_2.0.4/Prebuilts/Ubuntu-14.04)

# include directory
include_directories("${LIBVISO2_SRC_DIR}")
include_directories("${TARA_INCLUDES}/include")
include_directories("${OPENCV_PREFIX}/include")
include_directories("/usr/include/glib-2.0")
include_directories("/usr/lib/x86_64-linux-gnu/glib-2.0/include")

# library directory
link_directories("${TARA_LIBS}/lib_x64")

# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")

# sources
FILE(GLOB LIBVISO2_SRC_FILES "/home/lawrence/libviso2/src/*.cpp")

# make release version
set(CMAKE_BUILD_TYPE Release)

# demo program
add_executable(stereo_viso2 stereo_viso2.cpp ${LIBVISO2_SRC_FILES})
target_link_libraries (stereo_viso2 png econ_tara econ_xunit)

