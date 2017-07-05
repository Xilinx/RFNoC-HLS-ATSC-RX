# Install script for directory: /home/switchlanez/rfnoc/src/gnuradio/volk

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/switchlanez/rfnoc")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "volk_devel")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/switchlanez/rfnoc/src/gnuradio/build/volk/volk.pc")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "volk_devel")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/switchlanez/rfnoc/src/gnuradio/volk/kernels/volk" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "volk_devel")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/volk" TYPE FILE FILES
    "/home/switchlanez/rfnoc/src/gnuradio/volk/include/volk/volk_prefs.h"
    "/home/switchlanez/rfnoc/src/gnuradio/volk/include/volk/volk_complex.h"
    "/home/switchlanez/rfnoc/src/gnuradio/volk/include/volk/volk_common.h"
    "/home/switchlanez/rfnoc/src/gnuradio/volk/include/volk/volk_avx_intrinsics.h"
    "/home/switchlanez/rfnoc/src/gnuradio/volk/include/volk/volk_sse3_intrinsics.h"
    "/home/switchlanez/rfnoc/src/gnuradio/volk/include/volk/volk_neon_intrinsics.h"
    "/home/switchlanez/rfnoc/src/gnuradio/build/volk/include/volk/volk.h"
    "/home/switchlanez/rfnoc/src/gnuradio/build/volk/include/volk/volk_cpu.h"
    "/home/switchlanez/rfnoc/src/gnuradio/build/volk/include/volk/volk_config_fixed.h"
    "/home/switchlanez/rfnoc/src/gnuradio/build/volk/include/volk/volk_typedefs.h"
    "/home/switchlanez/rfnoc/src/gnuradio/volk/include/volk/volk_malloc.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "volk_devel")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/volk" TYPE FILE FILES
    "/home/switchlanez/rfnoc/src/gnuradio/build/volk/cmake/Modules/VolkConfig.cmake"
    "/home/switchlanez/rfnoc/src/gnuradio/build/volk/cmake/Modules/VolkConfigVersion.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/switchlanez/rfnoc/src/gnuradio/build/volk/lib/cmake_install.cmake")
  include("/home/switchlanez/rfnoc/src/gnuradio/build/volk/apps/cmake_install.cmake")
  include("/home/switchlanez/rfnoc/src/gnuradio/build/volk/python/volk_modtool/cmake_install.cmake")

endif()

