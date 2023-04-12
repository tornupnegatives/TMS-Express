# Author: Joseph Bellahcen <joeclb@icloud.com>

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
project(TMS-Express)

#  ====================================
#             PROGRAM FILES
#  ====================================
include_directories(inc/)
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
#              DEPENDENCIES
#  ====================================
include(vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package(SndFile CONFIG REQUIRED)
find_package(SampleRate CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        SndFile::sndfile
        SampleRate::samplerate
        nlohmann_json::nlohmann_json
        CLI11::CLI11
        )

# Rename executable
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)
