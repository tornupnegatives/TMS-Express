###############################################################################
# Project: TMS-Express
#
# File: FindCLI11.cmake
#
# Description:   Imports libsndfile and libsamplerate via pkg-config
#                Sets LIBSNDFILE_INCLUDE_DIR, LIBSNDFILE_LIBRARY,
#                LIBSAMPLERATE_INCLUDE_DIR, LIBSAMPLERATE_LIBRARY
#
# Author: Joseph Bellahcen <joeclb@icloud.com>
###############################################################################

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBSNDFILE_PKGCONF sndfile REQUIRED)
pkg_check_modules(LIBSAMPLERATE_PKGCONF samplerate REQUIRED)

find_path(LIBSNDFILE_INCLUDE_DIR
        NAMES sndfile.hh
        PATHS ${LIBSNDFILE_PKGCONF_INCLUDE_DIRS}
        )

find_library(LIBSNDFILE_LIBRARY
        NAMES sndfile libsndfile-1
        PATHS ${LIBSNDFILE_PKGCONF_LIBRARY_DIRS}
        )
        
find_path(LIBSAMPLERATE_INCLUDE_DIR
        NAMES samplerate.h
        PATHS ${LIBSAMPLERATE_PKGCONF_INCLUDE_DIRS}
        )

find_library(LIBSAMPLERATE_LIBRARY
        NAMES samplerate libsamplerate-0
        PATHS ${LIBSAMPLERATE_PKGCONF_LIBRARY_DIRS}
        )

