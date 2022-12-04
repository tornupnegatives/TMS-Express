cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
project(TMS-Express)

find_package(PkgConfig REQUIRED)

# Dependency: libsndfile
# Desc: Audio IO
# Sets: LIBSNDFILE_INCLUDE_DIR, LIBSNDFILE_LIBRARY
if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBSNDFILE_PKGCONF sndfile REQUIRED)
endif(PKG_CONFIG_FOUND)

find_path(LIBSNDFILE_INCLUDE_DIR
        NAMES sndfile.hh
        PATHS ${LIBSNDFILE_PKGCONF_INCLUDE_DIRS}
        )

find_library(LIBSNDFILE_LIBRARY
        NAMES sndfile libsndfile-1
        PATHS ${LIBSNDFILE_PKGCONF_LIBRARY_DIRS}
        )

include_directories(${LIBSNDFILE_INCLUDE_DIR})

# Dependency: libsamplerate
# Desc: Audio resampler
# Sets: LIBSAMPLERATE_INCLUDE_DIR, LIBSAMPLERATE_LIBRARY
if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBSAMPLERATE_PKGCONF samplerate REQUIRED)
endif(PKG_CONFIG_FOUND)

find_path(LIBSAMPLERATE_INCLUDE_DIR
        NAMES samplerate.h
        PATHS ${LIBSAMPLERATE_PKGCONF_INCLUDE_DIRS}
        )

find_library(LIBSAMPLERATE_LIBRARY
        NAMES samplerate libsamplerate-0
        PATHS ${LIBSAMPLERATE_PKGCONF_LIBRARY_DIRS}
        )

include_directories(${LIBSAMPLERATE_INCLUDE_DIR})

# Dependency: CLI11
# Desc: Command-line interface library
# Sets: CLI11
include(FetchContent)
FetchContent_Declare(CLI11
        GIT_REPOSITORY "https://github.com/CLIUtils/CLI11.git"
        GIT_TAG "v2.2.0"
        )
FetchContent_MakeAvailable(CLI11)

# Enable unit testing if TMS_TEST is set in the environment
if (DEFINED ENV{TMS_TEST})
    include(test/TmsTest.cmake)
endif()

# Project files
include_directories(inc/)
add_executable(${PROJECT_NAME}
        src/Audio/AudioBuffer.cpp
        src/Audio/AudioPreprocessor.cpp

        src/LPC_Analysis/Autocorrelator.cpp
        src/LPC_Analysis/PitchEstimator.cpp
        src/LPC_Analysis/LinearPredictor.cpp

        src/Frame_Encoding/Frame.cpp
        src/Frame_Encoding/FrameEncoder.cpp
        src/Frame_Encoding/FramePostprocessor.cpp

        src/Interfaces/BitstreamGenerator.cpp
        src/Interfaces/PathUtils.cpp

        src/main.cpp
        )

# Rename executable
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)

# Link dependencies
target_link_libraries(${PROJECT_NAME}
        CLI11
        ${LIBSNDFILE_LIBRARY}
        ${LIBSAMPLERATE_LIBRARY}
        )
