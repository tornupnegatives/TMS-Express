cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)
project(TMS-Express)

# Dependency: libsndfile and libsamplerate
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# Dependency: Boost::program_options
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# Unit tests
include(test/TmsTest.cmake)

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

        src/UserParameters.cpp
        src/main.cpp
        )

# Rename executable
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)

target_link_libraries(${PROJECT_NAME}
        ${Boost_LIBRARIES}
        sndfile
        samplerate
        )
