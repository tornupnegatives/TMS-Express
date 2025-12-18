# Copyright (C) 2024 Joseph Bellahcen <joeclb@icloud.com>

###############################################################################
# CMake Configuration #########################################################
###############################################################################

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-Wall -Wextra -Wpedantic)

###############################################################################
# Project Configuration #######################################################
###############################################################################

project(tmsexpress)
option(TMSEXPRESS_BUILD_TESTS "Build test programs" ON)
option(TMSEXPRESS_BUILD_GUI "Build GUI frontend" ON)

if(TMSEXPRESS_BUILD_GUI)
    # Converts Qt designer (.UI) files to C/C++ headers
    # set(CMAKE_AUTOUIC ON)

    # Produces "meta-object code" for Qt Q_OBJECT classes
    set(CMAKE_AUTOMOC ON)

    # Embeds resource files into Qt application binary
    # set(CMAKE_AUTORCC ON)

    # Configures main method to include Qt application object
    add_compile_definitions(TMSEXPRESS_GUI=1)
endif()

###############################################################################
# Project Sources & Includes ##################################################
###############################################################################

include_directories(${CMAKE_CURRENT_LIST_DIR} src)

add_executable(
    ${PROJECT_NAME}
    src/audio/AudioBuffer.cpp
    src/audio/AudioFilter.cpp
    src/analysis/Autocorrelation.cpp
    src/analysis/PitchEstimator.cpp
    src/analysis/LinearPredictor.cpp
    src/encoding/Frame.cpp
    src/encoding/FrameEncoder.cpp
    src/encoding/FramePostprocessor.cpp
    src/encoding/Synthesizer.cpp
    src/bitstream/BitstreamGenerator.cpp
    src/bitstream/PathUtils.cpp
    src/ui/cli/CommandLineApp.cpp
    src/main.cpp)

if(TMSEXPRESS_BUILD_GUI)
    target_sources(
        ${PROJECT_NAME}
        PRIVATE src/ui/gui/audiowaveform/AudioWaveform.cpp
                src/ui/gui/audiowaveform/AudioWaveformView.cpp
                src/ui/gui/controlpanels/ControlPanelView.cpp
                src/ui/gui/controlpanels/ControlPanelPitchView.cpp
                src/ui/gui/controlpanels/ControlPanelLpcView.cpp
                src/ui/gui/controlpanels/ControlPanelPostView.cpp
                src/ui/gui/MainWindow.cpp)
endif()

###############################################################################
# Project Dependencies ########################################################
###############################################################################

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
)

# TMS Express uses QT for its graphical interface. Unfortunately, QT does not
# play nicely with CMake's find_package tool, nor does it integrate well with
# pkg-config. The host must ensure that QT is installed and present in the path

if(TMSEXPRESS_BUILD_GUI)
    find_package(
        Qt6
        COMPONENTS Core
                   Gui
                   Multimedia
                   Widgets
        REQUIRED)

    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE Qt::Core
                Qt::Gui
                Qt::Widgets
                Qt::Multimedia)
endif()

# TMS Express uses libsndfile for audio I/O. The host must ensure that
# libsndfile is installed, but CMake can detect it via pkg-config. Note
# that pkg-config must also be installed

find_package(PkgConfig REQUIRED)

pkg_check_modules(
    SndFile
    REQUIRED
    IMPORTED_TARGET
    sndfile)

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::SndFile)

# The bulk of TMS Express' dependencies may be downloaded and configured using

file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

cpmaddpackage(
    CLI11
    GITHUB_REPOSITORY
    CLIUtils/CLI11
    VERSION
    2.6.1)

cpmaddpackage(
    json
    GITHUB_REPOSITORY
    nlohmann/json
    VERSION
    3.12.0)

cpmaddpackage(
    samplerate
    GITHUB_REPOSITORY
    libsndfile/libsamplerate
    GIT_TAG
    0.2.2
    OPTIONS
    "BUILD_TESTING OFF")

# nlohmann's JSON library is exposed by CMake as an interface, so it is best
# incorporated into TMS Express as a header, rather than attempting to link it
include_directories(${json_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE CLI11 samplerate)

###############################################################################
# Artifacts & Sub-Targets #####################################################
###############################################################################

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)

if(TMSEXPRESS_BUILD_TESTS)
    message(STATUS "Building TMS Express test suite")
    include(test/CMakeLists.txt)
endif()
