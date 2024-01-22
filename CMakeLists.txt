# Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

###############################################################################
# CMake Configuration #########################################################
###############################################################################

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-Wall -Wextra -Wpedantic)

# Required for Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

###############################################################################
# Project Configuration #######################################################
###############################################################################

project(TMS-Express)
option(TMSEXPRESS_BUILD_TESTS "Build test programs" ON)

###############################################################################
# Project Sources & Includes ##################################################
###############################################################################

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    tms_express
)

add_executable(${PROJECT_NAME}
    # Backend #################################################################
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

    # Frontend ###############################################################
    tms_express/ui/cli/CommandLineApp.cpp

    tms_express/ui/gui/audiowaveform/AudioWaveform.cpp
    tms_express/ui/gui/audiowaveform/AudioWaveformView.cpp

    tms_express/ui/gui/controlpanels/ControlPanelView.cpp
    tms_express/ui/gui/controlpanels/ControlPanelPitchView.cpp
    tms_express/ui/gui/controlpanels/ControlPanelLpcView.cpp
    tms_express/ui/gui/controlpanels/ControlPanelPostView.cpp

    tms_express/ui/gui/MainWindow.cpp

    # Main ####################################################################
    tms_express/main.cpp
)

###############################################################################
# Project Dependencies ########################################################
###############################################################################

find_package(Qt6 COMPONENTS Core Gui Multimedia Widgets REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(SndFile REQUIRED IMPORTED_TARGET sndfile)
pkg_check_modules(SampleRate REQUIRED IMPORTED_TARGET samplerate)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    PkgConfig::SndFile
    PkgConfig::SampleRate
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Multimedia
)

###############################################################################
# Artifacts & Sub-Targets #####################################################
###############################################################################

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)

if (TMSEXPRESS_BUILD_TESTS)
    message(STATUS "Building TMS Express test suite")
    include(test/CMakeLists.txt)
endif()
