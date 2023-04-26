# Author: Joseph Bellahcen <joeclb@icloud.com>

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
project(TMS-Express)

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

        src/Interfaces/BitstreamGenerator.cpp
        src/Interfaces/PathUtils.cpp

        src/main.cpp
        )

#  ====================================
#              DYNAMIC LIBS
#  ====================================
#set(CMAKE_AUTOMOC ON)
#find_package(Qt6 REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(sndfile REQUIRED)
find_package(samplerate REQUIRED)

target_link_libraries(${PROJECT_NAME}
        SndFile::sndfile
        SampleRate::samplerate
        )

# Rename executable
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)
