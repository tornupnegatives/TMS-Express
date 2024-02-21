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

project(TMS-Express)
option(TMSEXPRESS_BUILD_TESTS "Build test programs" ON)
option(TMSEXPRESS_BUILD_GUI "Build GUI frontend" ON)

if(TMSEXPRESS_BUILD_GUI)
    # CMake provides the UIC, MOC, and RCC tools to aid in Qt app development
    # - UIC: Converts .UI files to C/C++ headers
    # - MOC: Produces "meta-object code" for Q_OBJECT classes
    # - RCC: Embeds resource files into Qt application binary
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    add_compile_definitions(TMSEXPRESS_GUI=1)
endif()

###############################################################################
# Project Sources & Includes ##################################################
###############################################################################

include_directories(${CMAKE_CURRENT_LIST_DIR} tms_express)

add_executable(
    ${PROJECT_NAME}
    tms_express/audio/AudioBuffer.cpp
    tms_express/audio/AudioFilter.cpp
    tms_express/analysis/Autocorrelation.cpp
    tms_express/analysis/PitchEstimator.cpp
    tms_express/analysis/LinearPredictor.cpp
    tms_express/encoding/Frame.cpp
    tms_express/encoding/FrameEncoder.cpp
    tms_express/encoding/FramePostprocessor.cpp
    tms_express/encoding/Synthesizer.cpp
    tms_express/bitstream/BitstreamGenerator.cpp
    tms_express/bitstream/PathUtils.cpp
    tms_express/ui/cli/CommandLineApp.cpp
    tms_express/main.cpp)

if(TMSEXPRESS_BUILD_GUI)
    target_sources(
        ${PROJECT_NAME}
        PRIVATE tms_express/ui/gui/audiowaveform/AudioWaveform.cpp
                tms_express/ui/gui/audiowaveform/AudioWaveformView.cpp
                tms_express/ui/gui/controlpanels/ControlPanelView.cpp
                tms_express/ui/gui/controlpanels/ControlPanelPitchView.cpp
                tms_express/ui/gui/controlpanels/ControlPanelLpcView.cpp
                tms_express/ui/gui/controlpanels/ControlPanelPostView.cpp
                tms_express/ui/gui/MainWindow.cpp)
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
