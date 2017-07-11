INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_ATSC_RX atsc_rx)

FIND_PATH(
    ATSC_RX_INCLUDE_DIRS
    NAMES atsc_rx/api.h
    HINTS $ENV{ATSC_RX_DIR}/include
        ${PC_ATSC_RX_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    ATSC_RX_LIBRARIES
    NAMES gnuradio-atsc_rx
    HINTS $ENV{ATSC_RX_DIR}/lib
        ${PC_ATSC_RX_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ATSC_RX DEFAULT_MSG ATSC_RX_LIBRARIES ATSC_RX_INCLUDE_DIRS)
MARK_AS_ADVANCED(ATSC_RX_LIBRARIES ATSC_RX_INCLUDE_DIRS)

