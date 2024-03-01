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

find_package(PkgConfig REQUIRED)

pkg_check_modules(
    SndFile
    REQUIRED
    IMPORTED_TARGET
    sndfile)

pkg_check_modules(
    SampleRate
    REQUIRED
    IMPORTED_TARGET
    samplerate)

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::SndFile
                                              PkgConfig::SampleRate)

###############################################################################
# Artifacts & Sub-Targets #####################################################
###############################################################################

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)

if(TMSEXPRESS_BUILD_TESTS)
    message(STATUS "Building TMS Express test suite")
    include(test/CMakeLists.txt)
endif()
