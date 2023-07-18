# Author: Joseph Bellahcen <joeclb@icloud.com>

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

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
    tms_express
    lib
)

add_executable(${PROJECT_NAME}
    #  ====================================
    #             BACKEND SOURCES 
    #  ====================================
    tms_express/Audio/AudioBuffer.cpp
    tms_express/Audio/AudioFilter.cpp

    tms_express/LPC_Analysis/Autocorrelation.cpp
    tms_express/LPC_Analysis/PitchEstimator.cpp
    tms_express/LPC_Analysis/LinearPredictor.cpp

    tms_express/Frame_Encoding/Frame.cpp
    tms_express/Frame_Encoding/FrameEncoder.cpp
    tms_express/Frame_Encoding/FramePostprocessor.cpp
    tms_express/Frame_Encoding/Synthesizer.cpp

    tms_express/Bitstream_Generation/BitstreamGenerator.cpp
    tms_express/Bitstream_Generation/PathUtils.cpp

    #  ====================================
    #             FRONTEND SOURCES
    #  ====================================
    tms_express/User_Interfaces/CommandLineApp.cpp

    tms_express/User_Interfaces/MainWindow.cpp

    tms_express/User_Interfaces/Audio_Waveform/AudioWaveform.cpp
    tms_express/User_Interfaces/Audio_Waveform/AudioWaveformView.cpp

    tms_express/User_Interfaces/Control_Panels/ControlPanelView.cpp
    tms_express/User_Interfaces/Control_Panels/ControlPanelPitchView.cpp
    tms_express/User_Interfaces/Control_Panels/ControlPanelLpcView.cpp
    tms_express/User_Interfaces/Control_Panels/ControlPanelPostView.cpp

    tms_express/main.cpp
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
