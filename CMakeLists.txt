cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)
project(TMS-Express)

# Locate libsndfile and libsamplerate
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Boost
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# Bring in project files
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

# Link dependencies
target_link_libraries(${PROJECT_NAME}
        ${Boost_LIBRARIES}
        sndfile
        samplerate
        )

# Unit tests
enable_testing()

add_executable(TMS-Test
        src/LPC_Analysis/Autocorrelator.cpp
        test/AutocorrelatorTests.cpp

        src/Frame_Encoding/Frame.cpp
        test/FrameTests.cpp

        src/Frame_Encoding/FrameEncoder.cpp
        test/FrameEncoderTests.cpp
        )

target_link_libraries(
        TMS-Test
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(TMS-Test)
