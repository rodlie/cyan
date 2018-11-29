###
# -*- cmake -*-
#
# File:  FindLCMS2.cmake
#
# Original script was copied from:
# http://code.google.com/p/emeraldviewer/source/browse/indra/cmake
#
# $Id: FindLCMS2.cmake 23010 2014-12-01 21:47:43Z dburken $
###

# - Find LCMS2
# Find the LCMS2 includes and library
# This module defines
#  LCMS2_INCLUDE_DIR, where to find lcms2.h, etc.
#  LCMS2_LIBRARIES, the libraries needed to use LCMS2.
#  LCMS2_FOUND, If false, do not try to use LCMS2.
# also defined, but not for general use are
#  LCMS2_LIBRARY, where to find the LCMS2 library.

FIND_PATH(LCMS2_INCLUDE_DIR lcms2.h
  PATHS
    /usr/local/include/lcms2
    /usr/local/include
    /usr/include/lcms2
    /usr/include
  PATH_SUFFIXES
    lcms2-2.6
  DOC "Location of LCMS2 Headers"
)

SET(LCMS2_NAMES ${LCMS2_NAMES} lcms2)
FIND_LIBRARY(LCMS2_LIBRARY
  NAMES ${LCMS2_NAMES}
  PATHS /usr/lib /usr/local/lib
  )

IF (LCMS2_LIBRARY AND LCMS2_INCLUDE_DIR)
    SET(LCMS2_LIBRARIES ${LCMS2_LIBRARY})
    SET(LCMS2_FOUND "YES")
ELSE (LCMS2_LIBRARY AND LCMS2_INCLUDE_DIR)
  SET(LCMS2_FOUND "NO")
ENDIF (LCMS2_LIBRARY AND LCMS2_INCLUDE_DIR)


IF (LCMS2_FOUND)
   IF (NOT LCMS2_FIND_QUIETLY)
      MESSAGE(STATUS "Found LCMS2: ${LCMS2_LIBRARIES}")
   ENDIF (NOT LCMS2_FIND_QUIETLY)
ELSE (LCMS2_FOUND)
   IF (LCMS2_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find LCMS2 library")
   ENDIF (LCMS2_FIND_REQUIRED)
ENDIF (LCMS2_FOUND)

MARK_AS_ADVANCED(LCMS2_LIBRARY LCMS2_INCLUDE_DIR)
