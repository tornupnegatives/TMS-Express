# Author: Joseph Bellahcen <joeclb@icloud.com>

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Run Qt's moc, rcc, and uic tools
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

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
        #  ====================================
        #             BACKEND SOURCES
        #  ====================================
        src/Audio/AudioBuffer.cpp
        src/Audio/AudioFilter.cpp

        src/LPC_Analysis/Autocorrelation.cpp
        src/LPC_Analysis/PitchEstimator.cpp
        src/LPC_Analysis/LinearPredictor.cpp

        src/Frame_Encoding/Frame.cpp
        src/Frame_Encoding/FrameEncoder.cpp
        src/Frame_Encoding/FramePostprocessor.cpp
        src/Frame_Encoding/Synthesizer.cpp

        src/Bitstream_Generation/BitstreamGenerator.cpp
        src/Bitstream_Generation/PathUtils.cpp

        #  ====================================
        #             FRONTEND SOURCES
        #  ====================================
        src/User_Interfaces/CommandLineApp.cpp

        inc/User_Interfaces/MainWindow.h
        src/User_Interfaces/MainWindow.cpp

        inc/User_Interfaces/Audio_Waveform/AudioWaveform.h
        src/User_Interfaces/Audio_Waveform/AudioWaveform.cpp

        inc/User_Interfaces/Audio_Waveform/AudioWaveformView.h
        src/User_Interfaces/Audio_Waveform/AudioWaveformView.cpp

        inc/User_Interfaces/Control_Panels/ControlPanelView.h
        src/User_Interfaces/Control_Panels/ControlPanelView.cpp

        inc/User_Interfaces/Control_Panels/ControlPanelPitchView.h
        src/User_Interfaces/Control_Panels/ControlPanelPitchView.cpp

        inc/User_Interfaces/Control_Panels/ControlPanelLpcView.h
        src/User_Interfaces/Control_Panels/ControlPanelLpcView.cpp

        inc/User_Interfaces/Control_Panels/ControlPanelPostView.h
        src/User_Interfaces/Control_Panels/ControlPanelPostView.cpp

        src/main.cpp
        )

#  ====================================
#              DYNAMIC LIBS
#  ====================================
find_package(Qt6 COMPONENTS Core Gui Multimedia Widgets PrintSupport REQUIRED)

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
        Qt::PrintSupport
        Qt::Multimedia
        )

# Rename executable
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)

if (TMSEXPRESS_BUILD_TESTS)
    message(STATUS "Building TMS Express test suite")
    include(test/TmsTest.cmake)
endif()
