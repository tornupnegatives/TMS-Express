cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)
project(TMS-Express)

# Dependency: libsndfile
find_library(Sndfile sndfile REQUIRED)
include_directories(${Sndfile_INCLUDE_DIR})

# Dependency: libsamplerate
find_library(Samplerate samplerate REQUIRED)
include_directories(${Samplerate_INCLUDE_DIR})

# Dependency: Boost::program_options
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

include(FetchContent)
FetchContent_Declare(CLI11
        GIT_REPOSITORY "https://github.com/CLIUtils/CLI11.git"
        GIT_TAG "v2.2.0"
        )
FetchContent_MakeAvailable(CLI11)

# Unit tests
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

        src/Interfaces/Encoder.cpp

        src/UserParameters.cpp
        src/main.cpp
        )

# Rename executable
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)

target_link_libraries(${PROJECT_NAME}
        ${Boost_LIBRARIES}
        CLI11
        ${Sndfile}
        ${Samplerate}
        )
