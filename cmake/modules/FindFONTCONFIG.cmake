#
# Find the native FontConfig includes and library
#
# This module defines:
#   FONTCONFIG_INCLUDE_DIRS  - where to find fontconfig/fontconfig.h
#   FONTCONFIG_LIBRARIES     - libraries to link against
#   FONTCONFIG_FOUND         - TRUE if FontConfig was found

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_FONTCONFIG QUIET fontconfig)
endif()

find_path(FONTCONFIG_INCLUDE_DIR
  NAMES fontconfig/fontconfig.h
  HINTS ${PC_FONTCONFIG_INCLUDE_DIRS}
  PATHS /usr/include /usr/local/include
)

find_library(FONTCONFIG_LIBRARY
  NAMES fontconfig
  HINTS ${PC_FONTCONFIG_LIBRARY_DIRS}
  PATHS /usr/lib /usr/local/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FONTCONFIG
  DEFAULT_MSG
  FONTCONFIG_LIBRARY
  FONTCONFIG_INCLUDE_DIR
)

if(FONTCONFIG_FOUND)
  set(FONTCONFIG_LIBRARIES ${FONTCONFIG_LIBRARY})
  set(FONTCONFIG_INCLUDE_DIRS ${FONTCONFIG_INCLUDE_DIR})
endif()

mark_as_advanced(FONTCONFIG_INCLUDE_DIR FONTCONFIG_LIBRARY)
