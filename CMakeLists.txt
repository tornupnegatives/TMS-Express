# Author: Joseph Bellahcen <joeclb@icloud.com>

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

project(TMS-Express)
option(TMSEXPRESS_BUILD_TESTS "Build test programs" ON)

#  ====================================
#             PROGRAM FILES
#  ====================================
include_directories(
    inc
    lib
    )

add_executable(${PROJECT_NAME}
        src/Audio/AudioBuffer.cpp
        src/Audio/AudioFilter.cpp

        src/LPC_Analysis/Autocorrelator.cpp
        src/LPC_Analysis/PitchEstimator.cpp
        src/LPC_Analysis/LinearPredictor.cpp

        src/Frame_Encoding/Frame.cpp
        src/Frame_Encoding/FrameEncoder.cpp
        src/Frame_Encoding/FramePostprocessor.cpp

        src/Bitstream_Generation/BitstreamGenerator.cpp
        src/Bitstream_Generation/PathUtils.cpp

        src/main.cpp
        )

#  ====================================
#              DYNAMIC LIBS
#  ====================================
find_package(Qt6Gui REQUIRED)
find_package(Qt6Widgets REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SndFile REQUIRED IMPORTED_TARGET sndfile)
pkg_check_modules(SampleRate REQUIRED IMPORTED_TARGET samplerate)

target_link_libraries(${PROJECT_NAME}
        PkgConfig::SndFile
        PkgConfig::SampleRate
        Qt::Gui
        Qt::Widgets
        )

# Rename executable
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)

if (TMSEXPRESS_BUILD_TESTS)
    message(STATUS "Building TMS Express test suite")
    include(test/TmsTest.cmake)
endif()
